#include "cocsim/core.hpp"
#include "test_support.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { bool attack_to(const BattleState& b,EntityId target,double value){return std::any_of(b.events().begin(),b.events().end(),[=](const Event&e){return e.type==EventType::Attack&&e.actor==4&&e.target==target&&e.value==value;});} EntityId first_attack_target(const BattleState& b){const auto it=std::find_if(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Attack&&e.actor==4;});return it==b.events().end()?0:it->target;} void submit(BattleState& b,const std::vector<Command>& c){for(const auto& x:c)COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0();const auto* hog=data.find(Kind::HogRider,15);
  COCSIM_REQUIRE(hog&&hog->hp==1750&&hog->damage==250&&hog->dps==250&&hog->cooldown==1008&&hog->range==.6&&hog->movement_tiles_per_second==3&&hog->housing_space==5&&!hog->flying&&hog->target_focus==TargetFocus::Defenses&&hog->target_type==TargetType::Ground&&hog->jumps_walls&&hog->deployable);
  Scenario s;s.width=20;s.height=20;s.duration_ms=10000;s.defenders={{Kind::Wall,1,{10.5,10.5}},{Kind::Cannon,1,{14.5,10.5}},{Kind::GoldMine,1,{5.5,14.5}}};s.army={{Kind::HogRider,15,1}};
  const std::vector<Command> c={{CommandType::Deploy,kTickMs,0,0,Kind::HogRider,15,{5.5,10.5}}};BattleState b(data,s);submit(b,c);b.advance_ticks(700);
  // The nearer Gold Mine is not a defense; the shared Core must instead
  // choose the Cannon and omit the Wall from this troop's path grid.
  COCSIM_REQUIRE(attack_to(b,2,250));COCSIM_REQUIRE(first_attack_target(b)==2);
  const auto snap=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snap)&&restored.state_hash()==b.state_hash());for(int i=0;i<150;++i){b.advance_ticks(1);restored.advance_ticks(1);COCSIM_REQUIRE(b.state_hash()==restored.state_hash());}
  const std::string path="replay-hog-rider-test.json";std::string error;COCSIM_REQUIRE(save_replay(path,s,c,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(path,rs,rc,error));BattleState replay(data,rs);submit(replay,rc);replay.advance_ticks(850);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(path.c_str())==0);
}
