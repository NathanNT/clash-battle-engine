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
    return event.type == EventType::Damaged && event.target == target
      && std::abs(event.value - amount) < 1e-9;
  });
}
} // namespace

int main() {
  const auto data=GameData::v0();
  const auto* wizard=data.find(Kind::SuperWizard,14);
  COCSIM_REQUIRE(wizard && wizard->hp==925 && wizard->damage==670 && wizard->cooldown==2000);
  COCSIM_REQUIRE(std::abs(wizard->chain_damage_multiplier-.4)<1e-9);
  COCSIM_REQUIRE(wizard->chain_target_count==10 && wizard->chain_radius==3);

  // The source does not provide chain geometry. The versioned delta therefore
  // captures up to ten total targets around the primary using the documented
  // three-tile proxy and deterministic distance/entity-ID ordering.
  Scenario scenario; scenario.width=20; scenario.height=20; scenario.duration_ms=4000;
  scenario.defenders={{Kind::Cannon,1,{4.5,10.5}},{Kind::Cannon,1,{5.5,10.5}},{Kind::Cannon,1,{6.5,10.5}}};
  scenario.army={{Kind::SuperWizard,14,1}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::SuperWizard,14,{1.5,10.5}}};
  BattleState battle(data,scenario); submit_all(battle,commands);
  battle.advance_ticks(2); // T+10 launch; all branches are serialised projectiles.
  const auto projectiles=battle.observe_projectiles();
  COCSIM_REQUIRE(std::count_if(projectiles.begin(), projectiles.end(), [](const ProjectileView& projectile) {
    return projectile.owner==4;
  })==3);
  const auto snapshot=battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-18\n"));
  BattleState restored(data,scenario); COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash()==battle.state_hash());
  const std::string replay_path="replay-super-wizard-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path,scenario,commands,error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error));
  BattleState replay(data,replay_scenario); submit_all(replay,replay_commands); replay.advance_ticks(2);
  COCSIM_REQUIRE(replay.state_hash()==battle.state_hash());
  battle.advance_ticks(1); restored.advance_ticks(1); replay.advance_ticks(1);
  COCSIM_REQUIRE(damaged(battle,1,670));
  COCSIM_REQUIRE(damaged(battle,2,268));
  COCSIM_REQUIRE(damaged(battle,3,268));
  COCSIM_REQUIRE(battle.state_hash()==restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash()==replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
