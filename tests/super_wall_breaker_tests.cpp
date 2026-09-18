#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

bool has_damage(const BattleState& battle, EntityId actor, EntityId target, double value) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.actor == actor && event.target == target
        && std::abs(event.value - value) < 1e-9;
  });
}

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* super = data.find(Kind::SuperWallBreaker, 7);
  COCSIM_REQUIRE(super && super->hp == 350 && super->damage == 78 && super->death_damage == 175);
  COCSIM_REQUIRE(super->target_focus == TargetFocus::Walls);
  COCSIM_REQUIRE(super->wall_damage_multiplier == 40);
  COCSIM_REQUIRE(super->splash_radius == 2);
  COCSIM_REQUIRE(super->death_splash_radius == 2);
  COCSIM_REQUIRE(super->self_destruct_on_attack);
  COCSIM_REQUIRE(super->speed == 28 && super->movement_tiles_per_second == 3.5);

  // Reaching a wall follows the existing source-derived Wall Breaker attack
  // path: the level-7 base 78 impact is multiplied by 40 against the wall,
  // then the Super Wall Breaker is consumed without a duplicate death blast.
  Scenario reach_wall;
  reach_wall.width = 24;
  reach_wall.height = 20;
  reach_wall.duration_ms = 5000;
  reach_wall.defenders = {{Kind::Wall, 19, {12.5, 10.5}}};
  reach_wall.army = {{Kind::SuperWallBreaker, 7, 1}};
  const std::vector<Command> reach_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SuperWallBreaker, 7, {6.5, 10.5}},
  };
  BattleState reached(data, reach_wall);
  submit_all(reached, reach_commands);
  reached.advance_ticks(30);
  const auto snapshot = reached.snapshot();
  BattleState restored(data, reach_wall);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == reached.state_hash());

  const std::string replay_path = "replay-super-wall-breaker-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, reach_wall, reach_commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(30);
  COCSIM_REQUIRE(replay.state_hash() == reached.state_hash());

  reached.advance_ticks(200);
  restored.advance_ticks(200);
  replay.advance_ticks(200);
  COCSIM_REQUIRE(reached.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(reached.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(has_damage(reached, 2, 1, 3120));
  COCSIM_REQUIRE(std::any_of(reached.events().begin(), reached.events().end(), [](const Event& event) {
    return event.type == EventType::Destroyed && event.actor == 2 && event.target == 2
        && event.detail == "super_wall_breaker";
  }));

  // Supercell's distinct guarantee also applies if the troop is destroyed
  // before arrival.  Monolith kills it while it approaches the wall; the
  // inherited two-tile death blast applies its imported 175 damage times the
  // sourced x40 wall multiplier at the same deterministic tick.
  Scenario destroyed_first;
  destroyed_first.width = 24;
  destroyed_first.height = 20;
  destroyed_first.duration_ms = 1000;
  destroyed_first.defenders = {
    {Kind::Monolith, 1, {15.0, 10.5}},
    {Kind::Monolith, 1, {15.5, 10.5}},
    {Kind::Wall, 19, {8.5, 10.5}},
  };
  destroyed_first.army = {{Kind::SuperWallBreaker, 7, 1}};
  BattleState destroyed(data, destroyed_first);
  COCSIM_REQUIRE(destroyed.submit({CommandType::Deploy, kTickMs, 0, 0,
                                    Kind::SuperWallBreaker, 7, {6.5, 10.5}}));
  destroyed.advance_ticks(5);
  COCSIM_REQUIRE(has_damage(destroyed, 4, 3, 7000));
  COCSIM_REQUIRE(std::any_of(destroyed.events().begin(), destroyed.events().end(), [](const Event& event) {
    return event.type == EventType::Destroyed && event.actor == 2 && event.target == 4
        && event.detail == "super_wall_breaker";
  }));
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
