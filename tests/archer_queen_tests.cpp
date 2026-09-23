#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
const EntityView* queen(const std::vector<EntityView>& entities) {
  for (const auto& entity : entities)
    if (entity.kind == Kind::ArcherQueen && entity.side == Side::Attacker) return &entity;
  return nullptr;
}
}

int main() {
  const auto data = GameData::v0();
  COCSIM_REQUIRE(!data.find(Kind::ArcherQueen, 100));
  const auto* l101 = data.find(Kind::ArcherQueen, 101);
  const auto* l110 = data.find(Kind::ArcherQueen, 110);
  COCSIM_REQUIRE(l101 && l110);
  COCSIM_REQUIRE(l101->hp == 3360 && l101->damage == 587.25 && l101->cooldown == 752);
  COCSIM_REQUIRE(l110->hp == 3576 && l110->damage == 607.5 && l110->dps == 810);
  COCSIM_REQUIRE(l110->category == EntityCategory::Troop && l110->target_type == TargetType::Both);
  COCSIM_REQUIRE(l110->range == 5.0);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 6000;
  scenario.defenders = {{Kind::GoldMine, 1, {11.5, 10.5}}};
  scenario.army = {{Kind::ArcherQueen, 110, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::ArcherQueen, 110, {4.5, 10.5}},
  };
  BattleState first(data, scenario);
  for (const auto& command : commands) COCSIM_REQUIRE(first.submit(command));
  first.advance_ticks(80);
  COCSIM_REQUIRE(queen(first.observe()));
  COCSIM_REQUIRE(first.state_hash() != 0);
  const auto snapshot = first.snapshot();
  const auto expected_hash = first.state_hash();
  BattleState restored(data, scenario);
  std::string error;
  COCSIM_REQUIRE(restored.restore(snapshot, &error));
  COCSIM_REQUIRE(restored.state_hash() == expected_hash);
  first.advance_ticks(100);
  restored.advance_ticks(100);
  COCSIM_REQUIRE(first.state_hash() == restored.state_hash());
  const std::string replay_path = "replay-archer-queen-test.json";
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(180);
  COCSIM_REQUIRE(replay.state_hash() == first.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
  return 0;
}
