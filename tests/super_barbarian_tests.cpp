#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

bool has_damage(const BattleState& battle, double value, Milliseconds min_time = 0) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.actor == 2 && event.target == 1
        && event.time_ms >= min_time && std::abs(event.value - value) < 1e-9;
  });
}

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* super = data.find(Kind::SuperBarbarian, 8);
  COCSIM_REQUIRE(super && super->hp == 1000 && super->damage == 144 && super->cooldown == 800);
  COCSIM_REQUIRE(super->speed == 20 && super->movement_tiles_per_second == 2.5);
  COCSIM_REQUIRE(super->deployment_rage_duration == 8000);
  COCSIM_REQUIRE(std::abs(super->deployment_rage_damage_multiplier - 1.7) < 1e-9);
  COCSIM_REQUIRE(std::abs(super->deployment_rage_movement_speed_multiplier - 1.8) < 1e-9);

  // The same target is deliberately durable enough to cross the deployment
  // boundary. The first 10-ms movement step is 2.5 * 1.8 * 10 / 1000 tiles;
  // no Viewer interpolation or wall clock can alter that logical result.
  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 11000;
  scenario.defenders = {{Kind::GoldStorage, 19, {12.5, 10.5}}};
  scenario.army = {{Kind::SuperBarbarian, 8, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SuperBarbarian, 8, {1.5, 10.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2);
  const auto first_observation = battle.observe();
  const auto super_view = std::find_if(first_observation.begin(), first_observation.end(), [](const EntityView& entity) {
    return entity.kind == Kind::SuperBarbarian && entity.side == Side::Attacker;
  });
  COCSIM_REQUIRE(super_view != first_observation.end());
  COCSIM_REQUIRE(super_view->deployment_rage_active);
  COCSIM_REQUIRE(std::abs(super_view->position.x - 1.545) < 1e-9);

  battle.advance_ticks(398); // T+4000 ms: the deployment window is pending.
  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-18\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-super-barbarian-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(400);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(600);
  restored.advance_ticks(600);
  replay.advance_ticks(600);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  // The strict deadline means T+8010 and later uses base damage. This test
  // guards both the source-derived 70% boost and the serialised future state.
  COCSIM_REQUIRE(has_damage(battle, 244.8));
  COCSIM_REQUIRE(has_damage(battle, 144.0, 8010));
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
