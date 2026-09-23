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

bool has_ice_golem_freeze(const BattleState& battle) {
  const auto effects = battle.observe_spell_effects();
  return std::any_of(effects.begin(), effects.end(), [](const SpellEffectView& effect) {
    return effect.kind == SpellKind::Freeze && std::abs(effect.radius - 7.5) < 1e-9;
  });
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* l1 = data.find(Kind::IceGolem, 1);
  const auto* l5 = data.find(Kind::IceGolem, 5);
  const auto* l7 = data.find(Kind::IceGolem, 7);
  const auto* l9 = data.find(Kind::IceGolem, 9);
  COCSIM_REQUIRE(l1 && l1->death_freeze_radius == 7.5 && l1->death_freeze_duration == 4000);
  COCSIM_REQUIRE(l5 && l5->death_freeze_duration == 7008);
  COCSIM_REQUIRE(l7 && l7->death_freeze_duration == 8000);
  COCSIM_REQUIRE(l9 && l9->death_freeze_duration == 9008);

  // Two high-level cannons destroy the attacking Ice Golem. The surviving
  // Barbarian reserve keeps the battle active, making the death-triggered
  // shared Freeze effect observable rather than relying on a GUI animation.
  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 30000;
  scenario.defenders = {
    {Kind::Cannon, 21, {11.5, 10.5}},
    {Kind::Cannon, 21, {11.5, 14.5}},
  };
  scenario.army = {{Kind::IceGolem, 1, 1}, {Kind::Barbarian, 1, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::IceGolem, 1, {6.5, 10.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  for (int tick = 0; tick != 2000 && !has_ice_golem_freeze(battle); ++tick) battle.advance_ticks(1);
  COCSIM_REQUIRE(has_ice_golem_freeze(battle));
  COCSIM_REQUIRE(!battle.result().finished);
  const auto freeze_event = std::find_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Impact && event.detail == "ice golem death freeze" && event.value == 4000;
  });
  COCSIM_REQUIRE(freeze_event != battle.events().end());

  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-ice-golem-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  for (int tick = 0; tick != 2000 && !has_ice_golem_freeze(replay); ++tick) replay.advance_ticks(1);
  COCSIM_REQUIRE(has_ice_golem_freeze(replay));
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  // Freeze persists through the snapshot boundary and prevents the two
  // defender cannons from making a normal attack during the next logical tick.
  const auto attacks_before = std::count_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && (event.actor == 1 || event.actor == 2);
  });
  battle.advance_ticks(1);
  restored.advance_ticks(1);
  replay.advance_ticks(1);
  const auto attacks_after = std::count_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && (event.actor == 1 || event.actor == 2);
  });
  COCSIM_REQUIRE(attacks_after == attacks_before);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
