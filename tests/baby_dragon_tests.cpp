#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

bool attack_with_value(const BattleState& battle, double value) {
  return std::any_of(battle.events().begin(), battle.events().end(), [=](const Event& event) {
    return event.type == EventType::Attack && event.value == value;
  });
}

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* baby = data.find(Kind::BabyDragon, 12);
  COCSIM_REQUIRE(baby && baby->hp == 2350 && baby->damage == 185
                 && baby->dps == 185 && baby->cooldown == 1008
                 && baby->flying && baby->target_type == TargetType::Both
                 && baby->isolation_radius == 4.5
                 && baby->rage_damage_multiplier == 2
                 && baby->rage_attack_speed_multiplier == 1.5
                 && baby->deployable);

  Scenario isolated;
  isolated.width = 20; isolated.height = 20; isolated.duration_ms = 4000;
  isolated.defenders = {{Kind::GoldMine, 1, {10.5, 10.5}}};
  isolated.army = {{Kind::BabyDragon, 1, 1}};
  const std::vector<Command> isolated_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::BabyDragon, 1, {7.0, 10.5}},
  };
  BattleState isolated_battle(data, isolated);
  submit_all(isolated_battle, isolated_commands);
  isolated_battle.advance_ticks(3);
  COCSIM_REQUIRE(attack_with_value(isolated_battle, 150));

  // Only a second allied air unit inside the sourced 4.5-tile radius
  // suppresses the adaptation.  A nearby ground troop deliberately does not.
  Scenario nearby_air = isolated;
  nearby_air.army = {{Kind::BabyDragon, 1, 2}};
  const std::vector<Command> air_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::BabyDragon, 1, {7.0, 10.5}},
    {CommandType::Deploy, kTickMs, 0, 1, Kind::BabyDragon, 1, {7.0, 11.5}},
  };
  BattleState air_battle(data, nearby_air);
  submit_all(air_battle, air_commands);
  air_battle.advance_ticks(3);
  COCSIM_REQUIRE(attack_with_value(air_battle, 75));

  Scenario nearby_ground = isolated;
  nearby_ground.army = {{Kind::BabyDragon, 1, 1}, {Kind::Barbarian, 1, 1}};
  const std::vector<Command> ground_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::BabyDragon, 1, {7.0, 10.5}},
    {CommandType::Deploy, kTickMs, 0, 1, Kind::Barbarian, 1, {7.0, 11.5}},
  };
  BattleState ground_battle(data, nearby_ground);
  submit_all(ground_battle, ground_commands);
  ground_battle.advance_ticks(3);
  COCSIM_REQUIRE(attack_with_value(ground_battle, 150));

  const auto snapshot = air_battle.snapshot();
  BattleState restored(data, nearby_air);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == air_battle.state_hash());
  for (int tick = 0; tick != 100; ++tick) {
    air_battle.advance_ticks(1);
    restored.advance_ticks(1);
    COCSIM_REQUIRE(restored.state_hash() == air_battle.state_hash());
  }

  const std::string replay_path = "replay-baby-dragon-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, nearby_air, air_commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(103);
  COCSIM_REQUIRE(replay.state_hash() == air_battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
