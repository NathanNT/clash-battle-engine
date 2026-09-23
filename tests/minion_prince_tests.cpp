#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
const EntityView* prince(const std::vector<EntityView>& entities) {
  for (const auto& entity : entities)
    if (entity.kind == Kind::MinionPrince && entity.side == Side::Attacker) return &entity;
  return nullptr;
}
}

int main() {
  const auto data = GameData::v0();
  COCSIM_REQUIRE(!data.find(Kind::MinionPrince, 90));
  const auto* l91 = data.find(Kind::MinionPrince, 91);
  const auto* l95 = data.find(Kind::MinionPrince, 95);
  COCSIM_REQUIRE(l91 && l95);
  COCSIM_REQUIRE(l91->hp == 4390 && l91->damage == 640.05 && l91->cooldown == 848);
  COCSIM_REQUIRE(l95->hp == 4510 && l95->damage == 650.25 && l95->dps == 765);
  COCSIM_REQUIRE(l95->category == EntityCategory::Troop && l95->target_type == TargetType::Both);
  COCSIM_REQUIRE(l95->flying && l95->range == 4.5 && l95->speed == 24.0);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::Cannon, 1, {13.5, 10.5}}};
  scenario.army = {{Kind::MinionPrince, 95, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::MinionPrince, 95, {4.5, 10.5}},
  };
  BattleState first(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(first.submit(command));
  first.advance_ticks(80);
  COCSIM_REQUIRE(prince(first.observe()));
  const auto snapshot = first.snapshot();
  const auto expected_hash = first.state_hash();
  BattleState restored(data, scenario);
  std::string error;
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  COCSIM_REQUIRE(restored.state_hash() == expected_hash);
  first.advance_ticks(100);
  restored.advance_ticks(100);
  COCSIM_REQUIRE(first.state_hash() == restored.state_hash());
  const std::string replay_path = "replay-minion-prince-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(180);
  COCSIM_REQUIRE(replay.state_hash() == first.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  Scenario defended;
  defended.width = 24;
  defended.height = 20;
  defended.duration_ms = 3000;
  defended.defenders = {{Kind::MinionPrince, 95, {12.5, 10.5}}};
  defended.army = {{Kind::Barbarian, 1, 1}};
  BattleState melee(data, defended);
  COCSIM_REQUIRE(melee.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {8.5, 10.5}}));
  melee.advance_ticks(2);
  const auto melee_views = melee.observe();
  const auto melee_barbarian = std::find_if(melee_views.begin(), melee_views.end(), [](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == Kind::Barbarian;
  });
  COCSIM_REQUIRE(melee_barbarian != melee_views.end() && !melee_barbarian->target);

  defended.army = {{Kind::Archer, 1, 1}};
  BattleState ranged(data, defended);
  const std::vector<Command> ranged_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Archer, 1, {8.5, 10.5}},
  };
  for (const auto& command : ranged_commands) COCSIM_REQUIRE(ranged.submit(command));
  ranged.advance_ticks(2);
  const auto ranged_views = ranged.observe();
  const auto ranged_archer = std::find_if(ranged_views.begin(), ranged_views.end(), [](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == Kind::Archer;
  });
  COCSIM_REQUIRE(ranged_archer != ranged_views.end() && ranged_archer->target);
  const auto ranged_snapshot = ranged.snapshot();
  BattleState ranged_restored(data, defended);
  COCSIM_REQUIRE(ranged_restored.restore(ranged_snapshot, &error));
  ranged.advance_ticks(20);
  ranged_restored.advance_ticks(20);
  COCSIM_REQUIRE(ranged.state_hash() == ranged_restored.state_hash());
  const std::string defender_replay_path = "replay-minion-prince-defender-targetability-test.json";
  COCSIM_REQUIRE(save_replay(defender_replay_path, defended, ranged_commands, error));
  COCSIM_REQUIRE(load_replay(defender_replay_path, replay_scenario, replay_commands, error));
  BattleState defender_replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(defender_replay.submit(command));
  defender_replay.advance_ticks(22);
  COCSIM_REQUIRE(defender_replay.state_hash() == ranged.state_hash());
  COCSIM_REQUIRE(std::remove(defender_replay_path.c_str()) == 0);
  return 0;
}
