#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

int main() {
  const auto data = GameData::v0();
  const auto* valkyrie = data.find(Kind::SuperValkyrie, 12);
  COCSIM_REQUIRE(valkyrie && valkyrie->hp == 4500 && valkyrie->damage == 445.5);
  COCSIM_REQUIRE(valkyrie->cooldown == 1104 && valkyrie->range == .6 && valkyrie->splash);

  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 4000;
  scenario.defenders = {{Kind::Cannon, 1, {5.5, 10.5}}, {Kind::Cannon, 1, {6.5, 10.5}}};
  scenario.army = {{Kind::SuperValkyrie, 12, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SuperValkyrie, 12, {1.5, 10.5}},
  };
  BattleState battle(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
  battle.advance_ticks(100);
  const auto source_damage_hits = std::count_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Damaged && std::abs(event.value - 445.5) < 1e-9;
  });
  COCSIM_REQUIRE(source_damage_hits >= 1);
  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot) && restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-super-valkyrie-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(100);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
