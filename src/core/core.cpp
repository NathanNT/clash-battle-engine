#include "cocsim/core.hpp"
#include <limits>
#include <stdexcept>
namespace cocsim {
const char* result_name(Result r) {
  switch (r) { case Result::Active: return "active"; case Result::Ended: return "ended"; case Result::TimedOut: return "timed_out"; }
  return "invalid";
}
const char* command_name(CommandType t) {
  switch (t) { case CommandType::Wait: return "wait"; case CommandType::EndBattle: return "end_battle"; }
  return "invalid";
}
BattleState::BattleState(Scenario scenario, const GameData&) : scenario_(std::move(scenario)), board_(scenario_.width, scenario_.height) {
  if (scenario_.ruleset != kRulesetId) throw std::invalid_argument("unsupported ruleset: " + scenario_.ruleset);
  if (scenario_.duration_ms <= 0 || scenario_.duration_ms % kTickMs != 0)
    throw std::invalid_argument("duration must be positive and 16 ms tick aligned");
}
bool BattleState::submit(CommandType type, Milliseconds effective, std::string& error) {
  if (result_ != Result::Active) { error = "simulation already finished"; events_.push_back({EventType::Rejected,time_ms_,0,error}); return false; }
  if (type != CommandType::Wait && type != CommandType::EndBattle) { error = "unsupported command"; events_.push_back({EventType::Rejected,time_ms_,0,error}); return false; }
  if (effective <= time_ms_ || effective % kTickMs != 0 || effective > scenario_.duration_ms) {
    error = "command effective time must be future, 16 ms aligned, and within scenario duration";
    events_.push_back({EventType::Rejected,time_ms_,0,error}); return false;
  }
  const auto seq = next_sequence_++;
  commands_.push_back({type,time_ms_,effective,seq});
  events_.push_back({EventType::Accepted,time_ms_,seq,command_name(type)});
  error.clear(); return true;
}
void BattleState::advance_ticks(std::uint64_t count) {
  if (count > static_cast<std::uint64_t>(std::numeric_limits<Milliseconds>::max() / kTickMs))
    throw std::overflow_error("tick count overflow");
  for (std::uint64_t tick = 0; tick < count && result_ == Result::Active; ++tick) {
    time_ms_ += kTickMs;
    for (const auto& command : commands_) {
      if (command.effective_at_ms == time_ms_) {
        events_.push_back({EventType::Executed,time_ms_,command.sequence,command_name(command.type)});
        if (command.type == CommandType::EndBattle) result_ = Result::Ended;
      }
    }
    if (result_ == Result::Active && time_ms_ >= scenario_.duration_ms) result_ = Result::TimedOut;
    if (result_ != Result::Active) events_.push_back({EventType::Finished,time_ms_,0,result_name(result_)});
  }
}
bool BattleState::advance_to(Milliseconds target, std::string& error) {
  if (target < time_ms_ || target % kTickMs != 0) { error = "target time must be current or future and 16 ms aligned"; return false; }
  advance_ticks(static_cast<std::uint64_t>((target - time_ms_) / kTickMs));
  error.clear(); return true;
}
bool BattleState::replay_commands(const std::vector<Command>& commands, Milliseconds target, std::string& error) {
  if (time_ms_ != 0 || !commands_.empty()) { error = "replay requires a fresh state"; return false; }
  std::uint64_t sequence = 1;
  for (const auto& command : commands) {
    if (command.sequence != sequence++ || command.requested_at_ms < time_ms_ ||
        command.requested_at_ms % kTickMs != 0 || command.requested_at_ms >= command.effective_at_ms ||
        command.effective_at_ms > scenario_.duration_ms) { error = "invalid replay command ordering or timing"; return false; }
    if (!advance_to(command.requested_at_ms,error)) return false;
    if (!submit(command.type,command.effective_at_ms,error)) return false;
  }
  if (target < time_ms_) { error = "replay target precedes last command submission"; return false; }
  return advance_to(target,error);
}
Observation BattleState::observe() const { return {time_ms_,board_.width(),board_.height(),result_,state_hash()}; }
} // namespace cocsim
