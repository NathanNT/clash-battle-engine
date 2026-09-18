#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
bool has_damage(const BattleState& battle, EntityId target, double value) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.actor == 2 && event.target == target
        && std::abs(event.value - value) < 1e-9;
  });
}
void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}
} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* super = data.find(Kind::SuperGiant, 14);
  COCSIM_REQUIRE(super && super->hp == 5600 && super->damage == 410 && super->dps == 205);
  COCSIM_REQUIRE(super->cooldown == 2000 && super->target_focus == TargetFocus::Defenses);
  COCSIM_REQUIRE(super->wall_damage_multiplier == 5 && super->speed == 12);

  // The Super Giant retains its sourced defense priority, but the shared path
  // selects an intervening Wall deterministically. Its first impact therefore
  // proves the five-times Wall damage without inventing a special movement path.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 8000;
  scenario.defenders = {{Kind::Wall, 1, {5.5, 10.5}}};
  scenario.army = {{Kind::SuperGiant, 14, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SuperGiant, 14, {1.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(160);
  COCSIM_REQUIRE(has_damage(battle, 1, 2050));
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Destroyed && event.actor == 2 && event.target == 1;
  }));

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-super-giant-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario); submit_all(replay, replay_commands);
  replay.advance_ticks(160);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  battle.advance_ticks(300); restored.advance_ticks(300); replay.advance_ticks(300);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  Scenario normal_target; normal_target.width = 20; normal_target.height = 20; normal_target.duration_ms = 5000;
  normal_target.defenders = {{Kind::Cannon, 1, {5.5, 10.5}}}; normal_target.army = {{Kind::SuperGiant, 14, 1}};
  BattleState normal(data, normal_target);
  COCSIM_REQUIRE(normal.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::SuperGiant, 14, {1.5, 10.5}}));
  normal.advance_ticks(160);
  COCSIM_REQUIRE(has_damage(normal, 1, 410));
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
