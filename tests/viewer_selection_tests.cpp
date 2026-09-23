#include "selection.hpp"
#include "test_support.hpp"

#include <vector>

using namespace cocsim;

int main() {
  EntityView bobs;
  bobs.id = 7;
  bobs.kind = Kind::BobsHut;
  bobs.side = Side::Defender;
  bobs.position = {4.0, 47.0};
  bobs.footprint_width = 2;
  bobs.footprint_height = 2;

  EntityView helper;
  helper.id = 9;
  helper.kind = Kind::HelperHut;
  helper.side = Side::Defender;
  helper.position = {12.5, 48.5};
  helper.footprint_width = 3;
  helper.footprint_height = 3;

  EntityView attacker = helper;
  attacker.id = 3;
  attacker.side = Side::Attacker;
  std::vector<EntityView> entities{bobs, helper, attacker};

  COCSIM_REQUIRE(viewer::defender_at(entities, {3.0, 46.0}) == bobs.id);
  COCSIM_REQUIRE(viewer::defender_at(entities, {4.99, 47.99}) == bobs.id);
  COCSIM_REQUIRE(!viewer::defender_at(entities, {5.0, 47.0}));
  COCSIM_REQUIRE(viewer::defender_at(entities, {11.0, 47.0}) == helper.id);
  COCSIM_REQUIRE(viewer::defender_at(entities, {13.99, 49.99}) == helper.id);
  COCSIM_REQUIRE(!viewer::defender_at(entities, {10.99, 48.0}));
  COCSIM_REQUIRE(viewer::defender_with_id(entities, bobs.id)->kind == Kind::BobsHut);
  COCSIM_REQUIRE(viewer::defender_with_id(entities, helper.id)->kind == Kind::HelperHut);
  COCSIM_REQUIRE(viewer::defender_with_id(entities, attacker.id) == nullptr);

  EntityView lava_pup = attacker;
  lava_pup.id = 4;
  lava_pup.kind = Kind::LavaPup;
  lava_pup.position = {8.5, 47.5};
  lava_pup.footprint_width = 1;
  lava_pup.footprint_height = 1;
  entities.push_back(lava_pup);
  // Spawned attackers are rendered from the Core view but are never accepted
  // as defender placement/selection targets.
  COCSIM_REQUIRE(!viewer::defender_at(entities, {8.5, 47.5}));
  COCSIM_REQUIRE(viewer::defender_with_id(entities, lava_pup.id) == nullptr);

  EntityView air_defense;
  air_defense.id = 11;
  air_defense.kind = Kind::AirDefense;
  air_defense.side = Side::Defender;
  air_defense.position = {12.5, 10.5};
  air_defense.footprint_width = 3;
  air_defense.footprint_height = 3;
  entities.push_back(air_defense);
  // The visual selector consumes the same half-open 3x3 Core footprint as
  // scenario collision and range overlays; no sprite compensation is allowed.
  COCSIM_REQUIRE(viewer::defender_at(entities, {11.0, 9.0}) == air_defense.id);
  COCSIM_REQUIRE(viewer::defender_at(entities, {13.99, 11.99}) == air_defense.id);
  COCSIM_REQUIRE(!viewer::defender_at(entities, {14.0, 11.0}));
  COCSIM_REQUIRE(viewer::defender_with_id(entities, air_defense.id)->kind == Kind::AirDefense);

  EntityView xbow;
  xbow.id = 13;
  xbow.kind = Kind::XBow;
  xbow.side = Side::Defender;
  xbow.position = {18.5, 10.5};
  xbow.footprint_width = 3;
  xbow.footprint_height = 3;
  entities.push_back(xbow);
  COCSIM_REQUIRE(viewer::defender_at(entities, {17.0, 9.0}) == xbow.id);
  COCSIM_REQUIRE(viewer::defender_at(entities, {19.99, 11.99}) == xbow.id);
  COCSIM_REQUIRE(!viewer::defender_at(entities, {20.0, 10.0}));

  EntityView crafting_station;
  crafting_station.id = 15;
  crafting_station.kind = Kind::CraftingStation;
  crafting_station.side = Side::Defender;
  crafting_station.position = {24.5, 10.5};
  crafting_station.footprint_width = 3;
  crafting_station.footprint_height = 3;
  entities.push_back(crafting_station);
  COCSIM_REQUIRE(viewer::defender_at(entities, {23.0, 9.0}) == crafting_station.id);
  COCSIM_REQUIRE(viewer::defender_at(entities, {25.99, 11.99}) == crafting_station.id);
  COCSIM_REQUIRE(!viewer::defender_at(entities, {26.0, 10.0}));
  COCSIM_REQUIRE(viewer::defender_with_id(entities, crafting_station.id)->kind == Kind::CraftingStation);

  EntityView overlapping = bobs;
  overlapping.id = 5;
  entities.push_back(overlapping);
  COCSIM_REQUIRE(viewer::defender_at(entities, {4.0, 47.0}) == overlapping.id);
}
