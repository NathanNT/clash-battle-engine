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

bool has_split(const BattleState& battle) {
  const auto entities = battle.observe();
  const auto has_kind = [&](Kind kind) {
    return std::any_of(entities.begin(), entities.end(), [kind](const EntityView& entity) {
      return entity.side == Side::Attacker && entity.kind == kind && entity.level == 10;
    });
  };
  return has_kind(Kind::SuperHog) && has_kind(Kind::SuperRider);
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* parent_l12 = data.find(Kind::SuperHogRider, 12);
  const auto* hog_l10 = data.find(Kind::SuperHog, 10);
  const auto* hog_l15 = data.find(Kind::SuperHog, 15);
  const auto* rider_l10 = data.find(Kind::SuperRider, 10);
  const auto* rider_l15 = data.find(Kind::SuperRider, 15);
  COCSIM_REQUIRE(parent_l12 && parent_l12->hp == 1700 && parent_l12->damage == 230);
  COCSIM_REQUIRE(hog_l10 && hog_l10->hp == 800 && hog_l10->damage == 50
                 && hog_l10->target_focus == TargetFocus::Defenses && !hog_l10->deployable);
  COCSIM_REQUIRE(hog_l15 && hog_l15->hp == 1200 && hog_l15->damage == 80);
  COCSIM_REQUIRE(rider_l10 && rider_l10->hp == 550 && rider_l10->damage == 180
                 && rider_l10->wall_damage_multiplier == 2 && !rider_l10->deployable);
  COCSIM_REQUIRE(rider_l15 && rider_l15->hp == 900 && rider_l15->damage == 300);

  // The cannon pair destroys the parent before it can clear their position.
  // A wall survives next to the death point, so the Rider's unrestricted
  // shared targeting path can continue after a deterministic split.
  Scenario scenario;
  scenario.width = 28;
  scenario.height = 20;
  scenario.duration_ms = 30000;
  scenario.defenders = {
    {Kind::Wall, 19, {12.5, 10.5}},
    {Kind::Cannon, 21, {17.5, 8.5}},
    {Kind::Cannon, 21, {17.5, 12.5}},
  };
  scenario.army = {{Kind::SuperHogRider, 10, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::SuperHogRider, 10, {5.5, 10.5}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  for (int tick = 0; tick != 2000 && !has_split(battle); ++tick) battle.advance_ticks(1);
  COCSIM_REQUIRE(has_split(battle));
  const auto spawned = std::count_if(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed
           && (event.detail == "super_hog spawned" || event.detail == "super_rider spawned");
  });
  COCSIM_REQUIRE(spawned == 2);

  // Spawn queue order is a value-only deterministic contract. The resulting
  // entities survive Snapshot and replay reconstruction without adapter state.
  const auto snapshot = battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-22\n"));
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-super-hog-rider-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  for (int tick = 0; tick != 2000 && !has_split(replay); ++tick) replay.advance_ticks(1);
  COCSIM_REQUIRE(has_split(replay));
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());

  battle.advance_ticks(10);
  restored.advance_ticks(10);
  replay.advance_ticks(10);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash() == replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
