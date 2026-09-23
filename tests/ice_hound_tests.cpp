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

int ice_pup_count(const BattleState& battle) {
  const auto entities = battle.observe();
  return static_cast<int>(std::count_if(entities.begin(), entities.end(), [](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == Kind::IcePup;
  }));
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* hound = data.find(Kind::IceHound, 8);
  const auto* pup = data.find(Kind::IcePup, 1);
  COCSIM_REQUIRE(!data.find(Kind::IceHound, 4));
  COCSIM_REQUIRE(hound && hound->hp == 11500 && hound->damage == 50
                 && hound->cooldown == 2000 && hound->range == .75
                 && hound->movement_tiles_per_second == 2.5
                 && hound->target_focus == TargetFocus::AirDefenses
                 && hound->spawned_units == 16);
  COCSIM_REQUIRE(pup && pup->hp == 50 && pup->damage == 35 && pup->cooldown == 1008
                 && pup->range == 2.75 && pup->flying && !pup->deployable);

  // Air Defense has absolute priority over the closer Cannon. The staged
  // fallback is retained in Core, not duplicated by a scenario adapter.
  Scenario priority;
  priority.width = 40; priority.height = 20; priority.duration_ms = 5000;
  priority.defenders = {{Kind::Cannon, 21, {9.5, 10.5}}, {Kind::AirDefense, 16, {30.5, 10.5}}};
  priority.army = {{Kind::IceHound, 8, 1}};
  BattleState priority_battle(data, priority);
  COCSIM_REQUIRE(priority_battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::IceHound, 8, {5.5, 10.5}}));
  priority_battle.advance_ticks(2);
  const auto priority_entities = priority_battle.observe();
  const auto priority_view = std::find_if(priority_entities.begin(), priority_entities.end(), [](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == Kind::IceHound;
  });
  COCSIM_REQUIRE(priority_view != priority_entities.end() && priority_view->target == 2);

  // Ten stacked Air Defenses are a compact deterministic kill fixture. The
  // 16 L8 Pups are queued on the parent death tick from source-backed count.
  Scenario scenario;
  scenario.width = 24; scenario.height = 20; scenario.duration_ms = 5000;
  for (int i = 0; i != 10; ++i) scenario.defenders.push_back({Kind::AirDefense, 16, {10.5, 10.5}});
  scenario.army = {{Kind::IceHound, 8, 1}, {Kind::Barbarian, 1, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::IceHound, 8, {5.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  for (int tick = 0; tick != 500 && ice_pup_count(battle) != 16; ++tick) battle.advance_ticks(1);
  COCSIM_REQUIRE(ice_pup_count(battle) == 16);
  const auto spawned = std::count_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail == "ice_pup spawned";
  });
  COCSIM_REQUIRE(spawned == 16);

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-ice-hound-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  for (int tick = 0; tick != 500 && ice_pup_count(replay) != 16; ++tick) replay.advance_ticks(1);
  COCSIM_REQUIRE(ice_pup_count(replay) == 16);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  battle.advance_ticks(20); restored.advance_ticks(20); replay.advance_ticks(20);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  // Cleanup is deliberately best-effort. It is not part of replay semantics,
  // and Windows can retain a transient file handle after the parser returns.
  std::remove(replay_path.c_str());
}
