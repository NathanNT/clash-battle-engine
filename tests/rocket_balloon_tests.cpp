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

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* rocket = data.find(Kind::RocketBalloon, 12);
  COCSIM_REQUIRE(rocket && rocket->hp == 1240 && rocket->damage == 912
                 && rocket->cooldown == 3000 && rocket->flying);
  COCSIM_REQUIRE(rocket->target_focus == TargetFocus::DefensesOnly);
  COCSIM_REQUIRE(rocket->deployment_rage_duration == 4000);
  COCSIM_REQUIRE(std::abs(rocket->deployment_rage_damage_multiplier - 1.0) < 1e-9);
  COCSIM_REQUIRE(std::abs(rocket->deployment_rage_movement_speed_multiplier - (64.0 / 12.0)) < 1e-9);

  // The defender is deliberately beyond the four-second boosted travel path.
  // The first 10-ms movement step is (12 + 52) / 8 * 10 / 1000 tiles, derived
  // from the secondary level-5-Haste equivalence; no Viewer clock participates.
  Scenario scenario;
  scenario.width = 80;
  scenario.height = 20;
  scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::AirDefense, 13, {70.5, 10.5}}};
  scenario.army = {{Kind::RocketBalloon, 12, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::RocketBalloon, 12, {1.5, 10.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2);
  const auto early = battle.observe();
  const auto first = std::find_if(early.begin(), early.end(), [](const EntityView& entity) {
    return entity.kind == Kind::RocketBalloon && entity.side == Side::Attacker;
  });
  COCSIM_REQUIRE(first != early.end() && first->deployment_rage_active);
  COCSIM_REQUIRE(std::abs(first->position.x - 1.58) < 1e-9);

  battle.advance_ticks(198); // T+2000 ms, still within the serialized window.
  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-18\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-rocket-balloon-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(200);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(201); // T+4010 ms: strict deadline has expired.
  restored.advance_ticks(201);
  replay.advance_ticks(201);
  const auto late = battle.observe();
  const auto expired = std::find_if(late.begin(), late.end(), [](const EntityView& entity) {
    return entity.kind == Kind::RocketBalloon && entity.side == Side::Attacker;
  });
  COCSIM_REQUIRE(expired != late.end() && !expired->deployment_rage_active);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
