#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { void submit(BattleState& b,const std::vector<Command>& cs){for(const auto& c:cs) COCSIM_REQUIRE(b.submit(c));}
int attacks(const BattleState& b,EntityId target){return static_cast<int>(std::count_if(b.events().begin(),b.events().end(),[&](const Event& e){return e.type==EventType::Attack&&e.actor==1&&e.target==target&&e.detail=="multi-target attack";}));}}
int main(){const auto data=GameData::v0(); const auto* tower=data.find(Kind::MultiArcherTower,4); COCSIM_REQUIRE(tower&&tower->damage==73&&tower->cooldown==608&&tower->multi_target_count==3&&tower->range==10);
 Scenario one;one.width=20;one.height=20;one.duration_ms=3000;one.defenders={{Kind::MultiArcherTower,4,{13.5,10.5}}};one.army={{Kind::Barbarian,1,1}};std::vector<Command> one_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{5.5,10.5}}};BattleState b(data,one);submit(b,one_commands);b.advance_ticks(2);COCSIM_REQUIRE(attacks(b,2)==3&&b.observe_projectiles().size()==3);const auto snapshot=b.snapshot();BattleState restored(data,one);COCSIM_REQUIRE(restored.restore(snapshot));
 Scenario two=one;two.army={{Kind::Barbarian,1,2}};std::vector<Command> two_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{5.5,10.5}},{CommandType::Deploy,kTickMs,0,1,Kind::Barbarian,1,{6.5,10.5}}};BattleState pair(data,two);submit(pair,two_commands);pair.advance_ticks(2);COCSIM_REQUIRE(attacks(pair,2)==1&&attacks(pair,3)==2&&pair.observe_projectiles().size()==3);std::string error,path="replay-multi-archer-tower-test.json";COCSIM_REQUIRE(save_replay(path,two,two_commands,error));Scenario replay_scenario;std::vector<Command> replay_commands;COCSIM_REQUIRE(load_replay(path,replay_scenario,replay_commands,error));BattleState replay(data,replay_scenario);submit(replay,replay_commands);replay.advance_ticks(2);b.advance_ticks(1);restored.advance_ticks(1);pair.advance_ticks(1);replay.advance_ticks(1);COCSIM_REQUIRE(b.state_hash()==restored.state_hash()&&pair.state_hash()==replay.state_hash());COCSIM_REQUIRE(std::remove(path.c_str())==0);}
