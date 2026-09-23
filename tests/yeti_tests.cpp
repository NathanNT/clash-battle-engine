#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

int spawned_yetimites(const BattleState& battle) {
  return static_cast<int>(std::count_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail == "yetimite spawned";
  }));
}

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* yeti = data.find(Kind::Yeti, 8);
  const auto* yetimite = data.find(Kind::Yetimite, 8);
  COCSIM_REQUIRE(yeti && yeti->hp == 4650 && yeti->damage == 380
                 && yeti->cooldown == 1008 && yeti->range == .8
                 && yeti->movement_tiles_per_second == 1.5
                 && yeti->spawned_units == 14 && yeti->damage_spawn_threshold == 600);
  COCSIM_REQUIRE(yetimite && yetimite->hp == 590 && yetimite->damage == 92
                 && yetimite->cooldown == kTickMs && yetimite->target_focus == TargetFocus::Defenses
                 && yetimite->target_type == TargetType::Both && yetimite->splash
                 && yetimite->splash_radius == .8 && yetimite->movement_tiles_per_second == 3
                 && yetimite->defense_damage_multiplier == 4
                 && yetimite->resource_damage_multiplier == .5 && !yetimite->deployable);

  // The Monolith's large hits cross several 600-damage boundaries. All child
  // requests use the stable spawn queue. The Wall blocks the parent but not
  // the sourced wall-jumping child.
  Scenario scenario;
  scenario.width = 24; scenario.height = 20; scenario.duration_ms = 20000;
  scenario.defenders = {
    {Kind::Wall, 17, {8.5, 10.5}},
    {Kind::Monolith, 3, {12.5, 10.5}},
    {Kind::Monolith, 3, {20.5, 10.5}},
  };
  scenario.army = {{Kind::Yeti, 8, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Yeti, 8, {5.5, 10.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  for (int tick = 0; tick != 1000 && spawned_yetimites(battle) != 14; ++tick) battle.advance_ticks(1);
  COCSIM_REQUIRE(spawned_yetimites(battle) == 14);

  for (int tick = 0; tick != 1000; ++tick) battle.advance_ticks(1);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor >= 4 && event.value == 368;
  }));
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Destroyed && event.actor == event.target && event.detail == "yetimite";
  }));

  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-yeti-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  for (int tick = 0; tick != 2000; ++tick) replay.advance_ticks(1);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  battle.advance_ticks(20); restored.advance_ticks(20); replay.advance_ticks(20);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  std::remove(replay_path.c_str());
}
