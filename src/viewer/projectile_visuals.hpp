#pragma once

#include "cocsim/core.hpp"

#include <algorithm>
#include <cstdint>

namespace cocsim::viewer {

// Viewer-only timing. It is deliberately separate from kTickMs: these values
// shape a post-resolution visual trace and never reach BattleState, a replay,
// a snapshot or a state hash.
constexpr std::uint64_t kCosmeticProjectileFlightMs = 140;
constexpr std::uint64_t kCosmeticProjectileTraceMs = 180;

enum class ProjectileTraceKind { LogicalImpact, Healing, Beam, SourcedImpact };

// A post-impact presentation record. `launch_wall_ms` belongs only to the
// viewer clock; the Core event already carries the authoritative T+ms result.
struct ProjectileTrace {
  Vec2 origin{};
  Vec2 impact{};
  std::uint64_t launch_wall_ms{};
  std::uint64_t expires_wall_ms{};
  ProjectileTraceKind kind{ProjectileTraceKind::LogicalImpact};
};

inline bool uses_cosmetic_projectile_flight(ProjectileTraceKind kind) {
  return kind == ProjectileTraceKind::LogicalImpact || kind == ProjectileTraceKind::Healing;
}

inline float cosmetic_projectile_progress(const ProjectileTrace& trace, std::uint64_t wall_now_ms) {
  if (!uses_cosmetic_projectile_flight(trace.kind) || wall_now_ms >= trace.launch_wall_ms + kCosmeticProjectileFlightMs)
    return 1.0f;
  if (wall_now_ms <= trace.launch_wall_ms) return 0.0f;
  return std::clamp(float(wall_now_ms - trace.launch_wall_ms) / float(kCosmeticProjectileFlightMs), 0.0f, 1.0f);
}

} // namespace cocsim::viewer
