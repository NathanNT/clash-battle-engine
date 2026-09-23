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

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* pekka = data.find(Kind::Pekka, 13);
  COCSIM_REQUIRE(pekka && pekka->hp == 8800 && pekka->damage == 1692
                 && pekka->dps == 940 && pekka->cooldown == 1808
                 && pekka->range == .8 && pekka->movement_tiles_per_second == 2
                 && pekka->housing_space == 25 && !pekka->flying
                 && pekka->target_type == TargetType::Ground && pekka->deployable);

  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::HiddenTesla, 1, {10.5, 10.5}}};
  scenario.army = {{Kind::Pekka, 13, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Pekka, 13, {7.5, 10.5}},
  };
  BattleState battle(data, scenario);
  submit_all(battle, commands);
  battle.advance_ticks(200);
  // The historically removed Tesla x2 modifier must not silently return.
  COCSIM_REQUIRE(attack_with_value(battle, 1692));
  COCSIM_REQUIRE(!attack_with_value(battle, 3384));

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  for (int tick = 0; tick != 150; ++tick) {
    battle.advance_ticks(1);
    restored.advance_ticks(1);
    COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  }

  const std::string replay_path = "replay-pekka-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(350);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
