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

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* level_one = data.find(Kind::Monolith, 1);
  const auto* level_five = data.find(Kind::Monolith, 5);
  COCSIM_REQUIRE(level_one && level_one->footprint_width == 3 && level_one->footprint_height == 3);
  COCSIM_REQUIRE(level_one->damage == 225 && level_one->max_hp_damage_percent == 11);
  COCSIM_REQUIRE(level_one->cooldown == 1504 && level_one->range == 11 && level_one->target_type == TargetType::Both);
  COCSIM_REQUIRE(level_five && level_five->hp == 5959 && level_five->damage == 337.5
                 && level_five->dps == 225 && level_five->max_hp_damage_percent == 15
                 && level_five->attributes.town_hall_required == 18);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 5000;
  scenario.defenders = {{Kind::Monolith, 1, {12.5, 10.5}}};
  scenario.army = {{Kind::Dragon, 1, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Dragon, 1, {6.5, 10.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(3);
  // MONOLITH_SNAPSHOT_REPLAY: the projectile already contains its per-target
  // maximum-HP component, so this checkpoint catches an accidental recompute
  // from display state after restoration or replay.
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Projectile && event.actor == 1 && event.target == 2
        && std::abs(event.value - 434.0) < 1e-9;
  }));
  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-monolith-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(3);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(100);
  restored.advance_ticks(100);
  replay.advance_ticks(100);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Damaged && event.actor == 1 && event.target == 2
        && std::abs(event.value - 434.0) < 1e-9;
  }));
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
