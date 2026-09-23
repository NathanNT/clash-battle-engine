#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;
namespace {
void submit_all(BattleState& battle,const std::vector<Command>& commands){for(const auto& command:commands) COCSIM_REQUIRE(battle.submit(command));}
const EntityView* find(const std::vector<EntityView>& views,Kind kind,Side side){const auto it=std::find_if(views.begin(),views.end(),[&](const EntityView& view){return view.kind==kind&&view.side==side;});return it==views.end()?nullptr:&*it;}
bool has_projectile(const BattleState& battle){return std::any_of(battle.events().begin(),battle.events().end(),[](const Event& event){return event.type==EventType::Projectile&&event.actor==1&&event.detail=="logical projectile";});}
}
int main(){
  const auto data=GameData::v0();const auto* normal=data.find(Kind::XBow,13);const auto* charge_one=data.find(Kind::XBow,1,"supercharged");const auto* charge_two=data.find(Kind::XBow,2,"supercharged");
  COCSIM_REQUIRE(normal&&normal->hp==5000&&normal->damage==31.36&&normal->dps==245&&normal->range==14&&normal->cooldown==128&&normal->target_type==TargetType::Ground&&normal->alternate_range==11.5&&normal->alternate_target_type==TargetType::Both);
  COCSIM_REQUIRE(charge_one&&charge_one->hp==5000&&charge_one->damage==32.64&&charge_one->dps==255&&charge_one->attributes.town_hall_required==18&&charge_two&&charge_two->hp==5100&&charge_two->damage==32.64&&charge_two->dps==255&&charge_two->attributes.town_hall_required==18);
  Scenario ground;ground.width=32;ground.height=20;ground.duration_ms=4000;ground.defenders={{Kind::XBow,13,{20.5,10.5}}};ground.army={{Kind::Barbarian,1,1},{Kind::Dragon,1,1}};
  const std::vector<Command> ground_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{7.0,10.5}},{CommandType::Deploy,kTickMs,0,1,Kind::Dragon,1,{10.5,10.5}}};BattleState ground_battle(data,ground);submit_all(ground_battle,ground_commands);ground_battle.advance_ticks(2);const auto ground_views=ground_battle.observe();const auto* ground_xbow=find(ground_views,Kind::XBow,Side::Defender);const auto* barbarian=find(ground_views,Kind::Barbarian,Side::Attacker);COCSIM_REQUIRE(ground_xbow&&barbarian&&ground_xbow->target==barbarian->id); // 14-tile ground mode excludes the nearer Dragon.
  Scenario air;air.width=32;air.height=20;air.duration_ms=4000;air.defenders={{Kind::XBow,13,{20.5,10.5},"air_and_ground"}};air.army={{Kind::Barbarian,1,1},{Kind::Dragon,1,1}};
  const std::vector<Command> air_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{7.0,10.5}},{CommandType::Deploy,kTickMs,0,1,Kind::Dragon,1,{10.5,10.5}}};BattleState air_battle(data,air);submit_all(air_battle,air_commands);air_battle.advance_ticks(2);const auto air_views=air_battle.observe();const auto* air_xbow=find(air_views,Kind::XBow,Side::Defender);const auto* dragon=find(air_views,Kind::Dragon,Side::Attacker);COCSIM_REQUIRE(air_xbow&&dragon&&air_xbow->target==dragon->id&&has_projectile(air_battle)); // 13.5-tile ground target is outside the 11.5-tile alternate mode.
  const auto snapshot=air_battle.snapshot();BattleState restored(data,air);COCSIM_REQUIRE(restored.restore(snapshot)&&restored.state_hash()==air_battle.state_hash());air_battle.advance_ticks(8);restored.advance_ticks(8);COCSIM_REQUIRE(restored.state_hash()==air_battle.state_hash());
  std::string error;const std::string replay_path="replay-x-bow-test.json";COCSIM_REQUIRE(save_replay(replay_path,air,air_commands,error));Scenario replay_scenario;std::vector<Command> replay_commands;COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error));COCSIM_REQUIRE(replay_scenario.defenders.front().mode=="air_and_ground");BattleState replay(data,replay_scenario);submit_all(replay,replay_commands);replay.advance_ticks(10);COCSIM_REQUIRE(replay.state_hash()==air_battle.state_hash());COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
