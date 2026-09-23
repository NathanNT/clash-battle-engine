#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

bool has_beam_damage(const BattleState& battle, double amount) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.actor == 2
      && std::abs(event.value - amount) < 1e-9;
  });
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* dragon = data.find(Kind::InfernoDragon, 12);
  COCSIM_REQUIRE(dragon && dragon->hp == 2700 && dragon->cooldown == 128 && dragon->flying);
  // Damage remains the source DPS multiplied by the source 0.128-second
  // cadence. Scheduling is separately rounded up to the next 16-ms tick.
  COCSIM_REQUIRE(std::abs(dragon->inferno_initial_damage - 11.52) < 1e-9);
  COCSIM_REQUIRE(std::abs(dragon->inferno_second_damage - 23.296) < 1e-9);
  COCSIM_REQUIRE(std::abs(dragon->inferno_max_damage - 232.96) < 1e-9);
  COCSIM_REQUIRE(dragon->inferno_second_stage_at == 1696 && dragon->inferno_max_stage_at == 3200);

  // The secondary stage table becomes immutable GameData. The shared Core
  // quantizes its cadence to 130 ms and records a target lock in entity state;
  // no SDL/Python clock or render frame can alter the ramp.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::TownHall, 18, {5.5, 10.5}}};
  scenario.army = {{Kind::InfernoDragon, 12, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::InfernoDragon, 12, {1.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(220); // Crosses the serialized 1,700-ms lock boundary.
  COCSIM_REQUIRE(has_beam_damage(battle, 11.52));
  COCSIM_REQUIRE(has_beam_damage(battle, 23.296));
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.detail == "inferno beam";
  }));

  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-inferno-dragon-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario); submit_all(replay, replay_commands);
  replay.advance_ticks(220);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(160); restored.advance_ticks(160); replay.advance_ticks(160);
  COCSIM_REQUIRE(has_beam_damage(battle, 232.96));
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
