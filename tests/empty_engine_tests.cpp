#include "cocsim/core.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#undef assert
#define assert(condition) do { if (!(condition)) { std::fprintf(stderr, "assertion failed: %s at line %d\n", #condition, __LINE__); std::abort(); } } while (false)
using namespace cocsim;
int main() {
  static_assert(kTickMs==16);
  Board board;
  assert(board.width()==50 && board.height()==50);
  assert(board.contains(GridCell{0,0}) && !board.contains(GridCell{50,0}));
  assert(!board.buildable(GridCell{2,3}) && board.buildable(GridCell{3,3}));
  assert(board.buildable(Rect{3,3,44,44}) && !board.buildable(Rect{2,3,44,44}));
  assert(board.shortest_path({0,0},{2,0}).size()==3);
  assert(board.shortest_path({0,0},{2,0},{{1,0,1,1}}).size()==5);
  Scenario scenario;
  std::string error;
  Scenario parsed;
  assert(parse_scenario(scenario_json(scenario),parsed,error));
  assert(!parse_scenario("{\"format_version\":1,\"ruleset\":\"empty-16ms-v1\",\"width\":50,\"height\":50,\"seed\":1,\"duration_ms\":1600,\"defenders\":[]}",parsed,error));
  assert(!parse_scenario("{\"format_version\":1,\"ruleset\":\"empty-16ms-v1\",\"width\":50,\"height\":50,\"seed\":1,\"duration_ms\":1600,\"seed\":2}",parsed,error));
  assert(!parse_scenario("{\"format_version\":1,\"ruleset\":\"legacy\",\"width\":50,\"height\":50,\"seed\":1,\"duration_ms\":1600}",parsed,error));
  BattleState a(scenario),b(scenario);
  assert(!a.submit(CommandType::Wait,0,error));
  assert(!a.submit(CommandType::Wait,15,error));
  assert(a.submit(CommandType::Wait,32,error));
  assert(a.submit(CommandType::EndBattle,64,error));
  assert(a.submit(CommandType::Wait,64,error));
  assert(b.submit(CommandType::Wait,32,error));
  assert(b.submit(CommandType::EndBattle,64,error));
  assert(b.submit(CommandType::Wait,64,error));
  a.advance_ticks(2); b.advance_to(32,error);
  assert(a.time_ms()==32 && a.state_hash()==b.state_hash());
  auto snapshot=a.snapshot();
  BattleState restored(scenario);
  assert(restored.restore(snapshot,error));
  assert(restored.state_hash()==a.state_hash());
  assert(restored.events()==a.events());
  a.advance_ticks(2); restored.advance_ticks(2); b.advance_ticks(2);
  assert(a.time_ms()==64 && a.result()==Result::Ended);
  assert(a.state_hash()==restored.state_hash() && a.state_hash()==b.state_hash());
  assert(a.events()==restored.events());
  Scenario replay_scenario; std::vector<Command> replay_commands;
  const auto replay=replay_json(scenario,a.commands());
  assert(parse_replay(replay,replay_scenario,replay_commands,error));
  BattleState played(replay_scenario);
  assert(played.replay_commands(replay_commands,scenario.duration_ms,error));
  assert(played.state_hash()==a.state_hash());
  assert(!parse_replay(replay.substr(0,replay.size()-1)+",\"unknown\":1}",replay_scenario,replay_commands,error));
  auto old_tick_replay=replay;
  old_tick_replay.replace(old_tick_replay.find("\"tick_ms\":16"),12,"\"tick_ms\":10");
  assert(!parse_replay(old_tick_replay,replay_scenario,replay_commands,error));
  assert(!restored.restore("{\"format_version\":0}",error));
  BattleState timeout(scenario); timeout.advance_ticks(100);
  assert(timeout.time_ms()==1600 && timeout.result()==Result::TimedOut);
  assert(timeout.events().back().type==EventType::Finished);
  assert(!timeout.submit(CommandType::Wait,1616,error));
  auto bad=scenario; bad.duration_ms=17;
  bool threw=false; try { (void)BattleState(bad); } catch (...) { threw=true; }
  assert(threw);
  return 0;
}
