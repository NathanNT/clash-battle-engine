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

EntityView entity_view(const BattleState& battle, Kind kind, int level) {
  const auto views = battle.observe();
  const auto found = std::find_if(views.begin(), views.end(), [=](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == kind && entity.level == level;
  });
  COCSIM_REQUIRE(found != views.end());
  return *found;
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* l1 = data.find(Kind::ApprenticeWarden, 1);
  const auto* l4 = data.find(Kind::ApprenticeWarden, 4);
  COCSIM_REQUIRE(l1 && l1->life_aura_range == 7 && l1->life_aura_hp_increase_percent == 24);
  COCSIM_REQUIRE(l4 && l4->life_aura_range == 7 && l4->life_aura_hp_increase_percent == 30);

  // Both Wardens cover the Barbarian. The primary source says overlapping
  // Life Auras do not stack, so L4's 30 percent must win over L1's 24 percent.
  // The nearby Dragon proves that Life Aura applies only to ground troops.
  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::GoldMine, 1, {18.5, 10.5}}};
  scenario.army = {
    {Kind::ApprenticeWarden, 1, 1},
    {Kind::ApprenticeWarden, 4, 1},
    {Kind::Barbarian, 1, 1},
    {Kind::Dragon, 1, 1},
  };
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::ApprenticeWarden, 1, {3.5, 10.5}},
    {CommandType::Deploy, kTickMs, 0, 0, Kind::ApprenticeWarden, 4, {4.5, 10.5}},
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {6.5, 10.5}},
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Dragon, 1, {6.5, 11.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(2);
  const auto barbarian = entity_view(battle, Kind::Barbarian, 1);
  const auto dragon = entity_view(battle, Kind::Dragon, 1);
  const auto* barbarian_stats = data.find(Kind::Barbarian, 1);
  const auto* dragon_stats = data.find(Kind::Dragon, 1);
  COCSIM_REQUIRE(barbarian_stats && dragon_stats);
  COCSIM_REQUIRE(std::abs(barbarian.max_hp - barbarian_stats->hp * 1.3) < 1e-9);
  COCSIM_REQUIRE(std::abs(barbarian.hp - barbarian.max_hp) < 1e-9);
  COCSIM_REQUIRE(dragon.max_hp == dragon_stats->hp);

  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-apprentice-warden-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(2);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(10);
  restored.advance_ticks(10);
  replay.advance_ticks(10);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
