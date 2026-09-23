#include "cocsim/core.hpp"
#include "detail/json_reader.hpp"
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
namespace cocsim {
bool save_text(const std::string& path, const std::string& value, std::string& error) {
  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  if (!file || !(file << value)) { error = "cannot write " + path; return false; }
  error.clear(); return true;
}
bool load_text(const std::string& path, std::string& value, std::string& error) {
  std::ifstream file(path, std::ios::binary);
  if (!file) { error = "cannot open " + path; return false; }
  std::ostringstream buffer; buffer << file.rdbuf();
  if (!file.good() && !file.eof()) { error = "cannot read " + path; return false; }
  value = buffer.str(); error.clear(); return true;
}
std::string scenario_json(const Scenario& s) {
  if (s.ruleset!=kEmptyRuleset || s.width!=kHomeVillageTotalTiles || s.height!=kHomeVillageTotalTiles
      || s.duration_ms<=0 || s.duration_ms%kTickMs!=0)
    throw std::invalid_argument("only the empty-16ms-v1 scenario is supported");
  return "{\"format_version\":1,\"ruleset\":\"empty-16ms-v1\",\"width\":" + std::to_string(s.width)
    + ",\"height\":" + std::to_string(s.height) + ",\"seed\":" + std::to_string(s.seed)
    + ",\"duration_ms\":" + std::to_string(s.duration_ms) + "}";
}
static Scenario decode_scenario(const json::Value& value) {
  value.keys({"format_version","ruleset","width","height","seed","duration_ms"});
  if (value.at("format_version").integer() != 1) throw std::runtime_error("unsupported scenario format");
  Scenario s;
  s.ruleset = value.at("ruleset").string();
  const auto width = value.at("width").integer(), height = value.at("height").integer();
  if (width != kHomeVillageTotalTiles || height != kHomeVillageTotalTiles) throw std::runtime_error("unsupported board size");
  s.width = static_cast<int>(width); s.height = static_cast<int>(height);
  s.seed = value.at("seed").unsigned_integer();
  s.duration_ms = value.at("duration_ms").integer();
  (void)BattleState(s);
  return s;
}
bool parse_scenario(const std::string& text, Scenario& scenario, std::string& error) {
  try { auto decoded = decode_scenario(json::parse(text)); scenario = std::move(decoded); error.clear(); return true; }
  catch (const std::exception& e) { error = e.what(); return false; }
}
bool load_scenario(const std::string& path, Scenario& s, std::string& error) {
  std::string text; return load_text(path,text,error) && parse_scenario(text,s,error);
}
bool save_scenario(const std::string& path, const Scenario& s, std::string& error) {
  try { (void)BattleState(s); return save_text(path,scenario_json(s) + "\n",error); }
  catch (const std::exception& e) { error=e.what(); return false; }
}
std::string replay_json(const Scenario& s, const std::vector<Command>& commands) {
  std::string out = "{\"format_version\":1,\"tick_ms\":16,\"scenario\":" + scenario_json(s) + ",\"commands\":[";
  for (std::size_t i=0;i<commands.size();++i) {
    if (i) out += ",";
    const auto& c=commands[i];
    out += "{\"type\":\"" + std::string(command_name(c.type)) + "\",\"requested_at_ms\":" + std::to_string(c.requested_at_ms)
      + ",\"effective_at_ms\":" + std::to_string(c.effective_at_ms) + ",\"sequence\":" + std::to_string(c.sequence) + "}";
  }
  return out + "]}";
}
bool parse_replay(const std::string& text, Scenario& s, std::vector<Command>& commands, std::string& error) {
  try {
    const auto root=json::parse(text);
    root.keys({"format_version","tick_ms","scenario","commands"});
    if (root.at("format_version").integer()!=1 || root.at("tick_ms").integer()!=kTickMs) throw std::runtime_error("unsupported replay version or tick");
    auto candidate=decode_scenario(root.at("scenario"));
    const auto& array=root.at("commands");
    if (array.type!=json::Value::Type::Array) throw std::runtime_error("expected replay commands array");
    std::vector<Command> decoded;
    for (const auto& entry:array.array) {
      entry.keys({"type","requested_at_ms","effective_at_ms","sequence"});
      const auto type=entry.at("type").string();
      Command c;
      if (type=="wait") c.type=CommandType::Wait;
      else if (type=="end_battle") c.type=CommandType::EndBattle;
      else throw std::runtime_error("unsupported replay command: "+type);
      c.requested_at_ms=entry.at("requested_at_ms").integer();
      c.effective_at_ms=entry.at("effective_at_ms").integer();
      c.sequence=entry.at("sequence").unsigned_integer();
      decoded.push_back(c);
    }
    BattleState probe(candidate);
    if (!probe.replay_commands(decoded,candidate.duration_ms,error)) return false;
    s=std::move(candidate); commands=std::move(decoded); error.clear(); return true;
  } catch (const std::exception& e) { error=e.what(); return false; }
}
bool load_replay(const std::string& path, Scenario& s, std::vector<Command>& c, std::string& error) {
  std::string text; return load_text(path,text,error) && parse_replay(text,s,c,error);
}
bool save_replay(const std::string& path, const Scenario& s, const std::vector<Command>& c, std::string& error) {
  try {
    const auto encoded=replay_json(s,c);
    Scenario checked; std::vector<Command> checked_commands;
    if (!parse_replay(encoded,checked,checked_commands,error)) return false;
    return save_text(path,encoded + "\n",error);
  } catch (const std::exception& e) { error=e.what(); return false; }
}
} // namespace cocsim
