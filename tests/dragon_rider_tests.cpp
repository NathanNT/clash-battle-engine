#include "cocsim/core.hpp"
#include "test_support.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { void submit(BattleState& b,const std::vector<Command>& c){for(const auto& x:c)COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0();const auto* rider=data.find(Kind::DragonRider,6);
  COCSIM_REQUIRE(rider&&rider->hp==6000&&rider->damage==612&&rider->dps==510&&rider->cooldown==1200&&rider->range==4&&rider->movement_tiles_per_second==2.5&&rider->housing_space==25&&rider->flying&&rider->target_focus==TargetFocus::Defenses&&rider->target_type==TargetType::Both&&rider->deployable);
  Scenario s;s.width=24;s.height=20;s.duration_ms=9000;s.defenders={{Kind::GoldMine,1,{5.5,10.5}},{Kind::Cannon,1,{14.5,10.5}}};s.army={{Kind::DragonRider,6,1}};
  const std::vector<Command> c={{CommandType::Deploy,kTickMs,0,0,Kind::DragonRider,6,{1.5,10.5}}};BattleState b(data,s);submit(b,c);b.advance_ticks(300);
  COCSIM_REQUIRE(std::any_of(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Attack&&e.actor==3&&e.target==2&&e.value==612;}));
  const auto snap=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snap)&&restored.state_hash()==b.state_hash());for(int i=0;i<150;++i){b.advance_ticks(1);restored.advance_ticks(1);COCSIM_REQUIRE(b.state_hash()==restored.state_hash());}
  const std::string path="replay-dragon-rider-test.json";std::string error;COCSIM_REQUIRE(save_replay(path,s,c,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(path,rs,rc,error));BattleState replay(data,rs);submit(replay,rc);replay.advance_ticks(450);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(path.c_str())==0);
}
