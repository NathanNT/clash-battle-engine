#include "simulation_bridge.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace cocsim;

namespace {

void require(bool condition, const char* expression) {
  if (!condition) {
    std::cerr << "assertion failed: " << expression << '\n';
    std::exit(EXIT_FAILURE);
  }
}

template <class Predicate>
std::shared_ptr<const viewer::PresentationSnapshot> wait_for(Predicate predicate) {
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{1};
  while (std::chrono::steady_clock::now() < deadline) {
    if (auto frame = predicate()) return frame;
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
  return {};
}

} // namespace

int main() {
  const auto data = GameData::v0();
  Scenario scenario;
  scenario.width = 20;
  scenario.height = 20;
  scenario.duration_ms = 5'000;
  scenario.defenders = {{Kind::GoldMine, 1, {15.5, 10.5}}};
  scenario.army = {{Kind::Barbarian, 1, 2}};

  viewer::SimulationBridge bridge(data, scenario);
  // Reset while paused establishes an unambiguous T+0 baseline despite the
  // constructor having started its independent simulation owner.
  bridge.set_paused(true);
  bridge.request_reset();
  const auto reset = wait_for([&] {
    const auto frame = bridge.latest();
    return frame && frame->now_ms == 0 ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(reset), "paused reset publishes T+0");

  // GUI submits only a value command. The owner converts it to its next legal
  // fixed boundary, then a single requested tick applies it. No SDL state or
  // mutable BattleState reference crosses the thread boundary.
  bridge.enqueue({CommandType::Deploy, Kind::Barbarian, 1, {0.5, 10.5}, SpellKind::Rage});
  bridge.enqueue({CommandType::Deploy, Kind::Barbarian, 1, {0.5, 11.5}, SpellKind::Rage});
  // A queued command at T+10 is resolved at the beginning of the following
  // fixed update, matching BattleState's public advance_ticks(2) contract.
  bridge.request_step();
  bridge.request_step();
  const auto deployed = wait_for([&] {
    const auto frame = bridge.latest();
    if (!frame || frame->now_ms < 2 * kTickMs) return std::shared_ptr<const viewer::PresentationSnapshot>{};
    const auto attackers = std::count_if(frame->entities.begin(), frame->entities.end(), [](const EntityView& entity) {
      return entity.side == Side::Attacker && entity.kind == Kind::Barbarian;
    });
    return attackers == 2 ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(deployed), "queued deployment is applied at the fixed tick");
  require(deployed->army.size() == 1 && deployed->army.front().count == 0,
          "published inventory belongs to the same immutable frame");
  // The bridge must be a transport boundary, not a second combat
  // implementation. Its exact fixed-tick state matches the same value command
  // submitted directly to Core at the explicit T+10 boundary.
  BattleState direct(data, scenario);
  require(direct.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {0.5, 10.5}}),
          "direct baseline command is accepted");
  require(direct.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {0.5, 11.5}}),
          "same-tick direct command preserves sequence");
  direct.advance_ticks(2);
  require(deployed->state_hash == direct.state_hash(),
          "bridge and direct Core have identical deterministic state");
  // Holding a GUI frame cannot expose a buffer that the SIM thread later
  // overwrites. A subsequent publication must be a distinct immutable value.
  const auto held_frame = deployed;
  const auto held_generation = held_frame->generation;
  const auto held_hash = held_frame->state_hash;
  bridge.request_step();
  const auto advanced = wait_for([&] {
    const auto frame = bridge.latest();
    return frame && frame->generation > held_generation && frame->now_ms >= 3 * kTickMs
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(advanced), "manual step publishes a newer immutable frame");
  require(held_frame->generation == held_generation && held_frame->state_hash == held_hash,
          "held GUI frame remains immutable after buffer rotation");

  // Exercise the actual concurrent ownership boundary: a GUI reader keeps
  // obtaining and traversing immutable frames while the simulation publishes
  // twenty independently stepped states.  The end hash must still equal a
  // direct Core run at the same fixed tick.
  std::atomic<bool> reader_running{true};
  std::atomic<bool> reader_failed{false};
  std::atomic<std::uint64_t> reader_reads{};
  std::thread gui_reader([&] {
    while (reader_running.load(std::memory_order_acquire)) {
      const auto frame = bridge.latest();
      if (!frame) continue;
      const auto generation = frame->generation;
      const auto hash = frame->state_hash;
      std::size_t entity_count{};
      for (const auto& entity : frame->entities) entity_count += entity.id != 0;
      if (frame->generation != generation || frame->state_hash != hash || entity_count > frame->entities.size())
        reader_failed.store(true, std::memory_order_release);
      reader_reads.fetch_add(1, std::memory_order_relaxed);
      std::this_thread::yield();
    }
  });
  for (int step = 0; step < 20; ++step) {
    const auto prior_generation = bridge.latest()->generation;
    bridge.request_step();
    const auto next = wait_for([&] {
      const auto frame = bridge.latest();
      return frame && frame->generation > prior_generation
        ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
    });
    require(bool(next), "each stepped state is published while GUI reads");
  }
  reader_running.store(false, std::memory_order_release);
  gui_reader.join();
  require(reader_reads.load(std::memory_order_relaxed) > 0 && !reader_failed.load(std::memory_order_acquire),
          "concurrent GUI reader sees coherent immutable frames");
  direct.advance_ticks(21); // one held-frame step plus twenty concurrent steps
  const auto final_frame = bridge.latest();
  require(final_frame && final_frame->state_hash == direct.state_hash(),
          "concurrent presentation does not change deterministic Core state");
  bridge.stop();

  // A reset creates a command-queue cut: already queued input is discarded,
  // while input submitted after that cut belongs to the replacement battle.
  viewer::SimulationBridge reset_bridge(data, scenario);
  reset_bridge.set_paused(true);
  reset_bridge.request_reset();
  require(bool(wait_for([&] {
    const auto frame = reset_bridge.latest();
    return frame && frame->now_ms == 0 ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  })), "reset queue test establishes T+0");
  reset_bridge.enqueue({CommandType::Deploy, Kind::Barbarian, 1, {0.5, 10.5}, SpellKind::Rage});
  reset_bridge.request_reset();
  reset_bridge.request_step();
  reset_bridge.request_step();
  const auto discarded = wait_for([&] {
    const auto frame = reset_bridge.latest();
    return frame && frame->now_ms >= 2 * kTickMs ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(discarded) && discarded->army.front().count == 2
            && std::none_of(discarded->entities.begin(), discarded->entities.end(), [](const EntityView& entity) {
                 return entity.side == Side::Attacker;
               }), "reset discards commands from the prior battle");
  reset_bridge.request_reset();
  reset_bridge.enqueue({CommandType::Deploy, Kind::Barbarian, 1, {0.5, 10.5}, SpellKind::Rage});
  reset_bridge.request_step();
  reset_bridge.request_step();
  const auto after_reset = wait_for([&] {
    const auto frame = reset_bridge.latest();
    if (!frame || frame->now_ms < 2 * kTickMs) return std::shared_ptr<const viewer::PresentationSnapshot>{};
    return std::any_of(frame->entities.begin(), frame->entities.end(), [](const EntityView& entity) {
      return entity.side == Side::Attacker;
    }) ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(after_reset) && after_reset->army.front().count == 1,
          "post-reset command is delivered to the fresh battle");
  reset_bridge.stop();
  return EXIT_SUCCESS;
}
