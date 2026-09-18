#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
bool damaged(const BattleState& battle, EntityId target, double amount) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.actor == 4 && event.target == target
        && event.value == amount;
  });
}
void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}
} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* super = data.find(Kind::SuperArcher, 14);
  COCSIM_REQUIRE(super && super->hp == 725 && super->damage == 255 && super->dps == 170);
  COCSIM_REQUIRE(super->range == 6 && super->cooldown == 1500 && super->piercing_projectile_range == 12);

  // Acquisition stays at six tiles, while the immutable Sharp Shot ray passes
  // through every intersected defender out to its separately sourced 12-tile
  // endpoint. Their IDs prove deterministic ray-order ties independently of
  // entity-container order.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 5000;
  scenario.defenders = {
    {Kind::Cannon, 1, {5.5, 10.5}}, {Kind::Cannon, 1, {7.5, 10.5}},
    {Kind::Cannon, 1, {10.5, 10.5}},
  };
  scenario.army = {{Kind::SuperArcher, 14, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SuperArcher, 14, {1.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2); // launch at T+10; its fixed endpoint is snapshot state.
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Projectile && event.actor == 4
        && event.detail == "super archer sharp shot";
  }));
  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-18\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-super-archer-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario); submit_all(replay, replay_commands);

  battle.advance_ticks(1); restored.advance_ticks(1); replay.advance_ticks(3);
  COCSIM_REQUIRE(damaged(battle, 1, 255));
  COCSIM_REQUIRE(damaged(battle, 2, 255));
  COCSIM_REQUIRE(damaged(battle, 3, 255));
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  battle.advance_ticks(300); restored.advance_ticks(300); replay.advance_ticks(300);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
