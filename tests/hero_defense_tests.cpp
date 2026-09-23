#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

bool hero_attacked(const BattleState& battle, EntityId hero_id) {
  return std::any_of(battle.events().begin(), battle.events().end(), [=](const Event& event) {
    return event.type == EventType::Attack && event.actor == hero_id;
  });
}

struct HeroCase {
  Kind kind;
  int level;
};

} // namespace

int main() {
  const auto data = GameData::v0();
  const std::vector<HeroCase> heroes = {
    {Kind::BarbarianKing, 110},
    {Kind::ArcherQueen, 110},
    {Kind::GrandWarden, 85},
    {Kind::RoyalChampion, 55},
    {Kind::MinionPrince, 95},
    {Kind::DragonDuke, 25},
  };

  for (const auto hero : heroes) {
    // This is an explicit Core defender placement, not an inferred Hero Banner
    // patrol. A single attacker keeps target selection and event ownership
    // observable while exercising the same base attack stats as every adapter.
    Scenario scenario;
    scenario.width = 24;
    scenario.height = 20;
    scenario.duration_ms = 6000;
    scenario.defenders = {{hero.kind, hero.level, {12.5, 10.5}}};
    scenario.army = {{Kind::Barbarian, 1, 1}};
    const std::vector<Command> commands = {
      {CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {8.5, 10.5}},
    };

    BattleState first(data, scenario);
    const auto initial_entities = first.observe();
    const auto defender = std::find_if(initial_entities.begin(), initial_entities.end(), [&](const EntityView& entity) {
      return entity.kind == hero.kind && entity.side == Side::Defender;
    });
    COCSIM_REQUIRE(defender != initial_entities.end());
    for (const auto& command : commands) COCSIM_REQUIRE(first.submit(command));
    bool attacked = false;
    for (int tick = 0; tick != 400; ++tick) {
      first.advance_ticks(1);
      attacked = attacked || hero_attacked(first, defender->id);
    }
    COCSIM_REQUIRE(attacked);

    const auto snapshot = first.snapshot();
    const auto hash = first.state_hash();
    BattleState restored(data, scenario);
    std::string error;
    COCSIM_REQUIRE(restored.restore(snapshot, &error));
    COCSIM_REQUIRE(restored.state_hash() == hash);
    first.advance_ticks(100);
    restored.advance_ticks(100);
    COCSIM_REQUIRE(first.state_hash() == restored.state_hash());

    const std::string replay_path = "replay-hero-defense-test.json";
    COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
    Scenario replay_scenario;
    std::vector<Command> replay_commands;
    COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
    BattleState replay(data, replay_scenario);
    for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
    replay.advance_ticks(500);
    COCSIM_REQUIRE(replay.state_hash() == first.state_hash());
    COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
  }
  return 0;
}
