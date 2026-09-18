#include "demo_scenario.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace cocsim;

namespace {

const Placement* placement(const Scenario& scenario, Kind kind, int level) {
  const auto found = std::find_if(scenario.defenders.begin(), scenario.defenders.end(), [=](const Placement& candidate) {
    return candidate.kind == kind && candidate.level == level;
  });
  return found == scenario.defenders.end() ? nullptr : &*found;
}

const ArmySlot* army_slot(const Scenario& scenario, Kind kind, int level) {
  const auto found = std::find_if(scenario.army.begin(), scenario.army.end(), [=](const ArmySlot& candidate) {
    return candidate.kind == kind && candidate.level == level;
  });
  return found == scenario.army.end() ? nullptr : &*found;
}

bool overlaps(const Placement& left, const Stats& left_stats, const Placement& right, const Stats& right_stats) {
  const auto separated = [](double a_min, double a_max, double b_min, double b_max) {
    return a_max <= b_min || b_max <= a_min;
  };
  return !separated(left.position.x - left_stats.footprint_width / 2.0,
                    left.position.x + left_stats.footprint_width / 2.0,
                    right.position.x - right_stats.footprint_width / 2.0,
                    right.position.x + right_stats.footprint_width / 2.0)
      && !separated(left.position.y - left_stats.footprint_height / 2.0,
                     left.position.y + left_stats.footprint_height / 2.0,
                     right.position.y - right_stats.footprint_height / 2.0,
                     right.position.y + right_stats.footprint_height / 2.0);
}

} // namespace

int main() {
  const auto scenario = viewer::demo_scenario();
  const auto data = GameData::v0();
  COCSIM_REQUIRE(scenario.width == kHomeVillageTotalTiles);
  COCSIM_REQUIRE(scenario.height == kHomeVillageTotalTiles);
  COCSIM_REQUIRE(scenario.duration_ms == 180'000);
  COCSIM_REQUIRE(placement(scenario, Kind::TownHall, 18));
  COCSIM_REQUIRE(placement(scenario, Kind::Monolith, 5));
  COCSIM_REQUIRE(placement(scenario, Kind::EagleArtillery, 7));
  COCSIM_REQUIRE(placement(scenario, Kind::AirDefense, 16));
  COCSIM_REQUIRE(army_slot(scenario, Kind::SuperDragon, 13)->count == 5);
  COCSIM_REQUIRE(army_slot(scenario, Kind::DragonRider, 6)->count == 2);

  // The default Viewer scenario must remain materializable and must never rely
  // on overlapping Core footprints hidden by rendering order.
  for (std::size_t left = 0; left < scenario.defenders.size(); ++left) {
    const auto* left_stats = data.find(scenario.defenders[left].kind, scenario.defenders[left].level,
                                       scenario.defenders[left].variant);
    COCSIM_REQUIRE(left_stats);
    // Home Village buildings occupy only the central 44x44 construction grid.
    // The surrounding three tiles are attacker deployment border, never a
    // valid defender footprint, even for a four-tile Army Camp.
    COCSIM_REQUIRE(scenario.defenders[left].position.x - left_stats->footprint_width / 2.0 >= kHomeVillageDeploymentBorderTiles);
    COCSIM_REQUIRE(scenario.defenders[left].position.y - left_stats->footprint_height / 2.0 >= kHomeVillageDeploymentBorderTiles);
    COCSIM_REQUIRE(scenario.defenders[left].position.x + left_stats->footprint_width / 2.0 <= scenario.width - kHomeVillageDeploymentBorderTiles);
    COCSIM_REQUIRE(scenario.defenders[left].position.y + left_stats->footprint_height / 2.0 <= scenario.height - kHomeVillageDeploymentBorderTiles);
    for (std::size_t right = left + 1; right < scenario.defenders.size(); ++right) {
      const auto* right_stats = data.find(scenario.defenders[right].kind, scenario.defenders[right].level,
                                          scenario.defenders[right].variant);
      COCSIM_REQUIRE(right_stats);
      if (overlaps(scenario.defenders[left], *left_stats, scenario.defenders[right], *right_stats)) {
        std::cerr << "overlapping demo placements: " << to_string(scenario.defenders[left].kind)
                  << " at " << scenario.defenders[left].position.x << ',' << scenario.defenders[left].position.y
                  << " and " << to_string(scenario.defenders[right].kind)
                  << " at " << scenario.defenders[right].position.x << ',' << scenario.defenders[right].position.y << '\n';
        COCSIM_REQUIRE(false);
      }
    }
  }

  BattleState battle(data, scenario);
  COCSIM_REQUIRE(battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::SuperDragon, 13, {1.5, 25.5}}));
  COCSIM_REQUIRE(battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::Balloon, 13, {1.5, 26.5}}));
  COCSIM_REQUIRE(battle.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::DragonRider, 6, {1.5, 27.5}}));

  // The closed wall compartment is not a GUI-only no-deployment area. At
  // least one genuinely free cell inside its wall ring must be accepted by
  // the exact same Core command used by a live Viewer click.
  bool interior_cell_deployable = false;
  for (int y = 20; y <= 29 && !interior_cell_deployable; ++y) for (int x = 20; x <= 29; ++x) {
    BattleState candidate(data, scenario);
    COCSIM_REQUIRE(candidate.submit({CommandType::Deploy, kTickMs, 0, 0, Kind::SuperDragon, 13, {x + .5, y + .5}}));
    candidate.advance_ticks(2);
    interior_cell_deployable = std::any_of(candidate.events().begin(), candidate.events().end(), [](const Event& event) {
      return event.type == EventType::Deployed;
    });
    if (interior_cell_deployable) break;
  }
  COCSIM_REQUIRE(interior_cell_deployable);
}
