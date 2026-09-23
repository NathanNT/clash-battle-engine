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
}

int main() {
  const auto data = GameData::v0();
  const auto* dragon = data.find(Kind::Dragon, 13);
  COCSIM_REQUIRE(dragon && dragon->hp == 6000 && dragon->dps == 430
                 && dragon->damage == 537.5 && dragon->cooldown == 1248
                 && dragon->range == 2.5 && dragon->housing_space == 20
                 && dragon->flying && dragon->target_type == TargetType::Both
                 && dragon->deployable);

  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::HiddenTesla, 1, {10.5, 10.5}}};
  scenario.army = {{Kind::Dragon, 13, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Dragon, 13, {7.5, 10.5}},
  };
  BattleState battle(data, scenario); submit_all(battle, commands);
  battle.advance_ticks(200);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.value == 537.5;
  }));
  const auto snapshot = battle.snapshot(); BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot) && restored.state_hash() == battle.state_hash());
  for (int tick = 0; tick != 150; ++tick) { battle.advance_ticks(1); restored.advance_ticks(1); COCSIM_REQUIRE(restored.state_hash() == battle.state_hash()); }
  const std::string replay_path = "replay-dragon-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario); submit_all(replay, replay_commands); replay.advance_ticks(350);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
