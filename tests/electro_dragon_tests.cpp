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
  const auto data = GameData::v0();
  const auto* electro_dragon = data.find(Kind::ElectroDragon, 9);
  COCSIM_REQUIRE(electro_dragon && electro_dragon->hp == 6900 && electro_dragon->damage == 1785);
  COCSIM_REQUIRE(electro_dragon->cooldown == 3504 && electro_dragon->range == 2.5);
  COCSIM_REQUIRE(std::abs(electro_dragon->chain_damage_multiplier - .8) < 1e-9);
  COCSIM_REQUIRE(electro_dragon->chain_target_count == 5 && electro_dragon->chain_radius == 1);

  // The primary target is closest. Two possible second hops are equidistant,
  // and the Town Hall wins by maximum HP. Subsequent hops follow the line and
  // prove that selection is sequential rather than a fan-out from the first.
  Scenario scenario;
  scenario.width = 30;
  scenario.height = 24;
  scenario.duration_ms = 5000;
  scenario.defenders = {
    {Kind::GoldMine, 1, {4.0, 10.0}},
    {Kind::TownHall, 1, {8.0, 10.0}},
    {Kind::GoldMine, 1, {4.0, 14.0}},
    {Kind::GoldMine, 1, {12.0, 10.0}},
    {Kind::GoldMine, 1, {16.0, 10.0}},
    {Kind::GoldMine, 1, {20.0, 10.0}},
  };
  scenario.army = {{Kind::ElectroDragon, 9, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::ElectroDragon, 9, {0.5, 10.0}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2);
  const auto projectiles = battle.observe_projectiles();
  COCSIM_REQUIRE(std::count_if(projectiles.begin(), projectiles.end(), [](const ProjectileView& projectile) {
    return projectile.owner == 7;
  }) == 5);
  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-electro-dragon-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(2);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(1);
  restored.advance_ticks(1);
  replay.advance_ticks(1);
  COCSIM_REQUIRE(damaged(battle, 1, 1785));
  COCSIM_REQUIRE(damaged(battle, 2, 1428));
  COCSIM_REQUIRE(!damaged(battle, 3, 1142.4));
  COCSIM_REQUIRE(damaged(battle, 4, 1142.4));
  COCSIM_REQUIRE(damaged(battle, 5, 913.92));
  COCSIM_REQUIRE(damaged(battle, 6, 731.136));
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
