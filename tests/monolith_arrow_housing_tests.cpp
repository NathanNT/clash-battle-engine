#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

int main() {
  const auto data = GameData::v0();
  const auto* dragon = data.find(Kind::ElectroDragon, 9);
  COCSIM_REQUIRE(dragon && dragon->housing_space == 30);
  Scenario scenario;
  scenario.width = 40;
  scenario.height = 30;
  scenario.duration_ms = 3000;
  scenario.defenders = {{Kind::GoldStorage, 1, {35.5, 25.5}}};
  scenario.army = {{Kind::ArcherQueen, 110, 1}, {Kind::ElectroDragon, 9, 8}};
  std::vector<Command> commands;
  commands.push_back({CommandType::Deploy, kTickMs, 0, 0, Kind::ArcherQueen, 110, {1.5, 1.5}});
  for (int i = 0; i < 8; ++i)
    commands.push_back({CommandType::Deploy, (i + 2) * kTickMs, 0, 0,
                        Kind::ElectroDragon, 9, {1.5 + i, 1.5}});

  BattleState battle(data, scenario);
  COCSIM_REQUIRE(battle.monolith_arrow_deployed_housing() == 0);
  COCSIM_REQUIRE(battle.monolith_arrow_housing_tier() == 0);
  COCSIM_REQUIRE(battle.monolith_arrow_damage_percent() == 14);
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
  battle.advance_to(2 * kTickMs);
  COCSIM_REQUIRE(battle.deployed_housing() == 0);
  COCSIM_REQUIRE(battle.monolith_arrow_deployed_housing() == 25);
  battle.advance_to(7 * kTickMs);
  COCSIM_REQUIRE(battle.monolith_arrow_deployed_housing() == 175);
  COCSIM_REQUIRE(battle.monolith_arrow_housing_tier() == 0);
  COCSIM_REQUIRE(battle.monolith_arrow_damage_percent() == 14);
  battle.advance_to(8 * kTickMs);
  COCSIM_REQUIRE(battle.monolith_arrow_deployed_housing() == 205);
  COCSIM_REQUIRE(battle.monolith_arrow_housing_tier() == 1);
  COCSIM_REQUIRE(battle.monolith_arrow_damage_percent() == 10);

  const auto snapshot = battle.snapshot();
  const auto hash = battle.state_hash();
  BattleState restored(data, scenario);
  std::string error;
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  COCSIM_REQUIRE(restored.state_hash() == hash);
  COCSIM_REQUIRE(restored.monolith_arrow_deployed_housing() == 205);
  COCSIM_REQUIRE(restored.monolith_arrow_housing_tier() == 1);
  COCSIM_REQUIRE(restored.monolith_arrow_damage_percent() == 10);
  battle.advance_to(10 * kTickMs);
  restored.advance_to(10 * kTickMs);
  COCSIM_REQUIRE(battle.monolith_arrow_deployed_housing() == 265);
  COCSIM_REQUIRE(battle.monolith_arrow_housing_tier() == 2);
  COCSIM_REQUIRE(battle.monolith_arrow_damage_percent() == 5);
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-monolith-arrow-housing-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_to(10 * kTickMs);
  COCSIM_REQUIRE(replay.monolith_arrow_deployed_housing() == 265);
  COCSIM_REQUIRE(replay.monolith_arrow_housing_tier() == 2);
  COCSIM_REQUIRE(replay.monolith_arrow_damage_percent() == 5);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  // Exercise the exact published endpoints, not only a sample inside each
  // bracket. Same-tick deployment commands keep their submitted sequence.
  Scenario boundaries = scenario;
  boundaries.army = {{Kind::ArcherQueen, 110, 1}, {Kind::ElectroDragon, 9, 5},
                     {Kind::Barbarian, 1, 76}};
  BattleState boundary_battle(data, boundaries);
  COCSIM_REQUIRE(boundary_battle.submit({CommandType::Deploy, kTickMs, 0, 0,
                                         Kind::ArcherQueen, 110, {1.5, 1.5}}));
  for (int i = 0; i < 5; ++i)
    COCSIM_REQUIRE(boundary_battle.submit({CommandType::Deploy, kTickMs, 0, 0,
                                           Kind::ElectroDragon, 9, {2.5 + i, 2.5}}));
  for (int i = 0; i < 76; ++i) {
    const auto effective = (i < 5 ? 2 : i == 5 ? 3 : i < 75 ? 4 : 5) * kTickMs;
    COCSIM_REQUIRE(boundary_battle.submit({CommandType::Deploy, effective, 0, 0,
                                           Kind::Barbarian, 1, {1.5, 3.5}}));
  }
  const int expected_housing[] = {175, 180, 181, 250, 251};
  const int expected_percent[] = {14, 14, 10, 10, 5};
  for (int i = 0; i < 5; ++i) {
    boundary_battle.advance_to((i + 2) * kTickMs);
    COCSIM_REQUIRE(boundary_battle.monolith_arrow_deployed_housing() == expected_housing[i]);
    COCSIM_REQUIRE(boundary_battle.monolith_arrow_damage_percent() == expected_percent[i]);
  }
  return 0;
}
