#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace cocsim;

int main() {
  COCSIM_REQUIRE(kTickMs == 16);
  const auto data = GameData::v0();
  // These source durations were 1000 and 750 ms on the 10 ms grid.
  COCSIM_REQUIRE(data.find(Kind::Barbarian, 13)->cooldown == 1008);
  COCSIM_REQUIRE(data.find(Kind::ArcherQueen, 101)->cooldown == 752);

  Scenario scenario;
  scenario.width = 20;
  scenario.height = 20;
  scenario.seed = 1732050807;
  scenario.duration_ms = 8000;
  scenario.defenders = {{Kind::Barbarian, 13, {10.5, 10.5}},
                        {Kind::Cannon, 1, {13.5, 10.5}}};
  scenario.army = {{Kind::Barbarian, 13, 1}};
  const Command deploy{CommandType::Deploy, kTickMs, 0, 0,
                       Kind::Barbarian, 13, {1.5, 10.5}};
  BattleState battle(data, scenario);
  std::string error;
  Command old_grid = deploy;
  old_grid.requested_ms = 10;
  COCSIM_REQUIRE(!battle.submit(old_grid, &error));
  COCSIM_REQUIRE(battle.submit(deploy));
  bool rejected_unaligned_advance = false;
  try { battle.advance_to(1000); }
  catch (const std::invalid_argument&) { rejected_unaligned_advance = true; }
  COCSIM_REQUIRE(rejected_unaligned_advance);

  battle.advance_to(528);
  const auto first_event_time = [&](EventType type, const char* detail) {
    const auto it = std::find_if(battle.events().begin(), battle.events().end(), [&](const Event& event) {
      return event.type == type && event.actor == 2 && event.detail == detail;
    });
    COCSIM_REQUIRE(it != battle.events().end());
    return it->time_ms;
  };
  // Recorded 10 ms baseline: first attack at 480 ms, logical impact at
  // 490 ms. The same encounter now attacks at 496 and impacts at 512 ms.
  constexpr Milliseconds old_attack_ms = 480;
  constexpr Milliseconds old_impact_ms = 490;
  const auto attack_ms = first_event_time(EventType::Attack, "attack");
  const auto impact_ms = first_event_time(EventType::Impact, "projectile impact");
  COCSIM_REQUIRE(attack_ms == 496 && attack_ms - old_attack_ms == 16);
  COCSIM_REQUIRE(impact_ms == 512 && impact_ms - old_impact_ms == 22);

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "tick-migration-replay-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, scenario, battle.commands(), error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_to(528);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  battle.advance_to(1008);
  restored.advance_to(1008);
  replay.advance_to(1008);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  const auto result = battle.result();
  // Both grids leave this 1000 ms checkpoint active with no destruction and
  // one deployed troop. The requested 1000 ms is represented at 1008 ms.
  COCSIM_REQUIRE(!result.finished && result.time_ms == 1008);
  constexpr double old_destruction = 0;
  constexpr int old_stars = 0;
  constexpr int old_troops_remaining = 1;
  COCSIM_REQUIRE(result.destruction == old_destruction && result.stars == old_stars);
  COCSIM_REQUIRE(result.troops_remaining == old_troops_remaining);
  std::cout << "tick migration regression passed\n";
}
