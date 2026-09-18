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
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}
bool damaged(const BattleState& battle, EntityId target, double amount) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type==EventType::Damaged && event.target==target && std::abs(event.value-amount)<1e-9;
  });
}
} // namespace

int main() {
  const auto data=GameData::v0();
  const auto* bowler=data.find(Kind::SuperBowler,10);
  COCSIM_REQUIRE(bowler && bowler->hp==3700 && bowler->damage==682 && bowler->cooldown==2200);
  COCSIM_REQUIRE(bowler->bounce_impact_count==3 && bowler->bounce_step==3 && bowler->bounce_splash_radius==.6);

  Scenario scenario; scenario.width=20; scenario.height=20; scenario.duration_ms=4000;
  scenario.defenders={{Kind::Cannon,1,{4.5,10.5}},{Kind::Cannon,1,{7.5,10.5}},{Kind::Cannon,1,{10.5,10.5}}};
  scenario.army={{Kind::SuperBowler,10,1}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::SuperBowler,10,{1.5,10.5}}};
  BattleState battle(data,scenario); submit_all(battle,commands);
  battle.advance_ticks(2);
  const auto all_projectiles=battle.observe_projectiles();
  std::vector<ProjectileView> projectiles;
  for(const auto& projectile:all_projectiles) if(projectile.owner==4) projectiles.push_back(projectile);
  COCSIM_REQUIRE(projectiles.size()==3);
  COCSIM_REQUIRE(projectiles[0].target_position.x==4.5 && projectiles[1].target_position.x==7.5 && projectiles[2].target_position.x==10.5);
  const auto snapshot=battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-18\n"));
  BattleState restored(data,scenario); COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash()==battle.state_hash());
  const std::string replay_path="replay-super-bowler-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path,scenario,commands,error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error));
  BattleState replay(data,replay_scenario); submit_all(replay,replay_commands); replay.advance_ticks(2);
  COCSIM_REQUIRE(replay.state_hash()==battle.state_hash());
  battle.advance_ticks(1); restored.advance_ticks(1); replay.advance_ticks(1);
  COCSIM_REQUIRE(damaged(battle,1,682) && damaged(battle,2,682) && damaged(battle,3,682));
  COCSIM_REQUIRE(battle.state_hash()==restored.state_hash() && battle.state_hash()==replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
