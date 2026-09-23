#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace { void submit_all(BattleState& battle, const std::vector<Command>& commands) { for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command)); } }

int main() {
  const auto data = GameData::v0();
  const auto* archer = data.find(Kind::Archer, 14);
  COCSIM_REQUIRE(archer && archer->hp == 76 && archer->dps == 46 && archer->damage == 46
                 && archer->cooldown == 1008 && archer->range == 3.5
                 && archer->movement_tiles_per_second == 3 && archer->housing_space == 1
                 && archer->ranged && archer->target_focus == TargetFocus::Any && archer->target_type == TargetType::Both);
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 8000;
  scenario.defenders = {{Kind::Wall, 1, {9.5, 10.5}}, {Kind::GoldMine, 1, {10.5, 10.5}}};
  scenario.army = {{Kind::Archer, 14, 1}};
  const std::vector<Command> commands = {{CommandType::Deploy, kTickMs, 0, 0, Kind::Archer, 14, {6.5, 10.5}}};
  BattleState battle(data, scenario); submit_all(battle, commands); battle.advance_ticks(500);
  // The source documents reach past Walls. This locks only shared range-based
  // combat, not unsourced projectile or Wall-boundary geometry.
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) { return event.type == EventType::Attack && event.actor == 3 && event.target == 2 && event.value == 46; }));
  const auto snapshot = battle.snapshot(); BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot)); COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-archer-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error)); Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error)); BattleState replay(data, replay_scenario); submit_all(replay, replay_commands); replay.advance_ticks(500);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash()); COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
