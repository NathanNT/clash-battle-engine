#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

using namespace cocsim;

namespace {

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

int live_kind(const BattleState& battle, Kind kind) {
  const auto entities = battle.observe();
  return static_cast<int>(std::count_if(entities.begin(), entities.end(), [kind](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == kind;
  }));
}

bool has_event(const BattleState& battle, EventType type, std::string_view detail) {
  return std::any_of(battle.events().begin(), battle.events().end(), [=](const Event& event) {
    return event.type == type && event.detail == detail;
  });
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* druid = data.find(Kind::Druid, 6);
  const auto* bear = data.find(Kind::DruidBear, 6);
  COCSIM_REQUIRE(druid && druid->hp == 1850 && druid->healing_per_second == 105
                 && druid->cooldown == 1008 && druid->range == 5
                 && druid->movement_tiles_per_second == 3 && druid->heals
                 && druid->jumps_walls && druid->druid_transform_after == 30000
                 && druid->druid_transforms_on_death && druid->housing_space == 16);
  COCSIM_REQUIRE(bear && bear->hp == 2750 && bear->damage == 225 && bear->cooldown == 1008
                 && bear->range == .2 && bear->movement_tiles_per_second == 2.5
                 && bear->target_focus == TargetFocus::Defenses && !bear->deployable);

  // The deadline is captured at deployment and remains in snapshot state.
  // A far defender keeps the battle alive while the human form has no injured
  // ally to heal. The Bear appears on the regular deterministic spawn queue.
  Scenario timed;
  timed.width = 50; timed.height = 20; timed.duration_ms = 40000;
  timed.defenders = {{Kind::Cannon, 1, {45.5, 10.5}}};
  timed.army = {{Kind::Druid, 6, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Druid, 6, {1.5, 10.5}},
  };
  BattleState battle(data, timed);
  submit_all(battle, commands);
  battle.advance_ticks(1812);
  COCSIM_REQUIRE(live_kind(battle, Kind::Druid) == 1 && live_kind(battle, Kind::DruidBear) == 0);
  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, timed);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  for (int tick = 0; tick != 200 && live_kind(battle, Kind::DruidBear) == 0; ++tick) {
    battle.advance_ticks(1);
    restored.advance_ticks(1);
    COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  }
  COCSIM_REQUIRE(live_kind(battle, Kind::Druid) == 0 && live_kind(battle, Kind::DruidBear) == 1);
  COCSIM_REQUIRE(has_event(battle, EventType::Deployed, "druid_bear spawned"));
  while (battle.now() < 31000) {
    battle.advance_ticks(1);
    restored.advance_ticks(1);
    COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  }

  // Human-form defeat turns into a full Bear. This is not reported as a
  // permanent Druid destruction and therefore does not end the attacker.
  Scenario defeated;
  defeated.width = 24; defeated.height = 20; defeated.duration_ms = 20000;
  defeated.defenders = {{Kind::Cannon, 21, {14.5, 10.5}}};
  defeated.army = {{Kind::Druid, 1, 1}};
  BattleState defeated_battle(data, defeated);
  COCSIM_REQUIRE(defeated_battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Druid, 1, {10.5, 10.5}}));
  for (int tick = 0; tick != 2000 && live_kind(defeated_battle, Kind::DruidBear) == 0; ++tick)
    defeated_battle.advance_ticks(1);
  COCSIM_REQUIRE(live_kind(defeated_battle, Kind::DruidBear) == 1);
  COCSIM_REQUIRE(has_event(defeated_battle, EventType::TargetChanged, "druid transforms on human defeat"));
  COCSIM_REQUIRE(!has_event(defeated_battle, EventType::Destroyed, "druid"));

  // A Spring Trap whose capacity can eject the Druid is the documented
  // exception. The human form is permanently removed and no Bear is queued.
  Scenario spring;
  spring.width = 20; spring.height = 20; spring.duration_ms = 1000;
  spring.defenders = {{Kind::SpringTrap, 13, {5.5, 10.5}}, {Kind::Cannon, 1, {15.5, 10.5}}};
  spring.army = {{Kind::Druid, 1, 1}};
  BattleState spring_battle(data, spring);
  COCSIM_REQUIRE(spring_battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Druid, 1, {5.5, 10.5}}));
  spring_battle.advance_ticks(3);
  COCSIM_REQUIRE(live_kind(spring_battle, Kind::Druid) == 0 && live_kind(spring_battle, Kind::DruidBear) == 0);
  COCSIM_REQUIRE(has_event(spring_battle, EventType::Destroyed, "druid"));

  const std::string replay_path = "replay-druid-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, timed, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_to(battle.now());
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  std::remove(replay_path.c_str());
}
