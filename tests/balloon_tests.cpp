#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;
namespace { void submit_all(BattleState& b,const std::vector<Command>& c){for(const auto& x:c)COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0(); const auto* balloon=data.find(Kind::Balloon,13);
  COCSIM_REQUIRE(balloon&&balloon->hp==1360&&balloon->dps==326&&balloon->damage==978&&balloon->death_damage==425&&balloon->cooldown==3008&&balloon->range==.5&&balloon->movement_tiles_per_second==1.25&&balloon->housing_space==5&&balloon->flying&&balloon->target_focus==TargetFocus::DefensesOnly&&balloon->splash&&balloon->splash_radius==1.2&&balloon->death_splash_radius==1.2&&balloon->death_damage_ground_only);
  Scenario s;s.width=20;s.height=20;s.duration_ms=8000;s.defenders={{Kind::Cannon,1,{10.5,10.5}},{Kind::GoldMine,1,{11.5,10.5}}};s.army={{Kind::Balloon,13,1}};
  const std::vector<Command> c={{CommandType::Deploy,kTickMs,0,0,Kind::Balloon,13,{7.5,10.5}}}; BattleState b(data,s);submit_all(b,c);b.advance_ticks(500);
  COCSIM_REQUIRE(std::any_of(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Attack&&e.actor==3&&e.target==1&&e.value==978;}));
  COCSIM_REQUIRE(std::any_of(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Damaged&&e.actor==3&&e.target==2&&e.value==978;}));
  const auto snap=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snap)&&restored.state_hash()==b.state_hash());
  const std::string path="replay-balloon-test.json";std::string error;COCSIM_REQUIRE(save_replay(path,s,c,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(path,rs,rc,error));BattleState replay(data,rs);submit_all(replay,rc);replay.advance_ticks(500);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(path.c_str())==0);
}
