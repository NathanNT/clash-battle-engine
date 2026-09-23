#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for(const auto& command:commands) COCSIM_REQUIRE(battle.submit(command));
}
bool damaged(const BattleState& battle, EntityId target, double amount) {
  return std::any_of(battle.events().begin(),battle.events().end(),[&](const Event& event) {
    return event.type==EventType::Damaged&&event.target==target&&std::abs(event.value-amount)<1e-9;
  });
}
} // namespace

int main() {
  const auto data=GameData::v0();
  const auto* dragon=data.find(Kind::SuperDragon,13);
  COCSIM_REQUIRE(dragon&&dragon->hp==8400&&dragon->dps==537&&dragon->damage==966.6);
  COCSIM_REQUIRE(dragon->cooldown==1808&&dragon->range==3&&dragon->flying&&dragon->target_type==TargetType::Both);

  Scenario scenario; scenario.width=20; scenario.height=20; scenario.duration_ms=4000;
  scenario.defenders={{Kind::Cannon,1,{4.5,10.5}}}; scenario.army={{Kind::SuperDragon,13,1}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::SuperDragon,13,{1.5,10.5}}};
  BattleState battle(data,scenario); submit_all(battle,commands); battle.advance_ticks(2);
  const auto projectiles=battle.observe_projectiles();
  COCSIM_REQUIRE(projectiles.size()==1&&projectiles.front().owner==2&&projectiles.front().target_position.x==4.5);
  const auto snapshot=battle.snapshot(); BattleState restored(data,scenario);
  COCSIM_REQUIRE(restored.restore(snapshot)&&restored.state_hash()==battle.state_hash());
  const std::string replay_path="replay-super-dragon-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path,scenario,commands,error)); Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error)); BattleState replay(data,replay_scenario); submit_all(replay,replay_commands); replay.advance_ticks(2);
  COCSIM_REQUIRE(replay.state_hash()==battle.state_hash()); battle.advance_ticks(1); restored.advance_ticks(1); replay.advance_ticks(1);
  COCSIM_REQUIRE(damaged(battle,1,966.6)&&battle.state_hash()==restored.state_hash()&&battle.state_hash()==replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
