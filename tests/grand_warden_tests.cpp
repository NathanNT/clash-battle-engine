#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
const EntityView* warden(const std::vector<EntityView>& entities) {
  for (const auto& entity : entities)
    if (entity.kind == Kind::GrandWarden && entity.side == Side::Attacker) return &entity;
  return nullptr;
}
}

int main() {
  const auto data = GameData::v0();
  COCSIM_REQUIRE(!data.find(Kind::GrandWarden, 75));
  const auto* l76 = data.find(Kind::GrandWarden, 76);
  const auto* l85 = data.find(Kind::GrandWarden, 85);
  COCSIM_REQUIRE(l76 && l85);
  COCSIM_REQUIRE(l76->hp == 2516 && l76->damage == 588.6 && l76->cooldown == 1808);
  COCSIM_REQUIRE(l85->hp == 2669 && l85->damage == 637.2 && l85->dps == 354);
  COCSIM_REQUIRE(l85->category == EntityCategory::Troop && l85->target_type == TargetType::Both);
  COCSIM_REQUIRE(!l85->flying && l85->range == 7.0);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::GoldMine, 1, {13.5, 10.5}}};
  scenario.army = {{Kind::GrandWarden, 85, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::GrandWarden, 85, {4.5, 10.5}},
  };
  BattleState first(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(first.submit(command));
  first.advance_ticks(80);
  COCSIM_REQUIRE(warden(first.observe()));
  const auto snapshot = first.snapshot();
  const auto expected_hash = first.state_hash();
  BattleState restored(data, scenario);
  std::string error;
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  COCSIM_REQUIRE(restored.state_hash() == expected_hash);
  first.advance_ticks(100);
  restored.advance_ticks(100);
  COCSIM_REQUIRE(first.state_hash() == restored.state_hash());
  const std::string replay_path = "replay-grand-warden-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(180);
  COCSIM_REQUIRE(replay.state_hash() == first.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  // The pinned Grand Warden description explicitly offers an air form.  It
  // shares the normal ArmySlot and command path, but its persistent mode makes
  // the Core entity fly and therefore bypass the wall without a GUI rule.
  Scenario air_route;
  air_route.width = 30;
  air_route.height = 20;
  air_route.duration_ms = 6000;
  air_route.defenders = {{Kind::Wall, 1, {10.5, 10.5}}, {Kind::Cannon, 1, {18.5, 10.5}}};
  air_route.army = {{Kind::GrandWarden, 85, 1, "air"}};
  const std::vector<Command> air_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::GrandWarden, 85, {5.5, 10.5}},
  };
  BattleState air_battle(data, air_route);
  for (const auto& command : air_commands) COCSIM_REQUIRE(air_battle.submit(command));
  air_battle.advance_ticks(125);
  const auto air_entities = air_battle.observe();
  const auto* air_warden = warden(air_entities);
  COCSIM_REQUIRE(air_warden);
  COCSIM_REQUIRE(air_warden->flying);
  // The cannon's sourced 7-tile attack range lets the Warden stop before the
  // wall. A direct, unchanged y coordinate proves the flying form selected
  // the straight Core path instead of routing around the wall.
  COCSIM_REQUIRE(air_warden->position.x > 9.0);
  COCSIM_REQUIRE(std::abs(air_warden->position.y - 10.5) < 1e-9);
  COCSIM_REQUIRE(air_warden->target == 2);
  const auto air_snapshot = air_battle.snapshot();
  BattleState air_restored(data, air_route);
  COCSIM_REQUIRE(air_restored.restore(air_snapshot, &error));
  air_battle.advance_ticks(63);
  air_restored.advance_ticks(63);
  COCSIM_REQUIRE(air_battle.state_hash() == air_restored.state_hash());
  const std::string air_replay_path = "replay-grand-warden-air-test.json";
  COCSIM_REQUIRE(save_replay(air_replay_path, air_route, air_commands, error));
  COCSIM_REQUIRE(load_replay(air_replay_path, replay_scenario, replay_commands, error));
  COCSIM_REQUIRE(replay_scenario.army.size() == 1 && replay_scenario.army[0].mode == "air");
  BattleState air_replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(air_replay.submit(command));
  air_replay.advance_ticks(188);
  COCSIM_REQUIRE(air_replay.state_hash() == air_battle.state_hash());
  COCSIM_REQUIRE(std::remove(air_replay_path.c_str()) == 0);
  return 0;
}
