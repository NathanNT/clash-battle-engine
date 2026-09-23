#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

int main() {
  const auto data = GameData::v0();
  const auto* hound = data.find(Kind::LavaHound, 8);
  const auto* pup = data.find(Kind::LavaPup, 1);
  COCSIM_REQUIRE(hound && hound->hp == 9500 && hound->damage == 48);
  COCSIM_REQUIRE(hound->cooldown == 2000 && hound->range == 1
                 && hound->flying && hound->target_type == TargetType::Ground
                 && hound->target_focus == TargetFocus::AirDefenses && hound->spawned_units == 15 && hound->deployable);
  COCSIM_REQUIRE(pup && pup->hp == 50 && pup->damage == 35 && pup->cooldown == 1008
                 && pup->range == 2 && pup->flying && !pup->deployable);
  Scenario scenario;
  scenario.width = 20; scenario.height = 20; scenario.duration_ms = 4000;
  scenario.defenders = {{Kind::Cannon, 1, {7.5, 10.5}}, {Kind::AirDefense, 1, {11.5, 10.5}}};
  scenario.army = {{Kind::LavaHound, 8, 1}};
  const std::vector<Command> commands = {{CommandType::Deploy, kTickMs, 0, 0, Kind::LavaHound, 8, {1.5, 10.5}}};
  BattleState battle(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
  battle.advance_ticks(600);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.target == 2 && event.value == 48;
  }));
  COCSIM_REQUIRE(std::none_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail.find("lava_pup") != std::string::npos;
  }));

  Scenario death;
  death.width = 24; death.height = 20; death.duration_ms = 5000;
  for (int i = 0; i != 10; ++i) death.defenders.push_back({Kind::AirDefense, 16, {10.5, 10.5}});
  death.army = {{Kind::LavaHound, 8, 1}};
  BattleState split(data, death);
  COCSIM_REQUIRE(split.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::LavaHound, 8, {5.5, 10.5}}));
  for (int tick = 0; tick != 500 && std::count_if(split.events().begin(), split.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail == "lava_pup spawned";
  }) != 15; ++tick) split.advance_ticks(1);
  const auto split_views = split.observe();
  const auto pups = std::count_if(split_views.begin(), split_views.end(), [](const EntityView& entity) {
    return entity.side == Side::Attacker && entity.kind == Kind::LavaPup;
  });
  COCSIM_REQUIRE(pups == 15);
  COCSIM_REQUIRE(std::count_if(split.events().begin(), split.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail == "lava_pup spawned";
  }) == 15);
  const auto split_snapshot = split.snapshot();
  BattleState split_restored(data, death); COCSIM_REQUIRE(split_restored.restore(split_snapshot));
  COCSIM_REQUIRE(split_restored.state_hash() == split.state_hash());
  const std::vector<Command> split_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::LavaHound, 8, {5.5, 10.5}}
  };
  const std::string split_replay_path = "replay-lava-pup-split-test.json"; std::string split_error;
  COCSIM_REQUIRE(save_replay(split_replay_path, death, split_commands, split_error));
  Scenario split_replay_scenario; std::vector<Command> split_replay_commands;
  COCSIM_REQUIRE(load_replay(split_replay_path, split_replay_scenario, split_replay_commands, split_error));
  BattleState split_replay(data, split_replay_scenario);
  for (const auto& command : split_replay_commands) COCSIM_REQUIRE(split_replay.submit(command));
  for (int tick = 0; tick != 500 && std::count_if(split_replay.events().begin(), split_replay.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail == "lava_pup spawned";
  }) != 15; ++tick) split_replay.advance_ticks(1);
  COCSIM_REQUIRE(std::count_if(split_replay.events().begin(), split_replay.events().end(), [](const Event& event) {
    return event.type == EventType::Deployed && event.detail == "lava_pup spawned";
  }) == 15);
  COCSIM_REQUIRE(split_replay.state_hash() == split.state_hash());
  std::remove(split_replay_path.c_str());
  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario); COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());
  const std::string replay_path = "replay-lava-hound-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(600); COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  std::remove(replay_path.c_str());
}
