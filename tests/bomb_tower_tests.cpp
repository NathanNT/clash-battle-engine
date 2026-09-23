#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;
namespace {
void submit_all(BattleState& battle,const std::vector<Command>& commands){for(const auto& command:commands) COCSIM_REQUIRE(battle.submit(command));}
bool has_event(const BattleState& battle,EventType type,EntityId actor,EntityId target,double value,const char* detail){return std::any_of(battle.events().begin(),battle.events().end(),[&](const Event& event){return event.type==type&&event.actor==actor&&event.target==target&&event.value==value&&event.detail==detail;});}
}
int main(){
  const auto data=GameData::v0();const auto* tower=data.find(Kind::BombTower,1);const auto* max_tower=data.find(Kind::BombTower,13);
  COCSIM_REQUIRE(tower&&max_tower&&tower->target_type==TargetType::Ground&&tower->splash_radius==1.5&&tower->death_damage==150&&tower->death_splash_radius==2.75&&tower->death_damage_delay==1008&&tower->death_damage_ground_only&&max_tower->hp==3050&&max_tower->damage==134.2&&max_tower->death_damage==700);
  Scenario direct;direct.width=20;direct.height=20;direct.duration_ms=3000;direct.defenders={{Kind::BombTower,1,{10.5,10.5}}};direct.army={{Kind::Barbarian,1,1},{Kind::Archer,1,1}};
  BattleState splash(data,direct);const std::vector<Command> direct_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{7.5,10.5}},{CommandType::Deploy,kTickMs,0,1,Kind::Archer,1,{7.5,11.0}}};submit_all(splash,direct_commands);splash.advance_ticks(3);COCSIM_REQUIRE(has_event(splash,EventType::Damaged,1,2,26.4,"damage")&&has_event(splash,EventType::Damaged,1,3,26.4,"damage"));
  Scenario death;death.width=24;death.height=20;death.duration_ms=3000;death.defenders={{Kind::BombTower,1,{10.5,10.5}}};death.army={{Kind::Golem,1,1},{Kind::Dragon,1,1}};death.spells={{SpellKind::Lightning,1,5}};
  const std::vector<Command> death_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Golem,1,{7.99,10.5}},{CommandType::Deploy,kTickMs,0,1,Kind::Dragon,1,{7.99,10.5}},{CommandType::CastSpell,kTickMs,0,2,Kind::Barbarian,1,{10.5,10.5},SpellKind::Lightning},{CommandType::CastSpell,kTickMs,0,3,Kind::Barbarian,1,{10.5,10.5},SpellKind::Lightning},{CommandType::CastSpell,kTickMs,0,4,Kind::Barbarian,1,{10.5,10.5},SpellKind::Lightning},{CommandType::CastSpell,kTickMs,0,5,Kind::Barbarian,1,{10.5,10.5},SpellKind::Lightning},{CommandType::CastSpell,kTickMs,0,6,Kind::Barbarian,1,{10.5,10.5},SpellKind::Lightning}};
  BattleState battle(data,death);submit_all(battle,death_commands);battle.advance_ticks(2);const auto pending=battle.observe_death_explosions();COCSIM_REQUIRE(!battle.result().finished&&pending.size()==1&&pending.front().impact_ms==1024&&pending.front().radius==2.75&&pending.front().ground_only);
  const auto snapshot=battle.snapshot();BattleState restored(data,death);COCSIM_REQUIRE(restored.restore(snapshot)&&restored.state_hash()==battle.state_hash());battle.advance_ticks(62);restored.advance_ticks(62);COCSIM_REQUIRE(battle.state_hash()==restored.state_hash());COCSIM_REQUIRE(!has_event(battle,EventType::Impact,1,0,150,"death damage"));battle.advance_ticks(1);restored.advance_ticks(1);COCSIM_REQUIRE(battle.state_hash()==restored.state_hash());COCSIM_REQUIRE(has_event(battle,EventType::Damaged,1,2,150,"damage")&&!has_event(battle,EventType::Damaged,1,3,150,"damage")&&has_event(battle,EventType::Impact,1,0,150,"death damage"));
  std::string error;const std::string replay_path="replay-bomb-tower-test.json";COCSIM_REQUIRE(save_replay(replay_path,death,death_commands,error));Scenario replay_scenario;std::vector<Command> replay_commands;COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error));BattleState replay(data,replay_scenario);submit_all(replay,replay_commands);replay.advance_ticks(65);COCSIM_REQUIRE(replay.state_hash()==battle.state_hash());COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
