#include "simulation_bridge.hpp"
#include "replay_io.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
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
  // A queued command at T+16 is resolved at the beginning of the following
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
  // submitted directly to Core at the explicit T+16 boundary.
  BattleState direct(data, scenario);
  require(direct.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {0.5, 10.5}}),
          "direct baseline command is accepted");
  require(direct.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {0.5, 11.5}}),
          "same-tick direct command preserves sequence");
  direct.advance_ticks(2);
  require(deployed->state_hash == direct.state_hash(),
          "bridge and direct Core have identical deterministic state");
  require(deployed->monolith_arrow_housing == direct.monolith_arrow_deployed_housing()
          && deployed->monolith_arrow_tier == direct.monolith_arrow_housing_tier()
          && deployed->monolith_arrow_damage_percent == direct.monolith_arrow_damage_percent(),
          "Viewer housing projection is published from the exact Core state");
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

  // Maximum playback removes only presentation pacing. It must still produce
  // the same fixed-tick terminal state as a direct Core run.
  viewer::SimulationBridge maximum_bridge(data, scenario);
  maximum_bridge.set_maximum_playback(true);
  const auto maximum_frame = wait_for([&] {
    const auto frame = maximum_bridge.latest();
    return frame && frame->result.finished ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(maximum_frame) && maximum_frame->result.timed_out,
          "maximum playback reaches the fixed time limit");
  BattleState direct_maximum(data, scenario);
  direct_maximum.advance_to(((scenario.duration_ms + kTickMs - 1) / kTickMs) * kTickMs);
  require(maximum_frame->state_hash == direct_maximum.state_hash(),
          "maximum playback preserves the direct fixed-tick state");
  maximum_bridge.stop();

  // A replay is submitted before the first visible T+0 frame and again after
  // Reset. Its serialized timestamps and sequence must therefore match a
  // direct Core replay without being re-timed by the Viewer thread.
  const std::vector<Command> replay_commands = {
    {CommandType::Deploy, kTickMs, kTickMs, 1, Kind::Barbarian, 1, {0.5, 10.5}},
    {CommandType::Deploy, 3 * kTickMs, 3 * kTickMs, 2, Kind::Barbarian, 1, {0.5, 11.5}},
  };
  viewer::SimulationBridge replay_bridge(data, scenario, replay_commands);
  replay_bridge.set_paused(true);
  replay_bridge.request_reset();
  const auto replay_start = wait_for([&] {
    const auto frame = replay_bridge.latest();
    const bool commands_match = frame && frame->commands.size() == replay_commands.size()
      && std::equal(frame->commands.begin(), frame->commands.end(), replay_commands.begin(), [](const Command& left, const Command& right) {
        return left.type == right.type && left.requested_ms == right.requested_ms
          && left.effective_ms == right.effective_ms && left.sequence == right.sequence
          && left.kind == right.kind && left.level == right.level
          && left.position.x == right.position.x && left.position.y == right.position.y
          && left.spell == right.spell;
      });
    return frame && frame->now_ms == 0 && commands_match
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(replay_start), "replay publishes its serialized command log at T+0");
  replay_bridge.request_step();
  replay_bridge.request_step();
  replay_bridge.request_step();
  replay_bridge.request_step();
  const auto replay_frame = wait_for([&] {
    const auto frame = replay_bridge.latest();
    return frame && frame->now_ms >= 4 * kTickMs ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  BattleState direct_replay(data, scenario);
  for (const auto& command : replay_commands) require(direct_replay.submit(command), "direct replay command is accepted");
  direct_replay.advance_ticks(4);
  require(bool(replay_frame) && replay_frame->state_hash == direct_replay.state_hash(),
          "replay bridge preserves serialized command timing and order");
  replay_bridge.request_reset();
  replay_bridge.request_step();
  replay_bridge.request_step();
  replay_bridge.request_step();
  replay_bridge.request_step();
  const auto replay_reset_frame = wait_for([&] {
    const auto frame = replay_bridge.latest();
    return frame && frame->now_ms >= 4 * kTickMs ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(replay_reset_frame) && replay_reset_frame->state_hash == direct_replay.state_hash(),
          "replay reset returns to the same T+0 command schedule");
  replay_bridge.stop();

  // A reset creates a command-queue cut: already queued input is discarded,
  // while input submitted after that cut belongs to the replacement battle.
  viewer::SimulationBridge reset_bridge(data, scenario);
  reset_bridge.set_paused(true);
  const auto initial_generation = reset_bridge.latest()->generation;
  reset_bridge.request_reset();
  require(bool(wait_for([&] {
    const auto frame = reset_bridge.latest();
    return frame && frame->generation > initial_generation && frame->now_ms == 0
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  })), "reset queue test establishes T+0");
  reset_bridge.enqueue({CommandType::Deploy, Kind::Barbarian, 1, {0.5, 10.5}, SpellKind::Rage});
  const auto discard_reset_generation = reset_bridge.latest()->generation;
  reset_bridge.request_reset();
  require(bool(wait_for([&] {
    const auto frame = reset_bridge.latest();
    return frame && frame->generation > discard_reset_generation && frame->now_ms == 0
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  })), "reset publishes the command-queue cut before stepping");
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
  const auto fresh_reset_generation = reset_bridge.latest()->generation;
  reset_bridge.request_reset();
  require(bool(wait_for([&] {
    const auto frame = reset_bridge.latest();
    return frame && frame->generation > fresh_reset_generation && frame->now_ms == 0
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  })), "second reset publishes the fresh battle before input");
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

  Scenario equipped_scenario;
  equipped_scenario.width = 20;
  equipped_scenario.height = 20;
  equipped_scenario.duration_ms = 1000;
  equipped_scenario.defenders = {{Kind::GoldMine, 1, {15.5, 10.5}}};
  equipped_scenario.army = {{Kind::ArcherQueen, 110, 1}};
  equipped_scenario.hero_loadouts = {
    {Kind::ArcherQueen, HeroSupportChoice{"lassi", 15},
      {{"giant_arrow", 18}, {"monolith_arrow", 27}}},
  };
  viewer::SimulationBridge equipped_bridge(data, equipped_scenario);
  equipped_bridge.set_paused(true);
  equipped_bridge.request_reset();
  const auto equipped_frame = wait_for([&] {
    const auto frame = equipped_bridge.latest();
    return frame && frame->now_ms == 0 && frame->hero_loadouts.size() == 1
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(equipped_frame) && equipped_frame->hero_loadouts[0].pet
          && equipped_frame->hero_loadouts[0].pet->id == "lassi",
          "Viewer publishes the source-validated Hero loadout as values");
  BattleState equipped_direct(data, equipped_scenario);
  require(equipped_frame->state_hash == equipped_direct.state_hash(),
          "Viewer loadout frame comes from the same Core scenario");
  equipped_bridge.stop();

  auto editable_bridge = std::make_unique<viewer::SimulationBridge>(data, equipped_scenario);
  const auto* original_bridge = editable_bridge.get();
  Scenario invalid_loadout = equipped_scenario;
  invalid_loadout.hero_loadouts[0].equipment.push_back({"magic_mirror", 27});
  std::string selection_error;
  require(!viewer::replace_simulation_bridge(editable_bridge, data, invalid_loadout, {}, selection_error)
          && editable_bridge.get() == original_bridge && !selection_error.empty(),
          "invalid Viewer selection leaves the existing Core battle running");
  Scenario selected_loadout = equipped_scenario;
  selected_loadout.hero_loadouts[0].pet = HeroSupportChoice{"electro_owl", 15};
  selected_loadout.non_combat_obstacles = {{Kind::HeroBanner, {5.5, 5.5}, "dragon_duke"}};
  require(viewer::replace_simulation_bridge(editable_bridge, data, selected_loadout, {}, selection_error)
          && editable_bridge.get() != original_bridge && selection_error.empty(),
          "valid Viewer selection restarts through the Core bridge");
  const auto selected_frame = editable_bridge->latest();
  require(selected_frame && selected_frame->hero_loadouts[0].pet
          && selected_frame->hero_loadouts[0].pet->id == "electro_owl",
          "replacement frame publishes the selected Pet value");
  editable_bridge->stop();

  const std::vector<Command> equipped_commands = {
    {CommandType::Deploy, kTickMs, kTickMs, 1, Kind::ArcherQueen, 110, {0.5, 10.5}},
  };
  const std::string equipped_replay_path = "viewer-hero-loadout-replay-test.json";
  std::string equipped_replay_error;
  require(save_replay(equipped_replay_path, selected_loadout, equipped_commands, equipped_replay_error),
          "selected Hero loadout replay is saved as values");
  viewer::ViewerReplay equipped_replay;
  require(viewer::load_viewer_replay(equipped_replay_path, equipped_replay, equipped_replay_error),
          "Viewer reloads the selected Hero loadout replay");
  require(equipped_replay.scenario.hero_loadouts[0].pet
          && equipped_replay.scenario.hero_loadouts[0].pet->id == "electro_owl",
          "Viewer replay retains the selected Pet");
  require(equipped_replay.scenario.non_combat_obstacles.size() == 1
          && equipped_replay.scenario.non_combat_obstacles[0].variant == "dragon_duke",
          "Viewer replay retains the selected Hero Banner image");
  viewer::SimulationBridge equipped_replay_bridge(data, equipped_replay.scenario, equipped_replay.commands);
  equipped_replay_bridge.set_paused(true);
  equipped_replay_bridge.request_reset();
  const auto equipped_replay_start = wait_for([&] {
    const auto frame = equipped_replay_bridge.latest();
    return frame && frame->now_ms == 0 && frame->commands.size() == 1
      && frame->hero_loadouts.size() == 1 && frame->hero_loadouts[0].pet
      && frame->hero_loadouts[0].pet->id == "electro_owl"
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  require(bool(equipped_replay_start), "Viewer replay publishes selected values at T+0");
  equipped_replay_bridge.request_step();
  equipped_replay_bridge.request_step();
  const auto equipped_replay_frame = wait_for([&] {
    const auto frame = equipped_replay_bridge.latest();
    return frame && frame->now_ms >= 2 * kTickMs
      ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
  });
  BattleState equipped_replay_direct(data, equipped_replay.scenario);
  require(equipped_replay_direct.submit(equipped_commands.front()),
          "selected Hero loadout replay command is accepted by Core");
  equipped_replay_direct.advance_ticks(2);
  require(bool(equipped_replay_frame)
          && equipped_replay_frame->state_hash == equipped_replay_direct.state_hash(),
          "Viewer selected-loadout replay matches direct Core at fixed ticks");
  equipped_replay_bridge.stop();
  require(std::remove(equipped_replay_path.c_str()) == 0,
          "selected Hero loadout replay fixture is removed");

  Scenario warden_scenario = equipped_scenario;
  warden_scenario.army = {{Kind::GrandWarden, 85, 1, "air"}};
  warden_scenario.hero_loadouts.clear();
  const std::vector<Command> warden_commands = {
    {CommandType::Deploy, kTickMs, kTickMs, 1, Kind::GrandWarden, 85, {0.5, 10.5}},
  };
  std::array<std::uint64_t, 2> warden_hashes{};
  for (std::size_t mode_index = 0; mode_index < 2; ++mode_index) {
    const bool air = mode_index == 0;
    warden_scenario.army[0].mode = air ? "air" : "ground";
    const std::string path = air ? "viewer-warden-air-replay-test.json"
                                 : "viewer-warden-ground-replay-test.json";
    std::string error;
    require(save_replay(path, warden_scenario, warden_commands, error),
            "Viewer Warden mode replay is saved as values");
    viewer::ViewerReplay loaded;
    require(viewer::load_viewer_replay(path, loaded, error)
            && loaded.scenario.army[0].mode == warden_scenario.army[0].mode,
            "Viewer replay restores the selected Warden mode");
    viewer::SimulationBridge bridge(data, loaded.scenario, loaded.commands);
    bridge.set_paused(true);
    bridge.request_reset();
    require(bool(wait_for([&] {
      const auto frame = bridge.latest();
      return frame && frame->now_ms == 0 && frame->commands.size() == 1
        ? frame : std::shared_ptr<const viewer::PresentationSnapshot>{};
    })), "Viewer Warden replay starts at T+0");
    bridge.request_step();
    bridge.request_step();
    const auto frame = wait_for([&] {
      const auto candidate = bridge.latest();
      return candidate && candidate->now_ms >= 2 * kTickMs
        ? candidate : std::shared_ptr<const viewer::PresentationSnapshot>{};
    });
    BattleState direct(data, loaded.scenario);
    require(direct.submit(warden_commands.front()), "Core accepts Warden replay deploy");
    direct.advance_ticks(2);
    require(bool(frame) && frame->state_hash == direct.state_hash(),
            "Viewer Warden mode replay matches Core at fixed ticks");
    const auto hero = std::find_if(frame->entities.begin(), frame->entities.end(), [](const EntityView& entity) {
      return entity.kind == Kind::GrandWarden && entity.side == Side::Attacker;
    });
    require(hero != frame->entities.end() && hero->flying == air,
            "Viewer presents the Core Warden ground or air state");
    warden_hashes[mode_index] = frame->state_hash;
    bridge.stop();
    require(std::remove(path.c_str()) == 0, "Viewer Warden replay fixture is removed");
  }
  require(warden_hashes[0] != warden_hashes[1],
          "Warden modes produce distinct future-influencing Core hashes");
  return EXIT_SUCCESS;
}
