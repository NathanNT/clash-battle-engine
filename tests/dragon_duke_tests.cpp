#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
bool attack_with_value(const BattleState& battle, double value) {
  return std::any_of(battle.events().begin(), battle.events().end(), [=](const Event& event) {
    return event.type == EventType::Attack && event.value == value;
  });
}
bool damage_with_value(const BattleState& battle, double value) {
  return std::any_of(battle.events().begin(), battle.events().end(), [=](const Event& event) {
    return event.type == EventType::Damaged && event.value == value;
  });
}
}

int main() {
  const auto data = GameData::v0();
  std::string error;
  COCSIM_REQUIRE(!data.find(Kind::DragonDuke, 20));
  const auto* l21 = data.find(Kind::DragonDuke, 21);
  const auto* l25 = data.find(Kind::DragonDuke, 25);
  COCSIM_REQUIRE(l21 && l25);
  COCSIM_REQUIRE(l21->hp == 10600 && l21->damage == 460.8 && l21->cooldown == 1200);
  COCSIM_REQUIRE(l25->hp == 10900 && l25->damage == 480.0 && l25->dps == 400);
  COCSIM_REQUIRE(l25->flying && l25->target_type == TargetType::Both && l25->range == 1.25 && l25->speed == 20.0);
  COCSIM_REQUIRE(l25->isolation_radius == 6.0 && l25->rage_damage_multiplier == 2.0 && l25->rage_attack_speed_multiplier == 1.5 && l25->rage_trap_damage_multiplier == 0.8);

  Scenario isolated;
  isolated.width = 24; isolated.height = 20; isolated.duration_ms = 6000;
  isolated.defenders = {{Kind::Cannon, 1, {13.5, 10.5}}};
  isolated.army = {{Kind::DragonDuke, 25, 1}};
  BattleState first(data, isolated);
  COCSIM_REQUIRE(first.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::DragonDuke, 25, {10.5, 10.5}}));
  bool isolated_attack = false;
  for (int tick = 0; tick != 300; ++tick) {
    first.advance_ticks(1);
    isolated_attack = isolated_attack || attack_with_value(first, 960.0);
  }
  COCSIM_REQUIRE(isolated_attack);

  Scenario nearby_air = isolated;
  nearby_air.army = {{Kind::DragonDuke, 25, 1}, {Kind::Minion, 1, 1}};
  const std::vector<Command> paired_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::DragonDuke, 25, {10.5, 10.5}},
    {CommandType::Deploy, kTickMs, 0, 1, Kind::Minion, 1, {10.5, 11.5}},
  };
  BattleState paired(data, nearby_air);
  for (const auto& command : paired_commands) COCSIM_REQUIRE(paired.submit(command));
  bool paired_attack = false;
  for (int tick = 0; tick != 300; ++tick) {
    paired.advance_ticks(1);
    paired_attack = paired_attack || attack_with_value(paired, 480.0);
  }
  COCSIM_REQUIRE(paired_attack);

  // The post-July Royal Rampage contract retains 80 percent of trap damage
  // only while the Duke has no nearby friendly flying unit. Trap ownership is
  // preserved through the shared damage path, so this also covers serialised
  // trap projectiles rather than a Viewer-side health adjustment.
  const auto* air_bomb = data.find(Kind::AirBomb, 1);
  COCSIM_REQUIRE(air_bomb && air_bomb->damage > 0.0);
  Scenario trap_route;
  trap_route.width = 24; trap_route.height = 20; trap_route.duration_ms = 3000;
  trap_route.defenders = {{Kind::AirBomb, 1, {10.5, 10.5}}};
  trap_route.army = {{Kind::DragonDuke, 25, 1}};
  const std::vector<Command> trap_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::DragonDuke, 25, {8.5, 10.5}},
  };
  BattleState trap_isolated(data, trap_route);
  for (const auto& command : trap_commands) COCSIM_REQUIRE(trap_isolated.submit(command));
  const auto trap_snapshot = trap_isolated.snapshot();
  BattleState trap_restored(data, trap_route);
  COCSIM_REQUIRE(trap_restored.restore(trap_snapshot, &error));
  bool reduced_trap_damage = false;
  for (int tick = 0; tick != 20; ++tick) {
    trap_isolated.advance_ticks(1);
    trap_restored.advance_ticks(1);
    reduced_trap_damage = reduced_trap_damage || damage_with_value(trap_isolated, air_bomb->damage * 0.8);
  }
  COCSIM_REQUIRE(reduced_trap_damage);
  COCSIM_REQUIRE(trap_isolated.state_hash() == trap_restored.state_hash());
  const std::string trap_replay_path = "replay-dragon-duke-trap-test.json";
  COCSIM_REQUIRE(save_replay(trap_replay_path, trap_route, trap_commands, error));
  Scenario trap_replay_scenario;
  std::vector<Command> trap_replay_commands;
  COCSIM_REQUIRE(load_replay(trap_replay_path, trap_replay_scenario, trap_replay_commands, error));
  BattleState trap_replay(data, trap_replay_scenario);
  for (const auto& command : trap_replay_commands) COCSIM_REQUIRE(trap_replay.submit(command));
  trap_replay.advance_ticks(20);
  COCSIM_REQUIRE(trap_replay.state_hash() == trap_isolated.state_hash());
  COCSIM_REQUIRE(std::remove(trap_replay_path.c_str()) == 0);
  Scenario trap_paired = trap_route;
  trap_paired.army = {{Kind::DragonDuke, 25, 1}, {Kind::Minion, 1, 1}};
  BattleState trap_not_isolated(data, trap_paired);
  COCSIM_REQUIRE(trap_not_isolated.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::DragonDuke, 25, {8.5, 10.5}}));
  COCSIM_REQUIRE(trap_not_isolated.submit({CommandType::Deploy, kTickMs, 0, 1, Kind::Minion, 1, {4.5, 10.5}}));
  bool full_trap_damage = false;
  for (int tick = 0; tick != 20; ++tick) {
    trap_not_isolated.advance_ticks(1);
    full_trap_damage = full_trap_damage || damage_with_value(trap_not_isolated, air_bomb->damage);
  }
  COCSIM_REQUIRE(full_trap_damage);
  const auto snapshot = paired.snapshot();
  const auto expected_hash = paired.state_hash();
  BattleState restored(data, nearby_air);
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  COCSIM_REQUIRE(restored.state_hash() == expected_hash);
  const std::string replay_path = "replay-dragon-duke-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, nearby_air, paired_commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(300);
  COCSIM_REQUIRE(replay.state_hash() == expected_hash);
  paired.advance_ticks(100);
  restored.advance_ticks(100);
  replay.advance_ticks(100);
  COCSIM_REQUIRE(paired.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(paired.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  Scenario defended;
  defended.width = 24;
  defended.height = 20;
  defended.duration_ms = 3000;
  defended.defenders = {{Kind::DragonDuke, 25, {12.5, 10.5}}};
  defended.army = {{Kind::Barbarian, 1, 1}};
  BattleState ground_only(data, defended);
  COCSIM_REQUIRE(ground_only.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {8.5, 10.5}}));
  ground_only.advance_ticks(2);
  const auto ground_views = ground_only.observe();
  const auto ground_barbarian = std::find_if(ground_views.begin(), ground_views.end(), [](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == Kind::Barbarian;
  });
  COCSIM_REQUIRE(ground_barbarian != ground_views.end() && !ground_barbarian->target);

  defended.army = {{Kind::Archer, 1, 1}};
  BattleState air_targeting(data, defended);
  const std::vector<Command> defender_targeting_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Archer, 1, {8.5, 10.5}},
  };
  for (const auto& command : defender_targeting_commands) COCSIM_REQUIRE(air_targeting.submit(command));
  air_targeting.advance_ticks(2);
  const auto air_views = air_targeting.observe();
  const auto air_archer = std::find_if(air_views.begin(), air_views.end(), [](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == Kind::Archer;
  });
  COCSIM_REQUIRE(air_archer != air_views.end() && air_archer->target);
  const auto defender_targeting_snapshot = air_targeting.snapshot();
  BattleState defender_targeting_restored(data, defended);
  COCSIM_REQUIRE(defender_targeting_restored.restore(defender_targeting_snapshot, &error));
  air_targeting.advance_ticks(20);
  defender_targeting_restored.advance_ticks(20);
  COCSIM_REQUIRE(air_targeting.state_hash() == defender_targeting_restored.state_hash());
  const std::string defender_targeting_replay_path = "replay-dragon-duke-defender-targetability-test.json";
  COCSIM_REQUIRE(save_replay(defender_targeting_replay_path, defended, defender_targeting_commands, error));
  COCSIM_REQUIRE(load_replay(defender_targeting_replay_path, replay_scenario, replay_commands, error));
  BattleState defender_targeting_replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(defender_targeting_replay.submit(command));
  defender_targeting_replay.advance_ticks(22);
  COCSIM_REQUIRE(defender_targeting_replay.state_hash() == air_targeting.state_hash());
  COCSIM_REQUIRE(std::remove(defender_targeting_replay_path.c_str()) == 0);
  return 0;
}
