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
  const auto* cannon=data.find(Kind::RicochetCannon,4);
  COCSIM_REQUIRE(cannon && cannon->hp==6100 && cannon->damage==329.6 && cannon->cooldown==800);
  COCSIM_REQUIRE(std::abs(cannon->chain_damage_multiplier-.7)<1e-9);
  COCSIM_REQUIRE(cannon->chain_target_count==2 && std::abs(cannon->chain_radius-3.5)<1e-9);

  // The documented bounce reaches one eligible ground troop within 3.5 tiles.
  // The source does not publish its tie rule; Core makes its distance/ID rule
  // explicit and serialises the two logical projectiles.
  Scenario scenario; scenario.width=20; scenario.height=20; scenario.duration_ms=4000;
  scenario.defenders={{Kind::RicochetCannon,4,{10.5,10.5}}};
  scenario.army={{Kind::Barbarian,1,3}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{7.5,10.5}},
                                       {CommandType::Deploy,kTickMs,0,1,Kind::Barbarian,1,{5.0,10.5}},
                                       {CommandType::Deploy,kTickMs,0,2,Kind::Barbarian,1,{1.5,10.5}}};
  BattleState battle(data,scenario); submit_all(battle,commands); battle.advance_ticks(2);
  const auto projectiles=battle.observe_projectiles();
  COCSIM_REQUIRE(std::count_if(projectiles.begin(), projectiles.end(),
    [](const ProjectileView& p){ return p.owner==1; })==2);
  const auto snapshot=battle.snapshot();
  BattleState restored(data,scenario); COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash()==battle.state_hash());
  const std::string replay_path="replay-ricochet-cannon-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path,scenario,commands,error)); Scenario rs; std::vector<Command> rc;
  COCSIM_REQUIRE(load_replay(replay_path,rs,rc,error)); BattleState replay(data,rs); submit_all(replay,rc); replay.advance_ticks(2);
  battle.advance_ticks(1); restored.advance_ticks(1); replay.advance_ticks(1);
  COCSIM_REQUIRE(damaged(battle,2,329.6));
  COCSIM_REQUIRE(damaged(battle,3,230.72));
  COCSIM_REQUIRE(!damaged(battle,4,230.72));
  COCSIM_REQUIRE(battle.state_hash()==restored.state_hash() && battle.state_hash()==replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
