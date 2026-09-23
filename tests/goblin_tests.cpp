#include "cocsim/core.hpp"
#include "test_support.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { bool attack(const BattleState& b,double v){return std::any_of(b.events().begin(),b.events().end(),[=](const Event&e){return e.type==EventType::Attack&&e.value==v;});} void submit(BattleState& b,const std::vector<Command>& c){for(const auto& x:c) COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0(); const auto* goblin=data.find(Kind::Goblin,10);
  COCSIM_REQUIRE(goblin&&goblin->hp==166&&goblin->damage==82&&goblin->dps==82&&goblin->cooldown==1008&&goblin->range==.4&&goblin->movement_tiles_per_second==4&&goblin->housing_space==1&&goblin->target_focus==TargetFocus::Resources&&goblin->resource_damage_multiplier==2&&goblin->deployable);
  Scenario s; s.width=20;s.height=20;s.duration_ms=6000;s.defenders={{Kind::Cannon,1,{8.5,10.5}},{Kind::GoldMine,1,{12.5,10.5}}};s.army={{Kind::Goblin,10,1}};
  const std::vector<Command> c={{CommandType::Deploy,kTickMs,0,0,Kind::Goblin,10,{5.5,10.5}}}; BattleState b(data,s);submit(b,c);b.advance_ticks(156); COCSIM_REQUIRE(attack(b,164)); COCSIM_REQUIRE(!attack(b,82));
  const auto snap=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snap)&&restored.state_hash()==b.state_hash());for(int i=0;i<63;++i){b.advance_ticks(1);restored.advance_ticks(1);COCSIM_REQUIRE(b.state_hash()==restored.state_hash());}
  const std::string path="replay-goblin-test.json";std::string error;COCSIM_REQUIRE(save_replay(path,s,c,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(path,rs,rc,error));BattleState replay(data,rs);submit(replay,rc);replay.advance_ticks(219);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(path.c_str())==0);
}
