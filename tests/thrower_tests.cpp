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
  const int expected_hp[] = {2200, 2350, 2600, 2800};
  const int expected_damage[] = {475, 525, 575, 600};
  for (int level = 1; level <= 4; ++level) {
    const auto* thrower = data.find(Kind::Thrower, level);
    COCSIM_REQUIRE(thrower && thrower->hp == expected_hp[level - 1]
                   && thrower->damage == expected_damage[level - 1]
                   && thrower->dps == expected_damage[level - 1] / 2.5
                   && thrower->range == 6 && thrower->cooldown == 2496
                   && thrower->housing_space == 16 && !thrower->flying
                   && thrower->target_type == TargetType::Both && thrower->deployable);
  }
  const auto* thrower4 = data.find(Kind::Thrower, 4);
  COCSIM_REQUIRE(thrower4->attributes.town_hall_required == 18
                 && thrower4->attributes.laboratory_required == 16
                 && thrower4->speed == 18
                 && thrower4->movement_tiles_per_second == 2.25);

  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 5000;
  scenario.defenders = {{Kind::GoldMine, 1, {12.5, 10.5}}};
  scenario.army = {{Kind::Thrower, 4, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Thrower, 4, {6.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Projectile && event.actor == 2
        && event.value == 600 && event.detail == "logical projectile";
  }));

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  battle.advance_ticks(300); restored.advance_ticks(300);
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-thrower-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands); replay.advance_ticks(302);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
