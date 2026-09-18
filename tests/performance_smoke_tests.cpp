#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <chrono>
#include <iostream>

using namespace cocsim;

namespace {

Scenario load_scenario() {
  Scenario scenario;
  scenario.width = 44;
  scenario.height = 44;
  scenario.duration_ms = 10000;
  for (int index = 0; index < 10; ++index) {
    scenario.defenders.push_back({Kind::GoldMine, 17, {30.5 + (index % 3) * 4.0, 3.5 + (index / 3) * 4.0}});
  }
  scenario.army = {{Kind::Barbarian, 13, 50}};
  return scenario;
}

void deploy_army(BattleState& battle) {
  for (int index = 0; index < 50; ++index) {
    const Vec2 position{1.5, 0.5 + static_cast<double>(index % 44)};
    COCSIM_REQUIRE(battle.submit({CommandType::Deploy, kTickMs, 0, 0,
                                  Kind::Barbarian, 13, position}));
  }
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto scenario = load_scenario();
  BattleState headless(data, scenario);
  BattleState observed(data, scenario);
  deploy_army(headless);
  deploy_army(observed);

  constexpr std::uint32_t ticks = 120;
  const auto started = std::chrono::steady_clock::now();
  headless.advance_ticks(ticks);
  for (std::uint32_t tick = 0; tick < ticks; ++tick) {
    observed.advance_ticks(1);
    (void)observed.observe();
    (void)observed.observe_projectiles();
    (void)observed.result();
  }
  const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
  COCSIM_REQUIRE(headless.state_hash() == observed.state_hash());
  std::cout << "performance smoke: 2 x " << ticks << " ticks, 50 troops, "
            << elapsed << " seconds\n";
}
