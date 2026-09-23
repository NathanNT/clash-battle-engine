#include "demo_scenario.hpp"

namespace cocsim::viewer {

Scenario demo_scenario() {
  Scenario s;
  s.width = kHomeVillageTotalTiles;
  s.height = kHomeVillageTotalTiles;
  // A normal Home Village attack uses the complete three-minute clock.  The
  // Viewer may render several fixed ticks per frame, but this value remains
  // simulation time and never depends on wall-clock playback.
  s.duration_ms = 180'000;
  s.seed = 18'092'026;

  // TH18 air-attack practice base.  This is deliberately a compact, readable
  // war-base slice rather than a claim to reproduce a player layout or every
  // quantity limit. Each placement is an actually materialized Core level;
  // centres follow the Core footprint convention (odd widths at *.5, even
  // widths at integers). The inner wall ring creates a meaningful choice for
  // ground reinforcements while air troops exercise air defenses and traps.
  s.defenders = {
    // Core: Town Hall, heavy defenses and concealed traps.
    {Kind::TownHall, 18, {25.0, 25.0}},
    {Kind::InfernoTower, 12, {21.0, 21.0}, "multi_target"},
    {Kind::InfernoTower, 12, {29.0, 21.0}, "multi_target"},
    {Kind::Monolith, 5, {25.5, 29.5}},
    {Kind::HiddenTesla, 17, {21.0, 29.0}},
    {Kind::HiddenTesla, 17, {29.0, 29.0}},
    {Kind::SeekingAirMine, 8, {20.5, 27.5}},
    {Kind::AirBomb, 13, {28.5, 25.5}},
    {Kind::GiantBomb, 12, {20.5, 24.5}},
    {Kind::TornadoTrap, 3, {28.5, 27.5}},
    {Kind::SpringTrap, 13, {24.5, 21.5}},
    {Kind::SkeletonTrap, 5, {25.5, 21.5}, "air"},

    // Interior ring: long-range defenses cover approaches from every side.
    {Kind::EagleArtillery, 7, {25.0, 11.0}},
    {Kind::XBow, 13, {15.0, 15.0}, "air_and_ground"},
    {Kind::XBow, 13, {35.0, 15.0}, "air_and_ground"},
    {Kind::Scattershot, 6, {14.5, 25.5}},
    {Kind::Scattershot, 6, {35.5, 25.5}},
    {Kind::AirDefense, 16, {10.5, 15.5}},
    {Kind::AirDefense, 16, {39.5, 15.5}},
    {Kind::AirDefense, 16, {13.5, 34.5}},
    {Kind::AirDefense, 16, {36.5, 34.5}},
    {Kind::WizardTower, 17, {17.5, 35.5}},
    {Kind::WizardTower, 17, {32.5, 35.5}},
    {Kind::BombTower, 13, {25.5, 37.5}},
    {Kind::MultiArcherTower, 4, {17.5, 8.5}},
    {Kind::MultiGearTower, 3, {32.5, 8.5}},

    // Perimeter and targetable economy/army buildings make funneling visible.
    {Kind::Cannon, 21, {7.5, 20.5}}, {Kind::Cannon, 21, {42.5, 20.5}},
    {Kind::ArcherTower, 21, {7.5, 10.5}}, {Kind::ArcherTower, 21, {42.5, 10.5}},
    {Kind::Mortar, 18, {10.5, 40.5}}, {Kind::Mortar, 18, {39.5, 40.5}},
    {Kind::GoldMine, 10, {7.5, 30.5}}, {Kind::ElixirCollector, 10, {42.5, 30.5}},
    {Kind::GoldStorage, 15, {7.5, 36.5}}, {Kind::ElixirStorage, 15, {42.5, 36.5}},
    {Kind::DarkElixirStorage, 10, {11.5, 6.5}}, {Kind::DarkElixirDrill, 9, {38.5, 6.5}},
    {Kind::ArmyCamp, 12, {8.0, 45.0}}, {Kind::Barracks, 15, {15.5, 44.5}},
    {Kind::DarkBarracks, 10, {34.5, 44.5}}, {Kind::SpellFactory, 7, {21.5, 44.5}},
    {Kind::DarkSpellFactory, 6, {28.5, 44.5}},
    {Kind::CraftingStation, 1, {4.5, 44.5}},

    // A closed core compartment; wall cells are values in Core, not Viewer art.
    {Kind::Wall, 19, {18.5, 18.5}}, {Kind::Wall, 19, {19.5, 18.5}},
    {Kind::Wall, 19, {20.5, 18.5}}, {Kind::Wall, 19, {21.5, 18.5}},
    {Kind::Wall, 19, {22.5, 18.5}}, {Kind::Wall, 19, {23.5, 18.5}},
    {Kind::Wall, 19, {24.5, 18.5}}, {Kind::Wall, 19, {25.5, 18.5}},
    {Kind::Wall, 19, {26.5, 18.5}}, {Kind::Wall, 19, {27.5, 18.5}},
    {Kind::Wall, 19, {28.5, 18.5}}, {Kind::Wall, 19, {29.5, 18.5}},
    {Kind::Wall, 19, {30.5, 18.5}},
    {Kind::Wall, 19, {18.5, 19.5}}, {Kind::Wall, 19, {18.5, 20.5}},
    {Kind::Wall, 19, {18.5, 21.5}}, {Kind::Wall, 19, {18.5, 22.5}},
    {Kind::Wall, 19, {18.5, 23.5}}, {Kind::Wall, 19, {18.5, 24.5}},
    {Kind::Wall, 19, {18.5, 25.5}}, {Kind::Wall, 19, {18.5, 26.5}},
    {Kind::Wall, 19, {18.5, 27.5}}, {Kind::Wall, 19, {18.5, 28.5}},
    {Kind::Wall, 19, {18.5, 29.5}}, {Kind::Wall, 19, {18.5, 30.5}},
    {Kind::Wall, 19, {31.5, 19.5}}, {Kind::Wall, 19, {31.5, 20.5}},
    {Kind::Wall, 19, {31.5, 21.5}}, {Kind::Wall, 19, {31.5, 22.5}},
    {Kind::Wall, 19, {31.5, 23.5}}, {Kind::Wall, 19, {31.5, 24.5}},
    {Kind::Wall, 19, {31.5, 25.5}}, {Kind::Wall, 19, {31.5, 26.5}},
    {Kind::Wall, 19, {31.5, 27.5}}, {Kind::Wall, 19, {31.5, 28.5}},
    {Kind::Wall, 19, {31.5, 29.5}}, {Kind::Wall, 19, {31.5, 30.5}},
    {Kind::Wall, 19, {18.5, 31.5}}, {Kind::Wall, 19, {19.5, 31.5}},
    {Kind::Wall, 19, {20.5, 31.5}}, {Kind::Wall, 19, {21.5, 31.5}},
    {Kind::Wall, 19, {22.5, 31.5}}, {Kind::Wall, 19, {23.5, 31.5}},
    {Kind::Wall, 19, {24.5, 31.5}}, {Kind::Wall, 19, {25.5, 31.5}},
    {Kind::Wall, 19, {26.5, 31.5}}, {Kind::Wall, 19, {27.5, 31.5}},
    {Kind::Wall, 19, {28.5, 31.5}}, {Kind::Wall, 19, {29.5, 31.5}},
    {Kind::Wall, 19, {30.5, 31.5}},
  };

  // A plausible 304-space Super Dragon air composition. The demo keeps the
  // inventory undeployed: the player sends the same value commands as the
  // headless and RL adapters.
  s.army = {
    {Kind::SuperDragon, 13, 5}, {Kind::Balloon, 13, 6},
    {Kind::RocketBalloon, 12, 2}, {Kind::DragonRider, 6, 2},
    {Kind::Minion, 14, 4}, {Kind::MinionPrince, 95, 1},
    {Kind::GrandWarden, 85, 1, "air"}, {Kind::DragonDuke, 25, 1},
  };
  s.spells = {
    {SpellKind::Rage, 6, 3}, {SpellKind::Freeze, 7, 3}, {SpellKind::Haste, 7, 1},
  };
  return s;
}

} // namespace cocsim::viewer
