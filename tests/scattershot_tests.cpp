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
bool damaged(const BattleState& battle,EntityId target,double value){return std::any_of(battle.events().begin(),battle.events().end(),[&](const Event& event){return event.type==EventType::Damaged&&event.actor==1&&event.target==target&&std::abs(event.value-value)<1e-9;});}
bool damaged_between(const BattleState& battle,EntityId target,double lower,double upper){return std::any_of(battle.events().begin(),battle.events().end(),[&](const Event& event){return event.type==EventType::Damaged&&event.actor==1&&event.target==target&&event.value>lower&&event.value<upper;});}
}
int main(){
  const auto data=GameData::v0(); const auto* scattershot=data.find(Kind::Scattershot,1);
  COCSIM_REQUIRE(scattershot&&scattershot->hp==3600&&scattershot->damage==400&&scattershot->min_range==3&&scattershot->range==10&&scattershot->cooldown==3200);
  COCSIM_REQUIRE(scattershot->scattershot_direct_min_damage==300&&scattershot->scattershot_splash_max_damage==300&&scattershot->scattershot_splash_min_damage==100&&scattershot->scattershot_cone_angle_degrees==90&&scattershot->scattershot_cone_range==5&&scattershot->scattershot_inner_range==1&&scattershot->scattershot_same_altitude_only);
  Scenario scenario;scenario.width=24;scenario.height=20;scenario.duration_ms=4000;scenario.defenders={{Kind::Scattershot,1,{16.5,10.5}}};scenario.army={{Kind::Barbarian,1,1},{Kind::Golem,1,3},{Kind::Dragon,1,1}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{10.5,10.5}},{CommandType::Deploy,kTickMs,0,1,Kind::Golem,1,{9.5,10.5}},{CommandType::Deploy,kTickMs,0,2,Kind::Golem,1,{6.5,10.5}},{CommandType::Deploy,kTickMs,0,3,Kind::Golem,1,{10.5,13.5}},{CommandType::Deploy,kTickMs,0,4,Kind::Dragon,1,{6.5,10.5}}};
  BattleState battle(data,scenario);submit_all(battle,commands);battle.advance_ticks(2);
  const auto projectiles=battle.observe_projectiles();COCSIM_REQUIRE(projectiles.size()==1&&projectiles.front().owner==1);
  const auto snapshot=battle.snapshot();BattleState restored(data,scenario);COCSIM_REQUIRE(restored.restore(snapshot));
  // Direct impact, one-tile and 1–5-tile rear falloff are source bounded. A
  // lateral unit and opposite altitude are deliberately excluded.
  battle.advance_ticks(1);restored.advance_ticks(1);COCSIM_REQUIRE(damaged(battle,2,400)&&damaged_between(battle,3,299,301)&&damaged_between(battle,4,149,152));COCSIM_REQUIRE(!damaged_between(battle,5,149,152)&&!damaged_between(battle,6,149,152));
  std::string error;const std::string replay_path="replay-scattershot-test.json";COCSIM_REQUIRE(save_replay(replay_path,scenario,commands,error));Scenario replay_scenario;std::vector<Command> replay_commands;COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error));BattleState replay(data,replay_scenario);submit_all(replay,replay_commands);replay.advance_ticks(3);
  COCSIM_REQUIRE(battle.state_hash()==restored.state_hash()&&battle.state_hash()==replay.state_hash());COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
