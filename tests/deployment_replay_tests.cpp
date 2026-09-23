#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

bool has_event(const BattleState& battle, EventType type, const std::string& detail) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == type && event.detail == detail;
  });
}

} // namespace

int main() {
  Scenario scenario;
  scenario.width = 10;
  scenario.height = 10;
  scenario.duration_ms = 10'000;
  // A 3x3 free interior. Walls retain their one-cell deployment exclusion;
  // the centre is nevertheless deployable because it is outside every ring.
  scenario.defenders = {
    {Kind::Wall, 19, {1.5, 1.5}}, {Kind::Wall, 19, {2.5, 1.5}}, {Kind::Wall, 19, {3.5, 1.5}}, {Kind::Wall, 19, {4.5, 1.5}}, {Kind::Wall, 19, {5.5, 1.5}}, {Kind::Wall, 19, {6.5, 1.5}}, {Kind::Wall, 19, {7.5, 1.5}},
    {Kind::Wall, 19, {1.5, 2.5}}, {Kind::Wall, 19, {7.5, 2.5}}, {Kind::Wall, 19, {1.5, 3.5}}, {Kind::Wall, 19, {7.5, 3.5}}, {Kind::Wall, 19, {1.5, 4.5}}, {Kind::Wall, 19, {7.5, 4.5}}, {Kind::Wall, 19, {1.5, 5.5}}, {Kind::Wall, 19, {7.5, 5.5}}, {Kind::Wall, 19, {1.5, 6.5}}, {Kind::Wall, 19, {7.5, 6.5}},
    {Kind::Wall, 19, {1.5, 7.5}}, {Kind::Wall, 19, {2.5, 7.5}}, {Kind::Wall, 19, {3.5, 7.5}}, {Kind::Wall, 19, {4.5, 7.5}}, {Kind::Wall, 19, {5.5, 7.5}}, {Kind::Wall, 19, {6.5, 7.5}}, {Kind::Wall, 19, {7.5, 7.5}},
  };
  scenario.army = {{Kind::Barbarian, 13, 2}};

  const auto data = GameData::v0();
  BattleState battle(data, scenario);
  COCSIM_REQUIRE(battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 13, {4.5, 4.5}}));
  // This cell is empty, but precisely one tile inside the upper wall: the
  // exclusion ring, rather than merely the wall footprint, must reject it.
  COCSIM_REQUIRE(battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 13, {4.5, 2.5}}));
  battle.advance_ticks(2);

  const auto views = battle.observe();
  const auto deployed = std::find_if(views.begin(), views.end(), [](const EntityView& view) {
    return view.side == Side::Attacker;
  });
  COCSIM_REQUIRE(deployed != views.end());
  COCSIM_REQUIRE(has_event(battle, EventType::Rejected, "deployment blocked within one tile of defender"));

  // The Core command log captures both accepted player inputs with their
  // future effective tick and stable sequence. Persisting it reproduces the
  // accepted deployment and the later rule-level rejection exactly.
  const auto& commands = battle.commands();
  COCSIM_REQUIRE(commands.size() == 2);
  COCSIM_REQUIRE(commands[0].effective_ms == kTickMs && commands[0].sequence == 1);
  COCSIM_REQUIRE(commands[1].effective_ms == kTickMs && commands[1].sequence == 2);
  const std::string replay_path = "deployment-replay-domain-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));

  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  COCSIM_REQUIRE(replay_commands.size() == commands.size());
  COCSIM_REQUIRE(replay_commands[0].effective_ms == kTickMs && replay_commands[0].sequence == 1);
  COCSIM_REQUIRE(replay_commands[1].effective_ms == kTickMs && replay_commands[1].sequence == 2);
  BattleState replay(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay.submit(command));
  replay.advance_ticks(2);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);

  // Traps are concealed devices, not buildings: they may trigger after a
  // deployment but must never manufacture an invisible no-deployment margin.
  Scenario trap_scenario;
  trap_scenario.width = 10; trap_scenario.height = 10; trap_scenario.duration_ms = 1'000;
  trap_scenario.defenders = {{Kind::GiantBomb, 12, {4.5, 4.5}}};
  trap_scenario.army = {{Kind::Barbarian, 13, 1}};
  BattleState trap_battle(data, trap_scenario);
  COCSIM_REQUIRE(trap_battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 13, {4.5, 4.5}}));
  trap_battle.advance_ticks(2);
  COCSIM_REQUIRE(has_event(trap_battle, EventType::Deployed, "barbarian"));
  COCSIM_REQUIRE(!has_event(trap_battle, EventType::Rejected, "deployment blocked within one tile of defender"));

  Scenario metrics_scenario;
  metrics_scenario.width = 20; metrics_scenario.height = 20; metrics_scenario.duration_ms = 1'000;
  metrics_scenario.defenders = {{Kind::TownHall, 18, {10.0, 10.0}}};
  metrics_scenario.army = {{Kind::Barbarian, 13, 1}, {Kind::Dragon, 13, 1}};
  BattleState metrics(data, metrics_scenario);
  COCSIM_REQUIRE(metrics.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 13, {1.5, 10.5}}));
  COCSIM_REQUIRE(metrics.submit({CommandType::EndBattle, 2 * kTickMs}));
  metrics.advance_ticks(3);
  const auto result = metrics.result();
  COCSIM_REQUIRE(result.finished && result.reason == "player ended battle");
  const auto* barbarian = data.find(Kind::Barbarian, 13);
  const auto* dragon = data.find(Kind::Dragon, 13);
  COCSIM_REQUIRE(barbarian && dragon);
  COCSIM_REQUIRE(result.stars == 0 && result.troops_remaining == 2
                 && result.remaining_housing_space == barbarian->housing_space + dragon->housing_space
                 && result.remaining_time_ms == 1'000 - 2 * kTickMs);
  const auto terminal_snapshot = metrics.snapshot();
  BattleState restored_metrics(data, metrics_scenario);
  COCSIM_REQUIRE(restored_metrics.restore(terminal_snapshot));
  COCSIM_REQUIRE(restored_metrics.state_hash() == metrics.state_hash());
  const std::string terminal_replay_path = "terminal-replay-domain-test.json";
  COCSIM_REQUIRE(save_replay(terminal_replay_path, metrics_scenario, metrics.commands(), error));
  COCSIM_REQUIRE(load_replay(terminal_replay_path, replay_scenario, replay_commands, error));
  BattleState replay_terminal(data, replay_scenario);
  for (const auto& command : replay_commands) COCSIM_REQUIRE(replay_terminal.submit(command));
  replay_terminal.advance_ticks(3);
  COCSIM_REQUIRE(replay_terminal.state_hash() == metrics.state_hash());
  COCSIM_REQUIRE(replay_terminal.result().reason == "player ended battle");
  COCSIM_REQUIRE(std::remove(terminal_replay_path.c_str()) == 0);
  std::cout << "deployment and replay tests passed\n";
}
