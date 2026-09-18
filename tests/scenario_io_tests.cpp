#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace cocsim;

int main() {
  Scenario original;
  original.format_version = 3;
  original.width = 50;
  original.height = 50;
  original.seed = 42;
  original.duration_ms = 180000;
  original.defenders = {{Kind::TownHall, 18, {25.0, 25.0}, "normal", "normal"}};
  original.non_combat_obstacles = {{Kind::HeroBanner, {5.5, 5.5}}};
  original.army = {{Kind::Barbarian, 13, 2}};
  original.spells = {{SpellKind::Rage, 6, 1}};

  const std::vector<Command> commands{
    {CommandType::Deploy, kTickMs, kTickMs, 1, Kind::Barbarian, 13, {1.5, 1.5}},
    {CommandType::CastSpell, 2 * kTickMs, 2 * kTickMs, 2, Kind::Barbarian, 6, {10.5, 10.5}, SpellKind::Rage},
  };
  const std::string path = "scenario-io-domain-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(path, original, commands, error));

  Scenario loaded;
  std::vector<Command> loaded_commands;
  COCSIM_REQUIRE(load_replay(path, loaded, loaded_commands, error));
  COCSIM_REQUIRE(to_json(loaded) == to_json(original));
  COCSIM_REQUIRE(loaded_commands.size() == commands.size());
  COCSIM_REQUIRE(loaded_commands[0].type == CommandType::Deploy);
  COCSIM_REQUIRE(loaded_commands[0].kind == Kind::Barbarian);
  COCSIM_REQUIRE(loaded_commands[1].type == CommandType::CastSpell);
  COCSIM_REQUIRE(loaded_commands[1].spell == SpellKind::Rage);
  COCSIM_REQUIRE(std::remove(path.c_str()) == 0);
  std::cout << "scenario IO tests passed\n";
}
