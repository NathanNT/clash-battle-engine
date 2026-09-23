#pragma once
#include "cocsim/core.hpp"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
namespace cocsim::viewer {
class SimulationBridge {
 public:
  explicit SimulationBridge(Scenario scenario = {}) : state_(scenario) {}
  const BattleState& state() const { return state_; }
  BattleState& state() { return state_; }
  bool paused() const { return paused_; }
  void set_paused(bool value) { paused_=value; }
  int speed() const { return speed_; }
  void set_speed(int value) { speed_=std::clamp(value,1,8); }
  void reset() { auto scenario=state_.scenario(); state_=BattleState(scenario); replay_index_=0; accumulated_=0; paused_=true; }
  void new_empty() { replay_.clear(); replay_mode_=false; state_=BattleState(); replay_index_=0; accumulated_=0; paused_=true; }
  bool replay_loaded() const { return replay_mode_; }
  const std::vector<Command>& commands_for_save() const { return replay_loaded()?replay_:state_.commands(); }
  void step() { tick_once(); }
  void present_elapsed(std::uint64_t wall_ms) {
    if (paused_ || state_.result()!=Result::Active) return;
    // Wall time only budgets how many fixed logical ticks to execute.
    accumulated_ += std::min<std::uint64_t>(wall_ms,250) * static_cast<std::uint64_t>(speed_);
    const auto ticks=accumulated_/static_cast<std::uint64_t>(kTickMs);
    accumulated_ %= static_cast<std::uint64_t>(kTickMs);
    for (std::uint64_t i=0;i<ticks && state_.result()==Result::Active;++i) tick_once();
  }
  bool end_next_tick(std::string& error) {
    if (replay_loaded()) { error="replay commands are fixed"; return false; }
    return state_.submit(CommandType::EndBattle,state_.time_ms()+kTickMs,error);
  }
  bool wait_next_tick(std::string& error) {
    if (replay_loaded()) { error="replay commands are fixed"; return false; }
    return state_.submit(CommandType::Wait,state_.time_ms()+kTickMs,error);
  }
  bool load_replay_file(const std::string& path,std::string& error) {
    Scenario scenario; std::vector<Command> commands;
    if (!load_replay(path,scenario,commands,error)) return false;
    state_=BattleState(scenario); replay_=std::move(commands); replay_mode_=true; replay_index_=0;
    paused_=true; accumulated_=0; error.clear(); return true;
  }
 private:
  BattleState state_;
  bool paused_{true};
  int speed_{1};
  std::uint64_t accumulated_{};
  std::vector<Command> replay_;
  bool replay_mode_{};
  std::size_t replay_index_{};
  void tick_once() {
    while (replay_index_<replay_.size() && replay_[replay_index_].requested_at_ms==state_.time_ms()) {
      std::string error;
      if (!state_.submit(replay_[replay_index_].type,replay_[replay_index_].effective_at_ms,error))
        throw std::runtime_error("validated replay failed: "+error);
      ++replay_index_;
    }
    state_.advance_ticks(1);
  }
};
} // namespace cocsim::viewer
