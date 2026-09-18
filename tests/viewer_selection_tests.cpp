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

  EntityView overlapping = bobs;
  overlapping.id = 5;
  entities.push_back(overlapping);
  COCSIM_REQUIRE(viewer::defender_at(entities, {4.0, 47.0}) == overlapping.id);
}
