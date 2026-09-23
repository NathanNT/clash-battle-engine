#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
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
  original.non_combat_obstacles = {{Kind::HeroBanner, {5.5, 5.5}, "grand_warden"}};
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
  COCSIM_REQUIRE(loaded.non_combat_obstacles[0].variant == "grand_warden");
  Scenario too_many_banners = original;
  too_many_banners.non_combat_obstacles = {
    {Kind::HeroBanner, {5.5, 5.5}}, {Kind::HeroBanner, {8.5, 5.5}},
    {Kind::HeroBanner, {11.5, 5.5}}, {Kind::HeroBanner, {14.5, 5.5}},
    {Kind::HeroBanner, {17.5, 5.5}},
  };
  bool rejected_banner_limit = false;
  try { BattleState rejected(GameData::v0(), too_many_banners); }
  catch (const std::invalid_argument&) { rejected_banner_limit = true; }
  COCSIM_REQUIRE(rejected_banner_limit);
  Scenario too_many_attacking_heroes = original;
  too_many_attacking_heroes.non_combat_obstacles.clear();
  too_many_attacking_heroes.army = {
    {Kind::BarbarianKing, 110, 1}, {Kind::ArcherQueen, 110, 1},
    {Kind::GrandWarden, 85, 1}, {Kind::RoyalChampion, 55, 1},
    {Kind::MinionPrince, 95, 1},
  };
  bool rejected_hero_limit = false;
  try { BattleState rejected(GameData::v0(), too_many_attacking_heroes); }
  catch (const std::invalid_argument&) { rejected_hero_limit = true; }
  COCSIM_REQUIRE(rejected_hero_limit);
  COCSIM_REQUIRE(loaded_commands.size() == commands.size());
  COCSIM_REQUIRE(loaded_commands[0].type == CommandType::Deploy);
  COCSIM_REQUIRE(loaded_commands[0].kind == Kind::Barbarian);
  COCSIM_REQUIRE(loaded_commands[1].type == CommandType::CastSpell);
  COCSIM_REQUIRE(loaded_commands[1].spell == SpellKind::Rage);
  std::ifstream replay_input(path);
  const std::string encoded{std::istreambuf_iterator<char>{replay_input}, std::istreambuf_iterator<char>{}};
  replay_input.close();
  COCSIM_REQUIRE(encoded.find("\"format_version\":5") != std::string::npos);
  const std::string replay_tick = "\"tick_ms\":" + std::to_string(kTickMs);
  const auto tick_field = encoded.find(replay_tick);
  COCSIM_REQUIRE(tick_field != std::string::npos);
  std::string wrong_grid = encoded;
  wrong_grid.replace(tick_field, replay_tick.size(), "\"tick_ms\":10");
  const std::string wrong_grid_path = "scenario-io-wrong-grid-domain-test.json";
  std::ofstream wrong_grid_output(wrong_grid_path);
  wrong_grid_output << wrong_grid;
  wrong_grid_output.close();
  COCSIM_REQUIRE(!load_replay(wrong_grid_path, loaded, loaded_commands, error));
  COCSIM_REQUIRE(error == "replay tick duration mismatch (expected 16 ms)");
  COCSIM_REQUIRE(std::remove(wrong_grid_path.c_str()) == 0);
  const std::string requested_time = "\"requested_ms\":" + std::to_string(kTickMs);
  const std::string effective_time = "\"effective_ms\":" + std::to_string(kTickMs);
  const auto timestamp = encoded.find(requested_time);
  const auto effective_timestamp = encoded.find(effective_time);
  COCSIM_REQUIRE(timestamp != std::string::npos);
  COCSIM_REQUIRE(effective_timestamp != std::string::npos);
  std::string malformed = encoded;
  malformed.replace(timestamp, requested_time.size(), "\"requested_ms\":" + std::to_string(kTickMs + 1));
  malformed.replace(effective_timestamp, effective_time.size(), "\"effective_ms\":" + std::to_string(kTickMs + 1));
  const std::string malformed_path = "scenario-io-malformed-replay-domain-test.json";
  std::ofstream malformed_output(malformed_path);
  malformed_output << malformed;
  malformed_output.close();
  COCSIM_REQUIRE(!load_replay(malformed_path, loaded, loaded_commands, error));
  COCSIM_REQUIRE(error == "replay command time must be a future multiple of 16 ms");
  COCSIM_REQUIRE(std::remove(malformed_path.c_str()) == 0);
  COCSIM_REQUIRE(std::remove(path.c_str()) == 0);
  std::cout << "scenario IO tests passed\n";
}
