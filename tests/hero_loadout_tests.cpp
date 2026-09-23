#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
bool rejects(const GameData& data, const Scenario& scenario) {
  try { BattleState ignored(data, scenario); }
  catch (const std::invalid_argument&) { return true; }
  return false;
}
}

int main() {
  const auto data = GameData::v0();
  Scenario scenario;
  scenario.width = 30;
  scenario.height = 25;
  scenario.duration_ms = 3000;
  scenario.defenders = {{Kind::GoldStorage, 1, {25.5, 20.5}}};
  scenario.army = {{Kind::ArcherQueen, 110, 1}, {Kind::BarbarianKing, 110, 1}};
  scenario.hero_loadouts = {
    {Kind::ArcherQueen, HeroSupportChoice{"lassi", 15},
      {{"giant_arrow", 18}, {"monolith_arrow", 27}}},
    {Kind::BarbarianKing, HeroSupportChoice{"unicorn", 15},
      {{"vampstache", 18}, {"giant_gauntlet", 27}}},
  };

  BattleState battle(data, scenario);
  COCSIM_REQUIRE(battle.scenario().hero_loadouts.size() == 2);
  COCSIM_REQUIRE(to_json(scenario.hero_loadouts).find("monolith_arrow") != std::string::npos);
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::ArcherQueen, 110, {2.5, 2.5}},
  };
  COCSIM_REQUIRE(battle.submit(commands.front()));
  battle.advance_ticks(2);
  const auto hash = battle.state_hash();
  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == hash);
  COCSIM_REQUIRE(battle.restore(snapshot));
  COCSIM_REQUIRE(battle.state_hash() == hash);
  battle.advance_ticks(20);
  restored.advance_ticks(20);
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  Scenario changed = scenario;
  changed.hero_loadouts[0].pet = HeroSupportChoice{"electro_owl", 15};
  BattleState wrong_loadout(data, changed);
  COCSIM_REQUIRE(!wrong_loadout.restore(snapshot));
  BattleState baseline(data, scenario);
  COCSIM_REQUIRE(wrong_loadout.state_hash() == baseline.state_hash());
  COCSIM_REQUIRE(wrong_loadout.snapshot().canonical != baseline.snapshot().canonical);
  COCSIM_REQUIRE(wrong_loadout.submit(commands.front()));
  wrong_loadout.advance_ticks(2);
  COCSIM_REQUIRE(wrong_loadout.state_hash() == hash);

  const std::string path = "replay-hero-loadout-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(path, scenario, commands, error));
  Scenario loaded;
  std::vector<Command> loaded_commands;
  COCSIM_REQUIRE(load_replay(path, loaded, loaded_commands, error));
  COCSIM_REQUIRE(to_json(loaded) == to_json(scenario));
  BattleState replay(data, loaded);
  for (const auto& command : loaded_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(22);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(path.c_str()) == 0);

  Scenario invalid = scenario;
  invalid.hero_loadouts[0].equipment.push_back({"magic_mirror", 27});
  COCSIM_REQUIRE(rejects(data, invalid));
  invalid = scenario;
  invalid.hero_loadouts[0].equipment[0].id = "vampstache";
  COCSIM_REQUIRE(rejects(data, invalid));
  invalid = scenario;
  invalid.hero_loadouts[0].pet->level = 16;
  COCSIM_REQUIRE(rejects(data, invalid));
  invalid = scenario;
  invalid.hero_loadouts[1].pet = HeroSupportChoice{"lassi", 15};
  COCSIM_REQUIRE(rejects(data, invalid));
  invalid = scenario;
  invalid.army[0].count = 2;
  COCSIM_REQUIRE(rejects(data, invalid));
  return 0;
}
