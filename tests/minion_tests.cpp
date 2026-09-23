#include "cocsim/core.hpp"
#include "test_support.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { bool has_event(const BattleState& b,EventType t,EntityId a,double v=-1){return std::any_of(b.events().begin(),b.events().end(),[=](const Event&e){return e.type==t&&e.actor==a&&(v<0||e.value==v);});} void submit(BattleState& b,const std::vector<Command>& c){for(const auto& x:c)COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0();const auto* minion=data.find(Kind::Minion,14);
  COCSIM_REQUIRE(minion&&minion->hp==140&&minion->damage==92&&minion->dps==92&&minion->cooldown==1008&&minion->housing_space==2&&minion->flying&&minion->target_type==TargetType::Both&&minion->seeking_air_mine_immune&&minion->deployable);
  Scenario s;s.width=20;s.height=20;s.duration_ms=8000;s.defenders={{Kind::SeekingAirMine,1,{10.5,10.5}},{Kind::GoldMine,1,{15.5,10.5}}};s.army={{Kind::Minion,14,1}};
  const std::vector<Command> c={{CommandType::Deploy,kTickMs,0,0,Kind::Minion,14,{8.5,10.5}}};BattleState b(data,s);submit(b,c);b.advance_ticks(500);
  // The direct source description says normal Minions are undetectable by
  // Seeking Air Mines, so the armed mine must remain untriggered.
  COCSIM_REQUIRE(!has_event(b,EventType::TargetChanged,1));COCSIM_REQUIRE(!has_event(b,EventType::Projectile,1));COCSIM_REQUIRE(has_event(b,EventType::Attack,3,92));
  const auto snap=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snap)&&restored.state_hash()==b.state_hash());for(int i=0;i<150;++i){b.advance_ticks(1);restored.advance_ticks(1);COCSIM_REQUIRE(b.state_hash()==restored.state_hash());}
  const std::string path="replay-minion-test.json";std::string error;COCSIM_REQUIRE(save_replay(path,s,c,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(path,rs,rc,error));BattleState replay(data,rs);submit(replay,rc);replay.advance_ticks(650);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(path.c_str())==0);
}
