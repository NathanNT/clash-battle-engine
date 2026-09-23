#include "cocsim/core.hpp"
#include "test_support.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { void submit_all(BattleState& b,const std::vector<Command>& c){for(const auto& x:c) COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0(); const auto* barbarian=data.find(Kind::Barbarian,13);
  COCSIM_REQUIRE(barbarian&&barbarian->hp==310&&barbarian->dps==51&&barbarian->damage==51&&barbarian->cooldown==1008&&barbarian->range==.4&&barbarian->movement_tiles_per_second==2.25&&barbarian->housing_space==1&&barbarian->target_focus==TargetFocus::Any&&barbarian->target_type==TargetType::Ground);
  Scenario s; s.width=20;s.height=20;s.duration_ms=10000;s.defenders={{Kind::GoldMine,1,{10.5,10.5}}};s.army={{Kind::Barbarian,13,1}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,13,{7.5,10.5}}}; BattleState b(data,s);submit_all(b,commands);b.advance_ticks(500);
  COCSIM_REQUIRE(std::any_of(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Attack&&e.actor==2&&e.target==1&&e.value==51;}));
  const auto snapshot=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snapshot));COCSIM_REQUIRE(restored.state_hash()==b.state_hash());
  const std::string replay_path="replay-barbarian-test.json";std::string error;COCSIM_REQUIRE(save_replay(replay_path,s,commands,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(replay_path,rs,rc,error));BattleState replay(data,rs);submit_all(replay,rc);replay.advance_ticks(500);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
