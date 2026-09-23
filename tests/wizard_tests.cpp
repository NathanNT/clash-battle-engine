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
} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* wizard = data.find(Kind::Wizard, 14);
  COCSIM_REQUIRE(wizard && wizard->hp == 330 && wizard->dps == 310 && wizard->damage == 465
                 && wizard->cooldown == 1504 && wizard->range == 3
                 && wizard->movement_tiles_per_second == 2 && wizard->housing_space == 4
                 && wizard->target_focus == TargetFocus::Any && wizard->target_type == TargetType::Both
                 && wizard->splash && wizard->splash_radius == .3);

  // The sourced radius reaches the nearby target through the generic Core
  // splash path. Projectile and collision geometry are intentionally not
  // inferred from this compact radius regression.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 8000;
  scenario.defenders = {{Kind::Cannon, 1, {10.5, 10.5}}, {Kind::GoldMine, 1, {10.75, 10.5}}};
  scenario.army = {{Kind::Wizard, 14, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Wizard, 14, {7.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(500);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 3 && event.target == 1 && event.value == 465;
  }));
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Damaged && event.actor == 3 && event.target == 2 && event.value == 465;
  }));

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-wizard-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(500);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
