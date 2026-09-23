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

bool has_damage(const BattleState& battle, EntityId actor, double amount) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.actor == actor
           && std::abs(event.value - amount) < 1e-9;
  });
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* miner = data.find(Kind::SuperMiner, 12);
  COCSIM_REQUIRE(miner && miner->hp == 5000 && miner->cooldown == 256);
  COCSIM_REQUIRE(miner->burrows && miner->range == .6 && miner->death_damage == 2400
                 && miner->death_splash_radius == 2 && miner->death_damage_ground_only);
  COCSIM_REQUIRE(std::abs(miner->inferno_initial_damage - 66.25) < 1e-9);
  COCSIM_REQUIRE(std::abs(miner->inferno_second_damage - 127.5) < 1e-9);
  COCSIM_REQUIRE(std::abs(miner->inferno_max_damage - 185) < 1e-9);
  COCSIM_REQUIRE(miner->inferno_second_stage_at == 1504 && miner->inferno_max_stage_at == 3008);

  // The three source-table stages share the serialized lock timer used by the
  // existing ramp system, while the Super Miner remains a melee drill.
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::TownHall, 18, {5.5, 10.5}}};
  scenario.army = {{Kind::SuperMiner, 12, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SuperMiner, 12, {1.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(400);
  COCSIM_REQUIRE(has_damage(battle, 2, 66.25));
  COCSIM_REQUIRE(has_damage(battle, 2, 127.5));
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.detail == "super miner drill";
  }));

  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-super-miner-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(400);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(200);
  restored.advance_ticks(200);
  replay.advance_ticks(200);
  COCSIM_REQUIRE(has_damage(battle, 2, 185));
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());

  // Death damage shares the generic value-only explosion queue. Eight
  // level-12 Lightning casts deterministically destroy a defending Super
  // Miner at T+16ms, and the nearby ground Golem receives its 2,400 payload.
  Scenario death_scenario;
  death_scenario.width = 20; death_scenario.height = 20; death_scenario.duration_ms = 1000;
  death_scenario.defenders = {{Kind::SuperMiner, 12, {10.5, 10.5}}};
  death_scenario.army = {{Kind::Golem, 1, 1}};
  death_scenario.spells = {{SpellKind::Lightning, 12, 8}};
  BattleState death_battle(data, death_scenario);
  std::vector<Command> death_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Golem, 1, {8.5, 10.5}},
  };
  for (int index = 0; index != 8; ++index)
    death_commands.push_back({CommandType::CastSpell, kTickMs, 0, 0, Kind::Barbarian, 12,
                              {10.5, 10.5}, SpellKind::Lightning});
  submit_all(death_battle, death_commands);
  death_battle.advance_ticks(2);
  COCSIM_REQUIRE(has_damage(death_battle, 1, 2400));
  COCSIM_REQUIRE(std::any_of(death_battle.events().begin(), death_battle.events().end(), [](const Event& event) {
    return event.type == EventType::Impact && event.actor == 1 && event.detail == "death damage";
  }));
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
