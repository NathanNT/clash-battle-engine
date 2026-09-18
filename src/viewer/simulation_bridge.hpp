#pragma once

#include "cocsim/core.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace cocsim::viewer {

// A complete, immutable read model for one presented frame.  It contains
// values only: neither BattleState nor an Entity pointer can cross from the
// simulation owner to the GUI thread.
struct PresentationSnapshot {
  std::uint64_t generation{};
  Milliseconds now_ms{};
  // A diagnostic of the serialisable, future-influencing Core state at this
  // exact tick. It is copied for verification only; GUI code never feeds it
  // back into the simulation.
  std::uint64_t state_hash{};
  BattleResult result{};
  int deployed_housing{};
  std::vector<ArmySlot> army;
  std::vector<SpellSlot> spells;
  // Exact Core-accepted player commands, including their rule-effective time
  // and stable sequence. This is copied by value with the frame so the GUI
  // can save a replay without owning BattleState.
  std::vector<Command> commands;
  std::vector<EntityView> entities;
  std::vector<SpellEffectView> spell_effects;
  std::vector<DeathExplosionView> death_explosions;
  std::vector<ProjectileView> projectiles;
  // Events are a bounded tail, addressed by their original Core index.  A
  // slow GUI may skip cosmetic traces, but cannot make the simulation retain
  // or replay a mutable event history.
  std::size_t first_event_index{};
  std::size_t event_count{};
  std::vector<Event> events;
};

struct QueuedCommand {
  CommandType type{CommandType::Wait};
  Kind kind{Kind::Barbarian};
  int level{1};
  Vec2 position{};
  SpellKind spell{SpellKind::Rage};
  // Assigned while holding the queue mutex. It defines a total order for a
  // same-tick GUI batch before BattleState assigns its serialised sequence.
  std::uint64_t sequence{};
};

// SimulationBridge is the sole BattleState owner.  The presentation thread
// communicates by message passing only; the Core remains free of SDL, threads
// and wall-clock decisions.
class SimulationBridge {
 public:
  SimulationBridge(const GameData& data, Scenario scenario)
      : data_(data), scenario_(std::move(scenario)) {
    BattleState initial(data_, scenario_);
    publish(initial);
    simulation_thread_ = std::thread([this] { run(); });
  }

  ~SimulationBridge() { stop(); }
  SimulationBridge(const SimulationBridge&) = delete;
  SimulationBridge& operator=(const SimulationBridge&) = delete;

  void stop() {
    if (!running_.exchange(false)) return;
    wake_.notify_all();
    if (simulation_thread_.joinable()) simulation_thread_.join();
  }

  [[nodiscard]] std::shared_ptr<const PresentationSnapshot> latest() const {
    const auto index = published_slot_.load(std::memory_order_acquire);
    return std::atomic_load_explicit(&buffers_[index], std::memory_order_acquire);
  }

  void enqueue(QueuedCommand command) {
    {
      std::lock_guard lock(command_mutex_);
      command.sequence = next_command_sequence_++;
      commands_.push_back(std::move(command));
    }
    wake_.notify_one();
  }

  void set_paused(bool value) {
    paused_.store(value, std::memory_order_release);
    wake_.notify_one();
  }
  [[nodiscard]] bool paused() const { return paused_.load(std::memory_order_acquire); }

  void request_step() {
    step_requests_.fetch_add(1, std::memory_order_release);
    wake_.notify_one();
  }
  void request_reset() {
    // Establish the reset cut at the same serialisation point as enqueue().
    // Commands committed before this marker belong to the discarded battle;
    // commands committed after it are valid input for the fresh battle.
    {
      std::lock_guard lock(command_mutex_);
      reset_through_sequence_.store(next_command_sequence_ - 1, std::memory_order_release);
    }
    reset_requested_.store(true, std::memory_order_release);
    wake_.notify_one();
  }
  void set_playback_speed(std::uint32_t value) {
    playback_speed_.store(std::max(1u, value), std::memory_order_release);
    wake_.notify_one();
  }
  [[nodiscard]] std::uint32_t playback_speed() const {
    return playback_speed_.load(std::memory_order_acquire);
  }
  void set_maximum_playback(bool value) {
    maximum_playback_.store(value, std::memory_order_release);
    wake_.notify_one();
  }
  [[nodiscard]] bool maximum_playback() const {
    return maximum_playback_.load(std::memory_order_acquire);
  }

 private:
  void publish(const BattleState& battle) {
    auto frame = std::make_shared<PresentationSnapshot>();
    frame->generation = generation_++;
    frame->now_ms = battle.now();
    frame->state_hash = battle.state_hash();
    frame->result = battle.result();
    frame->deployed_housing = battle.deployed_housing();
    frame->army = battle.scenario().army;
    frame->spells = battle.scenario().spells;
    frame->commands = battle.commands();
    frame->entities = battle.observe();
    frame->spell_effects = battle.observe_spell_effects();
    frame->death_explosions = battle.observe_death_explosions();
    frame->projectiles = battle.observe_projectiles();
    const auto& all_events = battle.events();
    constexpr std::size_t kEventTail = 512;
    frame->first_event_index = all_events.size() > kEventTail ? all_events.size() - kEventTail : 0;
    frame->event_count = all_events.size();
    frame->events.assign(all_events.begin() + static_cast<std::ptrdiff_t>(frame->first_event_index), all_events.end());

    const auto next = (published_slot_.load(std::memory_order_relaxed) + 1) % buffers_.size();
    // The published handle deliberately erases mutability before its atomic
    // hand-off.  MSVC needs this explicit const-qualified shared_ptr rather
    // than deducing it from a mutable make_shared result.
    std::shared_ptr<const PresentationSnapshot> published = std::move(frame);
    std::atomic_store_explicit(&buffers_[next], std::move(published), std::memory_order_release);
    published_slot_.store(next, std::memory_order_release);
  }

