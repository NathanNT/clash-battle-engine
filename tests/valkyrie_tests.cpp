#include "cocsim/core.hpp"
#include "test_support.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { void submit(BattleState& b,const std::vector<Command>& c){for(const auto& x:c)COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0();const auto* valk=data.find(Kind::Valkyrie,12);
  COCSIM_REQUIRE(valk&&valk->hp==2900&&valk->damage==459&&valk->dps==255&&valk->cooldown==1808&&valk->range==.5&&valk->movement_tiles_per_second==3&&valk->housing_space==8&&!valk->flying&&valk->target_focus==TargetFocus::Any&&valk->target_type==TargetType::Ground&&valk->splash&&valk->splash_radius==1&&valk->deployable);
  Scenario s;s.width=20;s.height=20;s.duration_ms=8000;s.defenders={{Kind::Cannon,1,{5.5,10.5}},{Kind::GoldMine,1,{6.5,10.5}}};s.army={{Kind::Valkyrie,12,1}};
  const std::vector<Command> c={{CommandType::Deploy,kTickMs,0,0,Kind::Valkyrie,12,{1.5,10.5}}};BattleState b(data,s);submit(b,c);b.advance_ticks(200);
  COCSIM_REQUIRE(std::any_of(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Attack&&e.actor==3&&e.target==1&&e.value==459;}));
  COCSIM_REQUIRE(std::any_of(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Damaged&&e.actor==3&&e.target==2&&e.value==459;}));
  const auto snap=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snap)&&restored.state_hash()==b.state_hash());for(int i=0;i<150;++i){b.advance_ticks(1);restored.advance_ticks(1);COCSIM_REQUIRE(b.state_hash()==restored.state_hash());}
  const std::string path="replay-valkyrie-test.json";std::string error;COCSIM_REQUIRE(save_replay(path,s,c,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(path,rs,rc,error));BattleState replay(data,rs);submit(replay,rc);replay.advance_ticks(350);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(path.c_str())==0);
}
