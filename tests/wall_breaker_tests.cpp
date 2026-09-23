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
bool has_damage(const BattleState& battle, EntityId actor, EntityId target, double value) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.actor == actor && event.target == target
        && std::abs(event.value - value) < 1e-9;
  });
}
}

int main() {
  const auto data = GameData::v0();
  const auto* breaker = data.find(Kind::WallBreaker, 14);
  COCSIM_REQUIRE(breaker && breaker->hp == 170 && breaker->damage == 126 && breaker->dps == 126
      && breaker->death_damage == 78 && breaker->cooldown == 1008 && breaker->range == 1
      && breaker->housing_space == 2 && breaker->target_focus == TargetFocus::Walls
      && breaker->target_type == TargetType::Ground && breaker->wall_damage_multiplier == 40
      && breaker->splash_radius == 2 && breaker->death_splash_radius == 2
      && breaker->self_destruct_on_attack);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 5000;
  scenario.defenders = {{Kind::Wall, 1, {12.5, 10.5}}, {Kind::Wall, 1, {12.5, 11.5}}};
  scenario.army = {{Kind::WallBreaker, 14, 1}};
  const std::vector<Command> commands = {
      {CommandType::Deploy, kTickMs, 0, 0, Kind::WallBreaker, 14, {6.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(30);
  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-wall-breaker-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(30);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  battle.advance_ticks(200);
  restored.advance_ticks(200);
  replay.advance_ticks(200);
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(has_damage(battle, 3, 1, 5040));
  COCSIM_REQUIRE(has_damage(battle, 3, 2, 5040));
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Destroyed && event.actor == 3 && event.target == 3
        && event.detail == "wall_breaker";
  }));
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
