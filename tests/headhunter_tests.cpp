#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

int main() {
  const auto data = GameData::v0();
  const auto* headhunter = data.find(Kind::Headhunter, 4);
  COCSIM_REQUIRE(headhunter && headhunter->hp == 500 && headhunter->damage == 81
                 && headhunter->dps == 135 && headhunter->cooldown == 608
                 && headhunter->range == 3 && headhunter->speed == 32
                 && headhunter->housing_space == 6
                 && headhunter->target_focus == TargetFocus::Heroes
                 && headhunter->target_type == TargetType::Ground
                 && headhunter->attributes.town_hall_required == 18
                 && headhunter->attributes.laboratory_required == 16);

  // Heroes are explicitly outside the current Home Village scope. The stored
  // Hero focus therefore has no first-pass candidate and uses the normal
  // deterministic fallback against buildings without inventing a proxy Hero.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 4000;
  scenario.defenders = {{Kind::Cannon, 1, {10.5, 10.5}}};
  scenario.army = {{Kind::Headhunter, 4, 1}};
  const std::vector<Command> commands = {
      {CommandType::Deploy, kTickMs, 0, 0, Kind::Headhunter, 4, {1.5, 10.5}}};
  BattleState battle(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
  battle.advance_ticks(600);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.target == 1 && event.value == 81;
  }));

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario); COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-headhunter-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(600); COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  std::remove(replay_path.c_str());
}
