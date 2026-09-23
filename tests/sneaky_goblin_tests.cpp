#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}
}

int main() {
  const auto data = GameData::v0();
  const int expected_hp[] = {270, 320, 350};
  const int expected_damage[] = {155, 170, 180};
  for (int level = 7; level <= 9; ++level) {
    const auto* sneaky = data.find(Kind::SneakyGoblin, level);
    COCSIM_REQUIRE(sneaky && sneaky->hp == expected_hp[level - 7]
                   && sneaky->damage == expected_damage[level - 7]
                   && sneaky->dps == expected_damage[level - 7]
                   && sneaky->cooldown == 1008 && sneaky->range == .4
                   && sneaky->speed == 32 && sneaky->movement_tiles_per_second == 4
                   && sneaky->housing_space == 3 && !sneaky->flying
                   && sneaky->target_focus == TargetFocus::Resources
                   && sneaky->target_type == TargetType::Ground
                   && sneaky->resource_damage_multiplier == 2
                   && sneaky->defense_invisibility_duration == 5008 && sneaky->deployable);
  }
  COCSIM_REQUIRE(data.max_level(Kind::SneakyGoblin) == 9 && !data.find(Kind::SneakyGoblin, 10));

  Scenario resources;
  resources.width = 20; resources.height = 20; resources.duration_ms = 6000;
  resources.defenders = {{Kind::Cannon, 1, {8.5, 10.5}}, {Kind::GoldMine, 1, {12.5, 10.5}}};
  resources.army = {{Kind::SneakyGoblin, 9, 1}};
  const std::vector<Command> resource_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SneakyGoblin, 9, {5.5, 10.5}},
  };
  BattleState resource_battle(data, resources);
  submit_all(resource_battle, resource_commands); resource_battle.advance_ticks(156);
  COCSIM_REQUIRE(std::any_of(resource_battle.events().begin(), resource_battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.value == 360;
  }));
  COCSIM_REQUIRE(std::none_of(resource_battle.events().begin(), resource_battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.value == 190;
  }));

  Scenario cloak;
  cloak.width = 30; cloak.height = 20; cloak.duration_ms = 8000;
  cloak.defenders = {{Kind::Cannon, 1, {12.5, 10.5}}, {Kind::GoldStorage, 19, {18.5, 10.5}}};
  cloak.army = {{Kind::SneakyGoblin, 9, 1}};
  const std::vector<Command> cloak_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SneakyGoblin, 9, {6.5, 10.5}},
  };
  BattleState battle(data, cloak);
  submit_all(battle, cloak_commands); battle.advance_ticks(63);
  const auto views = battle.observe();
  COCSIM_REQUIRE(std::any_of(views.begin(), views.end(), [](const EntityView& entity) {
    return entity.kind == Kind::SneakyGoblin && entity.side == Side::Attacker && entity.invisible_to_defenses;
  }));
  const auto snapshot = battle.snapshot();
  BattleState restored(data, cloak);
  COCSIM_REQUIRE(restored.restore(snapshot) && restored.state_hash() == battle.state_hash());
  battle.advance_ticks(250);
  COCSIM_REQUIRE(std::none_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 1;
  }));
  battle.advance_ticks(2);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 1;
  }));
  restored.advance_ticks(252);
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-sneaky-goblin-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, cloak, cloak_commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands); replay.advance_ticks(315);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
