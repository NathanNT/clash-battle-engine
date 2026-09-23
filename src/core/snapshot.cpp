#include "cocsim/core.hpp"
#include "detail/json_reader.hpp"
#include <stdexcept>
namespace cocsim {
static void hash_bytes(std::uint64_t& hash, const std::string& text) {
  for (unsigned char byte : text) { hash ^= byte; hash *= 1099511628211ULL; }
}
std::uint64_t BattleState::state_hash() const {
  std::uint64_t hash=14695981039346656037ULL;
  hash_bytes(hash,scenario_json(scenario_));
  hash_bytes(hash,"|"+std::to_string(time_ms_)+"|"+result_name(result_)+"|"+std::to_string(next_sequence_));
  for (const auto& command:commands_)
    hash_bytes(hash,"|"+std::string(command_name(command.type))+":"+std::to_string(command.requested_at_ms)+":"
      +std::to_string(command.effective_at_ms)+":"+std::to_string(command.sequence));
  return hash;
}
std::string BattleState::snapshot() const {
  std::string out = "{\"format_version\":1,\"kind\":\"empty-snapshot\",\"time_ms\":"+std::to_string(time_ms_)
    +",\"replay\":"+replay_json(scenario_,commands_)+",\"events\":[";
  for (std::size_t i=0;i<events_.size();++i) {
    if (i) out+=",";
    const auto& e=events_[i];
    json::Value detail; detail.type=json::Value::Type::String; detail.scalar=e.detail;
    out+="{\"type\":"+std::to_string(static_cast<int>(e.type))+",\"time_ms\":"+std::to_string(e.time_ms)
      +",\"sequence\":"+std::to_string(e.sequence)+",\"detail\":"+json::stringify(detail)+"}";
  }
  return out+"]}";
}
bool BattleState::restore(const std::string& bytes,std::string& error) {
  try {
    const auto root=json::parse(bytes);
    root.keys({"format_version","kind","time_ms","replay","events"});
    if (root.at("format_version").integer()!=1 || root.at("kind").string()!="empty-snapshot")
      throw std::runtime_error("unsupported snapshot format");
    const auto target=root.at("time_ms").integer();
    // Reuse the strict replay parser, then reconstruct every event and queued command.
    const auto& replay=root.at("replay");
    if (replay.type!=json::Value::Type::Object) throw std::runtime_error("invalid snapshot replay");
    Scenario s; std::vector<Command> commands;
    if (!parse_replay(json::stringify(replay),s,commands,error)) return false;
    if (scenario_json(s)!=scenario_json(scenario_)) throw std::runtime_error("snapshot scenario mismatch");
    BattleState candidate(s);
    // A snapshot may precede later replay submissions; only restore commands already accepted.
    if (!candidate.replay_commands(commands,target,error)) return false;
    const auto& events=root.at("events");
    if (events.type!=json::Value::Type::Array) throw std::runtime_error("invalid snapshot events");
    std::vector<Event> restored_events;
    for (const auto& value:events.array) {
      value.keys({"type","time_ms","sequence","detail"});
      const auto type=value.at("type").integer();
      if (type<0 || type>3) throw std::runtime_error("invalid snapshot event type");
      restored_events.push_back({static_cast<EventType>(type),value.at("time_ms").integer(),
        value.at("sequence").unsigned_integer(),value.at("detail").string()});
    }
    candidate.events_=std::move(restored_events);
    *this=std::move(candidate); error.clear(); return true;
  } catch(const std::exception& e) { error=e.what(); return false; }
}
} // namespace cocsim
