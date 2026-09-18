#pragma once

#include "cocsim/core.hpp"

#include <cmath>

namespace cocsim::detail {

inline double dist(Vec2 left, Vec2 right) {
  const double dx = left.x - right.x;
  const double dy = left.y - right.y;
  return std::sqrt(dx * dx + dy * dy);
}

} // namespace cocsim::detail

