#include "cocsim/core.hpp"
#include "test_support.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
using namespace cocsim;
namespace { void submit_all(BattleState& b,const std::vector<Command>& c){for(const auto& x:c) COCSIM_REQUIRE(b.submit(x));} }
int main(){
  const auto data=GameData::v0(); const auto* titan=data.find(Kind::ElectroTitan,5);
  COCSIM_REQUIRE(titan&&titan->hp==9200&&titan->damage==405&&titan->dps==270&&titan->range==1.25&&titan->cooldown==1504&&titan->housing_space==32&&titan->target_type==TargetType::Both&&titan->aura_damage==60&&titan->aura_range==3.5&&titan->aura_cooldown==400&&titan->aura_excludes_walls&&titan->aura_ignores_freeze&&titan->aura_ignores_rage);
  Scenario s;s.width=20;s.height=20;s.duration_ms=3000;s.defenders={{Kind::GoldMine,1,{7.5,10}},{Kind::Wall,1,{7,10}},{Kind::Dragon,12,{6,12}}};s.army={{Kind::ElectroTitan,5,1}};s.spells={{SpellKind::Rage,6,1}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::ElectroTitan,5,{4,10}},{CommandType::CastSpell,kTickMs,0,1,Kind::Barbarian,1,{4,10},SpellKind::Rage}};
  BattleState b(data,s);submit_all(b,commands);b.advance_ticks(42);
  COCSIM_REQUIRE(std::count_if(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Attack&&e.actor==4&&e.detail=="electro titan aura"&&e.value==60;})==2);
  COCSIM_REQUIRE(std::none_of(b.events().begin(),b.events().end(),[](const Event&e){return e.type==EventType::Damaged&&e.target==2&&e.value==60;}));
  const auto snapshot=b.snapshot();BattleState restored(data,s);COCSIM_REQUIRE(restored.restore(snapshot));b.advance_ticks(40);restored.advance_ticks(40);COCSIM_REQUIRE(restored.state_hash()==b.state_hash());
  const std::string replay_path="replay-electro-titan-test.json";std::string error;COCSIM_REQUIRE(save_replay(replay_path,s,commands,error));Scenario rs;std::vector<Command> rc;COCSIM_REQUIRE(load_replay(replay_path,rs,rc,error));BattleState replay(data,rs);submit_all(replay,rc);replay.advance_ticks(82);COCSIM_REQUIRE(replay.state_hash()==b.state_hash());COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
