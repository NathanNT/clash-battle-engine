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
  const auto* giant = data.find(Kind::Giant, 14);
  COCSIM_REQUIRE(giant && giant->hp == 3000 && giant->dps == 114 && giant->damage == 228
                 && giant->cooldown == 2000 && giant->range == 1
                 && giant->movement_tiles_per_second == 1.5 && giant->housing_space == 5
                 && giant->target_focus == TargetFocus::Defenses && giant->target_type == TargetType::Ground);

  // The build-pinned contract covers the Defense first, then nearest-building
  // fallback after no Defense remains. Route shape and target-tie fidelity are
  // deliberately outside this compact shared-Core regression.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 40000;
  scenario.defenders = {{Kind::GoldMine, 1, {9.5, 4.5}}, {Kind::Cannon, 1, {10.5, 10.5}}};
  scenario.army = {{Kind::Giant, 14, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Giant, 14, {7.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2000);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 3 && event.target == 2 && event.value == 228;
  }));
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 3 && event.target == 1 && event.value == 228;
  }));

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-giant-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(2000);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
