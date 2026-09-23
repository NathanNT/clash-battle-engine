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

} // namespace

int main() {
  const auto data = GameData::v0();
  const int expected_hp[] = {6200, 6350, 6500, 6700};
  const int expected_damage[] = {209, 231, 253, 275};
  for (int level = 1; level <= 4; ++level) {
    const auto* rider = data.find(Kind::RootRider, level);
    COCSIM_REQUIRE(rider && rider->hp == expected_hp[level - 1]
                   && rider->damage == expected_damage[level - 1]
                   && rider->range == 1 && rider->cooldown == 2208
                   && rider->speed == 12 && rider->movement_tiles_per_second == 1.5
                   && rider->housing_space == 20 && !rider->flying
                   && rider->target_focus == TargetFocus::Defenses
                   && rider->target_type == TargetType::Ground && rider->smashes_walls
                   && rider->wall_damage_per_hit == 4000
                   && rider->wall_attack_cooldown == 400 && rider->deployable);
  }
  const auto* rider4 = data.find(Kind::RootRider, 4);
  COCSIM_REQUIRE(rider4->attributes.town_hall_required == 18
                 && rider4->attributes.laboratory_required == 16);

  Scenario focus;
  focus.width = 30; focus.height = 20; focus.duration_ms = 8000;
  focus.defenders = {{Kind::GoldMine, 1, {12.5, 10.5}}, {Kind::Cannon, 1, {16.5, 10.5}}};
  focus.army = {{Kind::RootRider, 4, 1}};
  const std::vector<Command> focus_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::RootRider, 4, {5.5, 10.5}},
  };
  BattleState focused(data, focus);
  submit_all(focused, focus_commands); focused.advance_ticks(2);
  const auto views = focused.observe();
  const auto rider = std::find_if(views.begin(), views.end(), [](const EntityView& entity) {
    return entity.kind == Kind::RootRider && entity.side == Side::Attacker;
  });
  COCSIM_REQUIRE(rider != views.end() && rider->target == 2);

  Scenario walls;
  walls.width = 30; walls.height = 20; walls.duration_ms = 8000;
  walls.defenders = {{Kind::Wall, 1, {10.5, 10.5}}, {Kind::Cannon, 1, {18.5, 10.5}}};
  walls.army = {{Kind::RootRider, 4, 1}};
  const std::vector<Command> wall_commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::RootRider, 4, {5.5, 10.5}},
  };
  BattleState battle(data, walls);
  submit_all(battle, wall_commands); battle.advance_ticks(350);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Attack && event.actor == 3 && event.target == 1
        && event.value == 4000 && event.detail == "root rider wall touch";
  }));
  const auto snapshot = battle.snapshot();
  BattleState restored(data, walls);
  COCSIM_REQUIRE(restored.restore(snapshot));
  battle.advance_ticks(300); restored.advance_ticks(300);
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-root-rider-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, walls, wall_commands, error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands); replay.advance_ticks(650);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
