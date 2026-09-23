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

int spawned_golemites(const BattleState& battle) {
  return static_cast<int>(std::count_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail == "golemite spawned";
  }));
}
} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* golem = data.find(Kind::Golem, 15);
  const auto* golemite = data.find(Kind::Golemite, 15);
  COCSIM_REQUIRE(golem && golem->hp == 10600 && golem->dps == 120 && golem->damage == 288
                 && golem->death_damage == 1050 && golem->death_splash_radius == 1.5
                 && golem->cooldown == 2400 && golem->range == 1
                 && golem->movement_tiles_per_second == 1.5 && golem->housing_space == 30
                 && golem->target_focus == TargetFocus::Defenses && golem->target_type == TargetType::Ground);
  COCSIM_REQUIRE(golemite && golemite->hp == 2040 && golemite->dps == 22 && golemite->damage == 66
                 && golemite->death_damage == 210 && golemite->death_splash_radius == 1.2
                 && golemite->parent_spawn_count == 4 && golemite->cooldown == 3008
                 && golemite->range == .5 && golemite->movement_tiles_per_second == 1.5
                 && golemite->housing_space == 6 && !golemite->deployable
                 && golemite->target_focus == TargetFocus::Defenses && golemite->target_type == TargetType::Ground);

  // The documented level-15 four-child split is queued by Core on parent death.
  // Public sources do not provide child coordinates or a split delay, so this
  // regression only locks the shared deterministic queue, not visual geometry.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 40000;
  scenario.defenders = {{Kind::Golem, 15, {10, 10}}};
  scenario.army = {{Kind::DragonRider, 6, 1}, {Kind::Golemite, 15, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::DragonRider, 6, {7, 10}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  int elapsed_ticks = 0;
  for (; elapsed_ticks != 3000 && spawned_golemites(battle) != 4; ++elapsed_ticks) battle.advance_ticks(1);
  COCSIM_REQUIRE(spawned_golemites(battle) == 4);

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-golem-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(elapsed_ticks);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
