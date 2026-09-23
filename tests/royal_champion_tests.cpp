#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
const EntityView* champion(const std::vector<EntityView>& entities) {
  for (const auto& entity : entities)
    if (entity.kind == Kind::RoyalChampion && entity.side == Side::Attacker) return &entity;
  return nullptr;
}
}

int main() {
  const auto data = GameData::v0();
  COCSIM_REQUIRE(!data.find(Kind::RoyalChampion, 50));
  const auto* l51 = data.find(Kind::RoyalChampion, 51);
  const auto* l55 = data.find(Kind::RoyalChampion, 55);
  COCSIM_REQUIRE(l51 && l55);
  COCSIM_REQUIRE(l51->hp == 4194 && l51->damage == 675.6 && l51->cooldown == 1200);
  COCSIM_REQUIRE(l55->hp == 4298 && l55->damage == 690.0 && l55->dps == 575);
  COCSIM_REQUIRE(l55->category == EntityCategory::Troop && l55->target_type == TargetType::Both);
  COCSIM_REQUIRE(l55->range == 3.0 && l55->speed == 24.0 && l55->jumps_walls);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::Cannon, 1, {13.5, 10.5}}};
  scenario.army = {{Kind::RoyalChampion, 55, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::RoyalChampion, 55, {4.5, 10.5}},
  };
  BattleState first(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(first.submit(command));
  first.advance_ticks(80);
  COCSIM_REQUIRE(champion(first.observe()));
  const auto snapshot = first.snapshot();
  const auto expected_hash = first.state_hash();
  BattleState restored(data, scenario);
  std::string error;
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  COCSIM_REQUIRE(restored.state_hash() == expected_hash);
  first.advance_ticks(100);
  restored.advance_ticks(100);
  COCSIM_REQUIRE(first.state_hash() == restored.state_hash());

  Scenario wall_route;
  wall_route.width = 30;
  wall_route.height = 20;
  wall_route.duration_ms = 6000;
  wall_route.defenders = {{Kind::Wall, 1, {10.5, 10.5}}, {Kind::Cannon, 1, {18.5, 10.5}}};
  wall_route.army = {{Kind::RoyalChampion, 55, 1}};
  const std::vector<Command> wall_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::RoyalChampion, 55, {5.5, 10.5}},
  };
  BattleState wall_battle(data, wall_route);
  for (const auto& command : wall_commands) COCSIM_REQUIRE(wall_battle.submit(command));
  wall_battle.advance_ticks(125);
  const auto wall_entities = wall_battle.observe();
  const auto* crossing_champion = champion(wall_entities);
  const auto wall = std::find_if(wall_entities.begin(), wall_entities.end(), [](const EntityView& entity) {
    return entity.kind == Kind::Wall && entity.side == Side::Defender;
  });
  COCSIM_REQUIRE(crossing_champion && crossing_champion->position.x > 10.5
                 && crossing_champion->target == 2 && wall != wall_entities.end());
  const auto wall_snapshot = wall_battle.snapshot();
  BattleState wall_restored(data, wall_route);
  COCSIM_REQUIRE(wall_restored.restore(wall_snapshot, &error));
  wall_battle.advance_ticks(63);
  wall_restored.advance_ticks(63);
  COCSIM_REQUIRE(wall_battle.state_hash() == wall_restored.state_hash());
  const std::string replay_path = "replay-royal-champion-wall-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, wall_route, wall_commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(188);
  COCSIM_REQUIRE(replay.state_hash() == wall_battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  // Wall crossing belongs to the Champion's sourced movement capability, not
  // to the attacking adapter. An explicit defender placement must therefore
  // take the same Core wall-bypass route while pursuing its only attacker.
  Scenario defender_wall_route;
  defender_wall_route.width = 30;
  defender_wall_route.height = 20;
  defender_wall_route.duration_ms = 6000;
  defender_wall_route.defenders = {{Kind::RoyalChampion, 55, {18.5, 10.5}}, {Kind::Wall, 1, {12.5, 10.5}}};
  defender_wall_route.army = {{Kind::Healer, 1, 1}};
  const std::vector<Command> defender_wall_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Healer, 1, {5.5, 10.5}},
  };
  BattleState defender_wall_battle(data, defender_wall_route);
  for (const auto& command : defender_wall_commands) COCSIM_REQUIRE(defender_wall_battle.submit(command));
  defender_wall_battle.advance_ticks(300);
  const auto defender_wall_entities = defender_wall_battle.observe();
  const auto defender_champion = std::find_if(defender_wall_entities.begin(), defender_wall_entities.end(), [](const EntityView& entity) {
    return entity.kind == Kind::RoyalChampion && entity.side == Side::Defender;
  });
  COCSIM_REQUIRE(defender_champion != defender_wall_entities.end());
  COCSIM_REQUIRE(defender_champion->position.x < 12.5);
  const auto defender_snapshot = defender_wall_battle.snapshot();
  BattleState defender_restored(data, defender_wall_route);
  COCSIM_REQUIRE(defender_restored.restore(defender_snapshot, &error));
  defender_wall_battle.advance_ticks(63);
  defender_restored.advance_ticks(100);
  COCSIM_REQUIRE(defender_wall_battle.state_hash() == defender_restored.state_hash());
  const std::string defender_replay_path = "replay-royal-champion-defender-wall-test.json";
  COCSIM_REQUIRE(save_replay(defender_replay_path, defender_wall_route, defender_wall_commands, error));
  COCSIM_REQUIRE(load_replay(defender_replay_path, replay_scenario, replay_commands, error));
  BattleState defender_replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(defender_replay.submit(command));
  defender_replay.advance_ticks(400);
  COCSIM_REQUIRE(defender_replay.state_hash() == defender_wall_battle.state_hash());
  COCSIM_REQUIRE(std::remove(defender_replay_path.c_str()) == 0);
  return 0;
}