  std::vector<QueuedCommand> take_commands() {
    std::lock_guard lock(command_mutex_);
    std::vector<QueuedCommand> result;
    result.swap(commands_);
    return result;
  }

  static void submit_queued(BattleState& battle, const std::vector<QueuedCommand>& commands) {
    for (const auto& queued : commands) {
      Command command;
      command.type = queued.type;
      command.kind = queued.kind;
      command.level = queued.level;
      command.position = queued.position;
      command.spell = queued.spell;
      // The simulation thread assigns the explicit next legal rule boundary.
      // GUI latency cannot create a stale or wall-clock-timed Core command.
      // commands is already committed in `QueuedCommand::sequence` order;
      // BattleState preserves that order in its own serialised sequence.
      command.requested_ms = battle.now() + kTickMs;
      battle.submit(command);
    }
  }

  void run() {
    BattleState battle(data_, scenario_);
    auto previous = std::chrono::steady_clock::now();
    auto last_publish = previous;
    std::uint64_t presentation_debt_ms{};
    std::chrono::steady_clock::duration presentation_remainder{};
    while (running_.load(std::memory_order_acquire)) {
      std::uint64_t discarded_through{};
      if (reset_requested_.exchange(false, std::memory_order_acq_rel)) {
        discarded_through = reset_through_sequence_.load(std::memory_order_acquire);
        battle = BattleState(data_, scenario_);
        presentation_debt_ms = 0;
        presentation_remainder = {};
        previous = std::chrono::steady_clock::now();
        publish(battle);
        last_publish = previous;
      }

      auto commands = take_commands();
      if (discarded_through != 0) {
        commands.erase(std::remove_if(commands.begin(), commands.end(), [discarded_through](const QueuedCommand& command) {
          return command.sequence <= discarded_through;
        }), commands.end());
      }
      submit_queued(battle, commands);
      const auto now = std::chrono::steady_clock::now();
      presentation_remainder += now - previous;
      previous = now;
      const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(presentation_remainder).count();
      presentation_remainder -= std::chrono::milliseconds{elapsed};

      std::uint32_t ticks{};
      bool force_publish = false;
      if (paused_.load(std::memory_order_acquire)) {
        ticks = step_requests_.exchange(0, std::memory_order_acq_rel);
        force_publish = ticks != 0;
      } else if (!battle.result().finished && maximum_playback_.load(std::memory_order_acquire)) {
        // This mode removes presentation pacing only. Each operation remains a
        // normal fixed Core tick and commands are still consumed at a future
        // tick-aligned boundary on the following loop iteration.
        constexpr std::uint32_t kMaximumTicksPerSlice = 1'000;
        ticks = kMaximumTicksPerSlice;
      } else if (!battle.result().finished) {
        presentation_debt_ms += static_cast<std::uint64_t>(std::max<std::int64_t>(0, elapsed))
          * playback_speed_.load(std::memory_order_acquire);
        const auto due = presentation_debt_ms / static_cast<std::uint64_t>(kTickMs);
        constexpr std::uint32_t kMaxTicksPerSlice = 120;
        ticks = static_cast<std::uint32_t>(std::min<std::uint64_t>(due, kMaxTicksPerSlice));
        presentation_debt_ms -= static_cast<std::uint64_t>(ticks) * static_cast<std::uint64_t>(kTickMs);
      }
      if (ticks != 0) {
        battle.advance_ticks(ticks);
        // Projecting a complete scene allocates and copies presentation
        // values. It is intentionally capped at the viewer's 30 Hz target;
        // a high playback speed may skip images, never rule ticks. Manual
        // stepping and a terminal result still become visible immediately.
        constexpr auto kPresentationInterval = std::chrono::milliseconds{33};
        if (force_publish || battle.result().finished || now - last_publish >= kPresentationInterval) {
          publish(battle);
          last_publish = now;
        }
        continue;
      }
      if (!commands.empty()) {
        publish(battle);
        last_publish = now;
      }

      std::unique_lock lock(wake_mutex_);
      wake_.wait_for(lock, std::chrono::milliseconds{1});
    }
  }

  const GameData& data_;
  Scenario scenario_;
  std::atomic<bool> running_{true};
  std::atomic<bool> paused_{false};
  std::atomic<bool> reset_requested_{false};
  std::atomic<std::uint64_t> reset_through_sequence_{};
  std::atomic<std::uint32_t> step_requests_{0};
  std::atomic<std::uint32_t> playback_speed_{1};
  std::atomic<bool> maximum_playback_{false};
  std::mutex command_mutex_;
  std::vector<QueuedCommand> commands_;
  std::uint64_t next_command_sequence_{1}; // guarded by command_mutex_
  std::mutex wake_mutex_;
  std::condition_variable wake_;
  std::thread simulation_thread_;
  std::array<std::shared_ptr<const PresentationSnapshot>, 3> buffers_{};
  std::atomic<std::size_t> published_slot_{0};
  std::uint64_t generation_{}; // simulation thread only after construction
};

} // namespace cocsim::viewer
