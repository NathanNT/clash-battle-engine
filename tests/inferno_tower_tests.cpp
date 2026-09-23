#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;
namespace {
void submit_all(BattleState& battle,const std::vector<Command>& commands){for(const auto& command:commands) COCSIM_REQUIRE(battle.submit(command));}
int beam_count(const BattleState& battle,double value){return static_cast<int>(std::count_if(battle.events().begin(),battle.events().end(),[&](const Event& event){return event.type==EventType::Attack&&event.actor==1&&event.value==value&&event.detail=="inferno beam";}));}
bool equal(double actual,double expected){return std::abs(actual-expected)<1e-9;}
}
int main(){
  const auto data=GameData::v0();const auto* normal=data.find(Kind::InfernoTower,12);const auto* charge_one=data.find(Kind::InfernoTower,1,"supercharged");const auto* charge_two=data.find(Kind::InfernoTower,2,"supercharged");
  COCSIM_REQUIRE(normal&&normal->hp==5100&&normal->range==9&&normal->cooldown==128&&normal->target_type==TargetType::Both&&normal->multi_target_range==10&&normal->multi_target_damage==19.84&&normal->multi_target_count==6&&normal->inferno_initial_damage==19.84&&normal->inferno_second_damage==42.24&&normal->inferno_max_damage==422.4&&normal->inferno_second_stage_at==1504&&normal->inferno_max_stage_at==5248);
  COCSIM_REQUIRE(charge_one);COCSIM_REQUIRE(charge_one->hp==5100);COCSIM_REQUIRE(equal(charge_one->damage,448));COCSIM_REQUIRE(equal(charge_one->dps,3500));COCSIM_REQUIRE(equal(charge_one->multi_target_damage,21.12));COCSIM_REQUIRE(charge_one->multi_target_count==6);COCSIM_REQUIRE(equal(charge_one->inferno_initial_damage,21.12));COCSIM_REQUIRE(equal(charge_one->inferno_second_damage,44.8));COCSIM_REQUIRE(equal(charge_one->inferno_max_damage,448));COCSIM_REQUIRE(charge_one->attributes.town_hall_required==18);
  COCSIM_REQUIRE(charge_two&&charge_two->hp==5300&&equal(charge_two->damage,448)&&equal(charge_two->dps,3500)&&equal(charge_two->multi_target_damage,21.12)&&charge_two->multi_target_count==6&&equal(charge_two->inferno_initial_damage,21.12)&&equal(charge_two->inferno_second_damage,44.8)&&equal(charge_two->inferno_max_damage,448)&&charge_two->attributes.town_hall_required==18);

  Scenario multi;multi.width=32;multi.height=20;multi.duration_ms=4000;multi.defenders={{Kind::InfernoTower,12,{20.5,10.5},"multi_target"}};multi.army={{Kind::Barbarian,1,7}};
  std::vector<Command> multi_commands;for(int index=0;index<7;++index) multi_commands.push_back({CommandType::Deploy,kTickMs,0,static_cast<std::uint64_t>(index),Kind::Barbarian,1,{12.0,7.5+index}});
  BattleState multi_battle(data,multi);submit_all(multi_battle,multi_commands);multi_battle.advance_ticks(2);COCSIM_REQUIRE(beam_count(multi_battle,19.84)==6);COCSIM_REQUIRE(std::none_of(multi_battle.events().begin(),multi_battle.events().end(),[](const Event& event){return event.type==EventType::Projectile&&event.actor==1;}));

  Scenario single;single.width=32;single.height=20;single.duration_ms=9000;single.defenders={{Kind::InfernoTower,12,{20.5,10.5}}};single.army={{Kind::Golem,15,1}};
  const std::vector<Command> single_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Golem,15,{12.0,10.5}}};BattleState single_battle(data,single);submit_all(single_battle,single_commands);single_battle.advance_ticks(600);COCSIM_REQUIRE(beam_count(single_battle,19.84)>0&&beam_count(single_battle,42.24)>0&&beam_count(single_battle,422.4)>0);
  const auto snapshot=single_battle.snapshot();BattleState restored(data,single);COCSIM_REQUIRE(restored.restore(snapshot)&&restored.state_hash()==single_battle.state_hash());single_battle.advance_ticks(10);restored.advance_ticks(10);COCSIM_REQUIRE(restored.state_hash()==single_battle.state_hash());

  Scenario charged;charged.width=32;charged.height=20;charged.duration_ms=4000;charged.defenders={{Kind::InfernoTower,2,{20.5,10.5},"multi_target","supercharged"}};charged.army={{Kind::Barbarian,1,1}};
  const std::vector<Command> charged_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{12.0,10.5}}};BattleState charged_battle(data,charged);submit_all(charged_battle,charged_commands);charged_battle.advance_ticks(2);COCSIM_REQUIRE(beam_count(charged_battle,21.12)==1&&charged_battle.observe().front().supercharged);
  std::string error;const std::string replay_path="replay-inferno-tower-test.json";COCSIM_REQUIRE(save_replay(replay_path,charged,charged_commands,error));Scenario replay_scenario;std::vector<Command> replay_commands;COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error));COCSIM_REQUIRE(replay_scenario.defenders.front().mode=="multi_target"&&replay_scenario.defenders.front().variant=="supercharged");BattleState replay(data,replay_scenario);submit_all(replay,replay_commands);replay.advance_ticks(2);COCSIM_REQUIRE(replay.state_hash()==charged_battle.state_hash());COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
