#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <iostream>

using namespace cocsim;

namespace {

Scenario deterministic_scenario() {
  Scenario scenario;
  scenario.width = 30;
  scenario.height = 20;
  scenario.duration_ms = 5000;
  scenario.defenders = {{Kind::Cannon, 1, {20.5, 10.5}}, {Kind::GoldMine, 1, {25.5, 10.5}}};
  scenario.army = {{Kind::Barbarian, 1, 2}};
  return scenario;
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto scenario = deterministic_scenario();
  const Command command{CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {2.5, 10.5}};

  BattleState batched(data, scenario);
  BattleState observed(data, scenario);
  COCSIM_REQUIRE(batched.submit(command));
  COCSIM_REQUIRE(observed.submit(command));
  batched.advance_ticks(150);
  for (int tick = 0; tick < 150; ++tick) {
    observed.advance_ticks(1);
    (void)observed.observe();
    (void)observed.observe_projectiles();
    (void)observed.observe_spell_effects();
    (void)observed.result();
  }
  COCSIM_REQUIRE(batched.state_hash() == observed.state_hash());

  const auto checkpoint = batched.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(checkpoint));
  batched.advance_ticks(100);
  restored.advance_ticks(100);
  COCSIM_REQUIRE(batched.state_hash() == restored.state_hash());
  COCSIM_REQUIRE(batched.events().size() >= observed.events().size());
  std::cout << "determinism tests passed\n";
}
