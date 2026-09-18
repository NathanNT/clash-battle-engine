#pragma once

#include "cocsim/core.hpp"

#include <optional>
#include <vector>

namespace cocsim::viewer {

// Presentation-only hit test against the exact Core footprint.  A defender
// can overlap a cosmetic sprite canvas, but only this rectangle represents
// occupied cells.  Equal overlap is resolved by entity id so selection stays
// deterministic even for malformed/debug scenarios.
inline std::optional<EntityId> defender_at(const std::vector<EntityView>& entities,
                                           Vec2 position) {
  std::optional<EntityId> result;
  for (const auto& entity : entities) {
    if (entity.side != Side::Defender) continue;
    const double left = entity.position.x - double(entity.footprint_width) / 2.0;
    const double top = entity.position.y - double(entity.footprint_height) / 2.0;
    const double right = left + entity.footprint_width;
    const double bottom = top + entity.footprint_height;
    if (position.x < left || position.x >= right || position.y < top || position.y >= bottom)
      continue;
    if (!result || entity.id < *result) result = entity.id;
  }
  return result;
}

inline const EntityView* defender_with_id(const std::vector<EntityView>& entities,
                                          EntityId id) {
  for (const auto& entity : entities)
    if (entity.side == Side::Defender && entity.id == id) return &entity;
  return nullptr;
}

} // namespace cocsim::viewer
