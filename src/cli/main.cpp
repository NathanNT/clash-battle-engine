#include "clash_battle_engine/core.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace clash_battle_engine;
static Milliseconds number(const char* text) {
  std::size_t used{};
  auto value=std::stoll(text,&used);
  if (text[used]!='\0') throw std::runtime_error("invalid millisecond value");
  return value;
}
static void must(bool success,const std::string& error) { if (!success) throw std::runtime_error(error); }
static void report(const BattleState& battle) {
  std::cout << "{\"ruleset\":\"" << kRulesetId << "\",\"time_ms\":" << battle.time_ms()
    << ",\"result\":\"" << result_name(battle.result()) << "\",\"entities\":[],\"state_hash\":"
    << battle.state_hash() << "}\n";
}
int main(int argc,char** argv) {
  try {
    if (argc<2) throw std::runtime_error("usage: clash_battle_engine demo | init-scenario PATH | validate PATH | simulate PATH [--until MS] [--wait MS] [--end MS] [--save-replay PATH] [--save-snapshot PATH] | replay PATH | resume SCENARIO SNAPSHOT");
    const std::string mode=argv[1]; std::string error;
    if (mode=="init-scenario") {
      if (argc!=3) throw std::runtime_error("init-scenario needs output path");
      must(save_scenario(argv[2],Scenario{},error),error); return 0;
    }
    if (mode=="validate") {
      if (argc!=3) throw std::runtime_error("validate needs scenario path");
      Scenario scenario; must(load_scenario(argv[2],scenario,error),error);
      std::cout << "valid scenario: " << scenario.ruleset << "\n"; return 0;
    }
    if (mode=="replay") {
      if (argc!=3) throw std::runtime_error("replay needs replay path");
      Scenario scenario; std::vector<Command> commands;
      must(load_replay(argv[2],scenario,commands,error),error);
      BattleState battle(scenario);
      must(battle.replay_commands(commands,scenario.duration_ms,error),error);
      report(battle); return 0;
    }
    if (mode=="resume") {
      if (argc!=4) throw std::runtime_error("resume needs scenario and snapshot paths");
      Scenario scenario; std::string snapshot;
      must(load_scenario(argv[2],scenario,error),error);
      must(load_text(argv[3],snapshot,error),error);
      BattleState battle(scenario); must(battle.restore(snapshot,error),error);
      battle.advance_ticks(static_cast<std::uint64_t>((scenario.duration_ms-battle.time_ms())/kTickMs));
      report(battle); return 0;
    }
    if (mode=="demo" || mode=="simulate") {
      if (mode=="simulate" && argc<3) throw std::runtime_error("simulate needs scenario path");
      Scenario scenario;
      int index=mode=="demo"?2:3;
      if (mode=="simulate") must(load_scenario(argv[2],scenario,error),error);
      BattleState battle(scenario);
      Milliseconds until=scenario.duration_ms;
      std::string replay_path,snapshot_path;
      for (;index<argc;++index) {
        const std::string flag=argv[index];
        if (flag=="--until" && index+1<argc) until=number(argv[++index]);
        else if (flag=="--wait" && index+1<argc) must(battle.submit(CommandType::Wait,number(argv[++index]),error),error);
        else if (flag=="--end" && index+1<argc) must(battle.submit(CommandType::EndBattle,number(argv[++index]),error),error);
        else if (flag=="--save-replay" && index+1<argc) replay_path=argv[++index];
        else if (flag=="--save-snapshot" && index+1<argc) snapshot_path=argv[++index];
        else throw std::runtime_error("unknown or incomplete option: "+flag);
      }
      must(battle.advance_to(until,error),error);
      if (!replay_path.empty()) must(save_replay(replay_path,scenario,battle.commands(),error),error);
      if (!snapshot_path.empty()) must(save_text(snapshot_path,battle.snapshot()+"\n",error),error);
      report(battle); return 0;
    }
    throw std::runtime_error("unsupported command: "+mode);
  } catch (const std::exception& e) { std::cerr << "error: " << e.what() << "\n"; return 2; }
}
