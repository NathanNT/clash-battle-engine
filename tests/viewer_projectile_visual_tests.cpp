#include "projectile_visuals.hpp"
#include "test_support.hpp"

using namespace cocsim;

int main() {
  viewer::ProjectileTrace logical{{1.0, 2.0}, {9.0, 2.0}, 1'000, 1'180,
                                  viewer::ProjectileTraceKind::LogicalImpact};
  COCSIM_REQUIRE(viewer::uses_cosmetic_projectile_flight(logical.kind));
  COCSIM_REQUIRE(viewer::cosmetic_projectile_progress(logical, 1'000) == 0.0f);
  COCSIM_REQUIRE(viewer::cosmetic_projectile_progress(logical, 1'070) == 0.5f);
  COCSIM_REQUIRE(viewer::cosmetic_projectile_progress(logical, 1'140) == 1.0f);
  COCSIM_REQUIRE(viewer::cosmetic_projectile_progress(logical, 1'180) == 1.0f);

  logical.kind = viewer::ProjectileTraceKind::Healing;
  COCSIM_REQUIRE(viewer::uses_cosmetic_projectile_flight(logical.kind));
  COCSIM_REQUIRE(viewer::cosmetic_projectile_progress(logical, 1'035) == 0.25f);
  logical.kind = viewer::ProjectileTraceKind::Beam;
  COCSIM_REQUIRE(!viewer::uses_cosmetic_projectile_flight(logical.kind));
  COCSIM_REQUIRE(viewer::cosmetic_projectile_progress(logical, 1'000) == 1.0f);
  logical.kind = viewer::ProjectileTraceKind::SourcedImpact;
  COCSIM_REQUIRE(viewer::cosmetic_projectile_progress(logical, 1'000) == 1.0f);
}
