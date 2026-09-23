#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

int main() {
  Scenario scenario;
  scenario.width = 50;
  scenario.height = 50;
  scenario.duration_ms = 5'000;
  scenario.defenders = {
    {Kind::AirDefense, 16, {10.5, 10.5}},
    {Kind::AirDefense, 16, {40.5, 40.5}},
  };
  scenario.army = {{Kind::Dragon, 13, 2}};
  BattleState battle(GameData::v0(), scenario);
  const std::vector<Command> commands{
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Dragon, 13, {1.5, 10.5}},
    {CommandType::Deploy, 3 * kTickMs, 0, 0, Kind::Dragon, 13, {45.5, 40.5}},
  };

  // The first Dragon is only in reach of the upper-left defense. The lower
  // defense must remain uncommitted, rather than retaining this distant target.
  COCSIM_REQUIRE(battle.submit(commands[0]));
  battle.advance_ticks(2);
  COCSIM_REQUIRE(std::none_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 2;
  }));

  // When a second Dragon enters the lower-right Air Defense range, it must be
  // acquired immediately even though combat began elsewhere on the board.
  COCSIM_REQUIRE(battle.submit(commands[1]));
  battle.advance_ticks(2);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 2 && event.target == 4;
  }));

  const auto snapshot = battle.snapshot();
  std::string error;
  BattleState restored(GameData::v0(), scenario);
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  battle.advance_ticks(20);
  restored.advance_ticks(20);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());

  const std::string replay_path = "targeting-reacquisition-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(GameData::v0(), replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_to(24 * kTickMs);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
