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
}

int main() {
  const auto data = GameData::v0();
  const auto* healer = data.find(Kind::Healer, 11);
  COCSIM_REQUIRE(healer && healer->hp == 2100 && healer->healing_per_second == 82
      && healer->cooldown == 704 && healer->range == 4.5
      && healer->housing_space == 14 && healer->flying && healer->heals
      && healer->target_focus == TargetFocus::FriendlyTroops
      && healer->target_type == TargetType::Ground);

  Scenario ground_heal;
  ground_heal.width = 20;
  ground_heal.height = 20;
  ground_heal.duration_ms = 5000;
  ground_heal.defenders = {{Kind::Cannon, 1, {10.5, 10.5}}};
  ground_heal.army = {{Kind::Barbarian, 1, 1}, {Kind::Healer, 11, 1}};
  const std::vector<Command> ground_commands = {
      {CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {7.5, 10.5}},
      {CommandType::Deploy, kTickMs, 0, 1, Kind::Healer, 11, {7.5, 11.5}},
  };
  BattleState ground(data, ground_heal);
  submit_all(ground, ground_commands);
  ground.advance_ticks(3);
  const auto launch = std::find_if(ground.events().begin(), ground.events().end(), [](const Event& event) {
    return event.type == EventType::Projectile && event.actor == 3 && event.target == 2
        && event.detail == "logical healing projectile";
  });
  COCSIM_REQUIRE(launch != ground.events().end() && launch->projectile_id != 0);
  const auto snapshot = ground.snapshot();
  BattleState restored(data, ground_heal);
  COCSIM_REQUIRE(restored.restore(snapshot));
  ground.advance_ticks(1);
  restored.advance_ticks(1);
  COCSIM_REQUIRE(restored.state_hash() == ground.state_hash());
  COCSIM_REQUIRE(std::any_of(ground.events().begin(), ground.events().end(), [](const Event& event) {
    return event.type == EventType::Healed && event.actor == 3 && event.target == 2
        && event.value > 0 && event.detail == "projectile heal";
  }));
  const std::string replay_path = "replay-healer-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, ground_heal, ground_commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(4);
  COCSIM_REQUIRE(replay.state_hash() == ground.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  // The source-backed Healer restriction is semantic, not a Viewer filter:
  // a wounded Balloon must never become a friendly healing target.
  Scenario air_target;
  air_target.width = 20;
  air_target.height = 20;
  air_target.duration_ms = 5000;
  air_target.defenders = {{Kind::AirDefense, 1, {10.5, 10.5}}};
  air_target.army = {{Kind::Balloon, 13, 1}, {Kind::Healer, 11, 1}};
  BattleState air(data, air_target);
  submit_all(air, {{CommandType::Deploy, kTickMs, 0, 0, Kind::Balloon, 13, {7.5, 10.5}},
                   {CommandType::Deploy, 2 * kTickMs, 0, 1, Kind::Healer, 11, {2.5, 10.5}}});
  air.advance_ticks(20);
  COCSIM_REQUIRE(std::any_of(air.events().begin(), air.events().end(), [](const Event& event) {
    return event.type == EventType::Damaged && event.actor == 1 && event.target == 2;
  }));
  COCSIM_REQUIRE(std::none_of(air.events().begin(), air.events().end(), [](const Event& event) {
    return event.actor == 3 && (event.type == EventType::Projectile || event.type == EventType::Healed);
  }));
}
