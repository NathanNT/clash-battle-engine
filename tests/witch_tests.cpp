#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

int main() {
  const auto data = GameData::v0();
  const auto* witch = data.find(Kind::Witch, 8);
  COCSIM_REQUIRE(witch && witch->hp == 650 && witch->damage == 182);
  COCSIM_REQUIRE(witch->cooldown == 704 && witch->range == 4
                 && witch->housing_space == 12 && witch->target_type == TargetType::Both
                 && witch->movement_tiles_per_second == 1.5 && witch->deployable);

  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 4000;
  scenario.defenders = {{Kind::Cannon, 1, {7.5, 10.5}}};
  scenario.army = {{Kind::Witch, 8, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Witch, 8, {1.5, 10.5}},
  };

  BattleState battle(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
  battle.advance_ticks(100);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Damaged && event.value == 182;
  }));
  // No public current source supplies a reproducible Skeleton stat table or
  // summon lifecycle. This parent-baseline test makes the deliberately absent
  // child behavior explicit rather than silently creating a proxy troop.
  COCSIM_REQUIRE(std::none_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail.find("skeleton") != std::string::npos;
  }));

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-witch-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(100);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  battle.advance_ticks(20); restored.advance_ticks(20); replay.advance_ticks(20);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
