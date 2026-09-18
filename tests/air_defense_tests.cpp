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

const EntityView& find_view(const std::vector<EntityView>& views, EntityId id) {
  const auto it = std::find_if(views.begin(), views.end(), [id](const EntityView& view) {
    return view.id == id;
  });
  COCSIM_REQUIRE(it != views.end());
  return *it;
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* level_one = data.find(Kind::AirDefense, 1);
  const auto* level_sixteen = data.find(Kind::AirDefense, 16);
  COCSIM_REQUIRE(level_one && level_one->footprint_width == 3 && level_one->footprint_height == 3);
  COCSIM_REQUIRE(level_one->target_type == TargetType::Air && level_one->range == 10);
  COCSIM_REQUIRE(level_sixteen && level_sixteen->hp == 2000 && level_sixteen->damage == 700);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 5000;
  scenario.defenders = {{Kind::AirDefense, 1, {12.5, 10.5}}};
  scenario.army = {{Kind::Dragon, 1, 2}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Dragon, 1, {6.5, 10.5}},
    {CommandType::Deploy, kTickMs, 0, 1, Kind::Dragon, 1, {6.5, 10.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2);
  // Equal-position valid air targets must use the stable entity-id tie break.
  COCSIM_REQUIRE(find_view(battle.observe(), 1).target == 2);
  battle.advance_ticks(1);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Projectile && event.actor == 1 && event.target == 2
        && event.detail == "logical projectile";
  }));

  // AIR_DEFENSE_SNAPSHOT_REPLAY: the selected target and in-flight logical
  // projectile are future-influencing values, so restore and replay must agree
  // before and after the first scheduled impact.
  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-air-defense-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(3);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(100);
  restored.advance_ticks(100);
  replay.advance_ticks(100);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
