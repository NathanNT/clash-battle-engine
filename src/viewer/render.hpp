#pragma once

#include "assets.hpp"
#include "projectile_visuals.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include <vector>

namespace cocsim::viewer {

const ArmySlot* army_slot(const std::vector<ArmySlot>& army, Kind kind) {
  for (const auto& slot : army) if (slot.kind == kind) return &slot;
  return nullptr;
}

const SpellSlot* spell_slot(const std::vector<SpellSlot>& spells, SpellKind kind) {
  for (const auto& slot : spells) if (slot.kind == kind) return &slot;
  return nullptr;
}

struct RenderSelection {
  std::vector<EntityView> entities;
  std::size_t attackers_total{};
  std::size_t attackers_omitted{};
};

RenderSelection select_render_entities(const std::vector<EntityView>& all, std::size_t attacker_budget) {
  RenderSelection result;
  result.entities.reserve(std::min(all.size(), attacker_budget + 128));
  // Defenders stay visible because they describe the village.  Attackers are
  // sampled in their deterministic entity-id order: the core keeps simulating
  // every troop, while the viewer bounds the amount of per-frame raster work.
  for (const auto& entity : all) {
    if (entity.side == Side::Defender) result.entities.push_back(entity);
    else ++result.attackers_total;
  }
  std::size_t included_attackers = 0;
  for (const auto& entity : all) if (entity.side == Side::Attacker) {
    if (included_attackers++ < attacker_budget) result.entities.push_back(entity);
  }
  result.attackers_omitted = result.attackers_total > attacker_budget ? result.attackers_total - attacker_budget : 0;
  return result;
}

struct HeatmapCache {
  struct Source {
    EntityId id{};
    Kind kind{};
    int level{};
    Vec2 position{};
    double hp{};
    bool supercharged{};
    bool operator==(const Source& other) const {
      return id == other.id && kind == other.kind && level == other.level
        && position.x == other.position.x && position.y == other.position.y
        && hp == other.hp && supercharged == other.supercharged;
    }
  };

  bool air{};
  int deployed_housing{};
  std::vector<Source> sources;
  std::array<float, kHomeVillageTotalTiles * kHomeVillageTotalTiles> intensity{};

  void update(const GameData& data, const std::vector<EntityView>& entities, int housing, bool requested_air) {
    std::vector<Source> next_sources;
    next_sources.reserve(entities.size());
    for (const auto& entity : entities) if (entity.side == Side::Defender)
      next_sources.push_back({entity.id, entity.kind, entity.level, entity.position, entity.hp, entity.supercharged});
    if (air == requested_air && deployed_housing == housing && sources == next_sources) return;
    air = requested_air;
    deployed_housing = housing;
    sources = std::move(next_sources);
    intensity.fill(0.0f);
    for (const auto& source : sources) {
      const auto* stats = data.find(source.kind, source.level, source.supercharged ? "supercharged" : "normal");
      if (!stats || stats->dps <= 0 || stats->activation_housing_space > deployed_housing) continue;
      const bool can_hit = air ? stats->target_type != TargetType::Ground : stats->target_type != TargetType::Air;
      if (!can_hit) continue;
      const int min_x = std::max(0, int(std::floor(source.position.x - stats->range)));
      const int max_x = std::min(kHomeVillageTotalTiles - 1, int(std::ceil(source.position.x + stats->range)));
      const int min_y = std::max(0, int(std::floor(source.position.y - stats->range)));
      const int max_y = std::min(kHomeVillageTotalTiles - 1, int(std::ceil(source.position.y + stats->range)));
      for (int y = min_y; y <= max_y; ++y) for (int x = min_x; x <= max_x; ++x) {
        const auto distance = std::hypot(double(x) + .5 - source.position.x, double(y) + .5 - source.position.y);
        if (distance >= stats->min_range && distance <= stats->range)
          intensity[static_cast<std::size_t>(y * kHomeVillageTotalTiles + x)] += float(stats->dps);
      }
    }
  }
};

// A compact three-band disc is deliberately cheaper than a per-pixel circle:
// the viewer can keep its presentation budget with a dense projectile volley.
// It consumes only an already observed position and has no Core dependency.
void draw_projectile_orb(SDL_Renderer* renderer, float center_x, float center_y, float diameter) {
  const float radius = diameter / 2.0f;
  const float cap_height = std::max(1.0f, diameter * .24f);
  const float cap_inset = diameter * .21f;
  SDL_FRect top{center_x - radius + cap_inset, center_y - radius, diameter - 2.0f * cap_inset, cap_height};
  SDL_FRect middle{center_x - radius, center_y - radius + cap_height, diameter, diameter - 2.0f * cap_height};
  SDL_FRect bottom{center_x - radius + cap_inset, center_y + radius - cap_height, diameter - 2.0f * cap_inset, cap_height};
  SDL_RenderFillRect(renderer, &top);
  SDL_RenderFillRect(renderer, &middle);
  SDL_RenderFillRect(renderer, &bottom);
}

void draw_scene(SDL_Renderer* renderer, const Scenario& scenario, const GameData& data, Images& images, const std::vector<EntityView>& entities, const std::vector<SpellEffectView>& spell_effects, const std::vector<DeathExplosionView>& death_explosions, const std::vector<ProjectileView>& projectiles, const std::vector<ProjectileTrace>& traces, std::uint64_t wall_now_ms, const HeatmapCache& heatmap_cache, float origin_x, float origin_y, float zoom, bool ranges, bool heatmap, bool show_projectiles, EntityId selected_defender) {
  const float ox = origin_x, oy = origin_y;
  SDL_SetRenderDrawColor(renderer, 24, 29, 37, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 45, 91, 54, 255);
  SDL_FRect build{ox, oy, kHomeVillageTotalTiles * zoom, kHomeVillageTotalTiles * zoom};
  SDL_RenderFillRect(renderer, &build);
  SDL_SetRenderDrawColor(renderer, 59, 78, 68, 255);
  for (int i = 0; i <= kHomeVillageTotalTiles; ++i) {
    SDL_RenderLine(renderer, ox + i * zoom, oy, ox + i * zoom, oy + kHomeVillageTotalTiles * zoom);
    SDL_RenderLine(renderer, ox, oy + i * zoom, ox + kHomeVillageTotalTiles * zoom, oy + i * zoom);
  }
  if (heatmap) {
    // Values are cached until a defender or Eagle activation state changes.
    // Rasterisation is therefore O(tiles), never O(tiles * defenders), every
    // presentation frame.
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int y = 0; y < kHomeVillageTotalTiles; ++y) for (int x = 0; x < kHomeVillageTotalTiles; ++x) {
      const auto dps = heatmap_cache.intensity[static_cast<std::size_t>(y * kHomeVillageTotalTiles + x)];
      if (dps <= 0) continue;
      const float intensity = float(1.0 - std::exp(-dps / 250.0));
      SDL_SetRenderDrawColor(renderer, Uint8(235 * intensity), Uint8(70 * (1.0f - intensity)), 35, Uint8(150 * intensity));
      SDL_FRect cell{ox + x * zoom, oy + y * zoom, zoom, zoom};
      SDL_RenderFillRect(renderer, &cell);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  for (const auto& effect : spell_effects) {
    const SDL_Color colour = effect.kind == SpellKind::Rage ? SDL_Color{190, 70, 240, 85}
      : effect.kind == SpellKind::Heal ? SDL_Color{80, 230, 110, 85}
      : effect.kind == SpellKind::Haste ? SDL_Color{245, 145, 35, 85}
      : effect.kind == SpellKind::Jump ? SDL_Color{245, 220, 45, 85}
      : SDL_Color{90, 190, 255, 95};
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    for (int y = std::max(0, int(std::floor(effect.position.y - effect.radius))); y < std::min(kHomeVillageTotalTiles, int(std::ceil(effect.position.y + effect.radius))); ++y)
      for (int x = std::max(0, int(std::floor(effect.position.x - effect.radius))); x < std::min(kHomeVillageTotalTiles, int(std::ceil(effect.position.x + effect.radius))); ++x)
        if (std::hypot(x + .5 - effect.position.x, y + .5 - effect.position.y) <= effect.radius) {
          SDL_FRect cell{ox + x * zoom, oy + y * zoom, zoom, zoom};
          SDL_RenderFillRect(renderer, &cell);
        }
  }
  // Read-only telegraph of an already queued Core effect.  The Viewer never
  // predicts or applies this damage; only its exact T+ms impact tick can.
  for (const auto& effect : death_explosions) {
    SDL_SetRenderDrawColor(renderer, 245, 125, 35, 95);
    for (int y = std::max(0, int(std::floor(effect.position.y - effect.radius))); y < std::min(kHomeVillageTotalTiles, int(std::ceil(effect.position.y + effect.radius))); ++y)
      for (int x = std::max(0, int(std::floor(effect.position.x - effect.radius))); x < std::min(kHomeVillageTotalTiles, int(std::ceil(effect.position.x + effect.radius))); ++x)
        if (std::hypot(x + .5 - effect.position.x, y + .5 - effect.position.y) <= effect.radius) {
          SDL_FRect cell{ox + x * zoom, oy + y * zoom, zoom, zoom};
          SDL_RenderFillRect(renderer, &cell);
        }
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  // The Hero Banner is source-marked non-targetable and has no hit points.
  // Render it from the immutable scenario rather than fabricating an Entity.
  for (const auto& obstacle : scenario.non_combat_obstacles) {
    const auto* meta = data.find_non_combat(obstacle.kind);
    if (!meta) continue;
    const float width = meta->footprint_width * zoom, height = meta->footprint_height * zoom;
    SDL_FRect destination{ox + float(obstacle.position.x) * zoom - width / 2, oy + float(obstacle.position.y) * zoom - height / 2, width, height};
    SDL_SetRenderDrawColor(renderer, 220, 85, 70, 255);
    SDL_FRect blocked{destination.x - meta->deployment_margin * zoom, destination.y - meta->deployment_margin * zoom,
                      width + 2 * meta->deployment_margin * zoom, height + 2 * meta->deployment_margin * zoom};
    SDL_RenderRect(renderer, &blocked);
    if (const auto* asset = images.find_asset(obstacle.kind, 1, obstacle.variant)) SDL_RenderTexture(renderer, asset->texture, &asset->source, &destination);
    else { SDL_SetRenderDrawColor(renderer, 125, 100, 170, 255); SDL_RenderFillRect(renderer, &destination); }
    SDL_SetRenderDrawColor(renderer, 236, 188, 62, 255);
    SDL_RenderRect(renderer, &destination);
  }
  // Red outlines mark the one-tile no-deployment margin around each building.
  // A revealed trap is still not a building and remains deployable terrain.
  for (const auto& entity : entities) if (entity.side == Side::Defender && entity.category != EntityCategory::Trap) {
    SDL_SetRenderDrawColor(renderer, 220, 85, 70, 255);
    SDL_FRect blocked{ox + float(entity.position.x - entity.footprint_width / 2.0 - 1.0) * zoom,
                      oy + float(entity.position.y - entity.footprint_height / 2.0 - 1.0) * zoom,
                      float(entity.footprint_width + 2) * zoom, float(entity.footprint_height + 2) * zoom};
    SDL_RenderRect(renderer, &blocked);
  }
  // Join adjacent wall cells with an underlay, keeping the connection obvious
  // even at a zoom where an individual wall sprite has transparent padding.
  // A tile lookup replaces the former quadratic pair scan for large villages.
  SDL_SetRenderDrawColor(renderer, 105, 70, 40, 255);
  std::array<std::array<bool, kHomeVillageTotalTiles>, kHomeVillageTotalTiles> walls{};
  for (const auto& entity : entities) if (entity.kind == Kind::Wall) {
    const int x = int(std::floor(entity.position.x)), y = int(std::floor(entity.position.y));
    if (x >= 0 && x < kHomeVillageTotalTiles && y >= 0 && y < kHomeVillageTotalTiles) walls[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = true;
  }
  for (int y = 0; y < kHomeVillageTotalTiles; ++y) for (int x = 0; x < kHomeVillageTotalTiles; ++x) if (walls[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)]) {
    if (x + 1 < kHomeVillageTotalTiles && walls[static_cast<std::size_t>(y)][static_cast<std::size_t>(x + 1)]) {
      SDL_FRect connector{ox + (float(x) + .5f) * zoom, oy + (float(y) + .5f) * zoom - zoom * .28f, zoom, zoom * .56f};
      SDL_RenderFillRect(renderer, &connector);
    }
    if (y + 1 < kHomeVillageTotalTiles && walls[static_cast<std::size_t>(y + 1)][static_cast<std::size_t>(x)]) {
      SDL_FRect connector{ox + (float(x) + .5f) * zoom - zoom * .28f, oy + (float(y) + .5f) * zoom, zoom * .56f, zoom};
      SDL_RenderFillRect(renderer, &connector);
    }
  }
  for (const auto& entity : entities) {
    const float width = entity.footprint_width * zoom, height = entity.footprint_height * zoom;
    SDL_FRect destination{ox + float(entity.position.x) * zoom - width / 2, oy + float(entity.position.y) * zoom - height / 2, width, height};
    // The amber frame is the exact collision/deployment footprint, independent
    // of the cosmetic sprite silhouette.
    if (entity.side == Side::Defender && (entity.footprint_width > 1 || entity.footprint_height > 1)) {
      SDL_SetRenderDrawColor(renderer, 236, 188, 62, 255);
      SDL_RenderRect(renderer, &destination);
    }
    if (entity.side == Side::Defender && entity.id == selected_defender) {
      // This outline uses the Core collision rectangle, never alpha bounds or
      // a GUI-side hitbox.  Selection is visual-only and cannot change state.
      SDL_SetRenderDrawColor(renderer, 85, 220, 255, 255);
      SDL_RenderRect(renderer, &destination);
    }
    const std::string variant = entity.supercharged ? "supercharged" : "normal";
    if (entity.underground) {
      // This is intentionally a state marker rather than a fabricated sprite:
      // underground Miners are non-targetable and bypass walls in cocsim_core.
      SDL_SetRenderDrawColor(renderer, 82, 55, 33, 230);
      SDL_FRect tunnel{destination.x + width * .2f, destination.y + height * .4f, width * .6f, height * .2f};
      SDL_RenderFillRect(renderer, &tunnel);
    } else if (const auto* asset = images.find_asset(entity.kind, entity.level, variant)) SDL_RenderTexture(renderer, asset->texture, &asset->source, &destination);
    else { SDL_SetRenderDrawColor(renderer, entity.side == Side::Attacker ? 224 : 205, entity.side == Side::Attacker ? 155 : 75, 70, 255); SDL_RenderFillRect(renderer, &destination); }
    if (entity.invisible_to_defenses) {
      // Core exposes the exact remaining Cloak state; the cyan frame is a
      // presentation-only indicator and cannot affect targeting or timing.
      SDL_SetRenderDrawColor(renderer, 75, 225, 245, 255);
      SDL_RenderRect(renderer, &destination);
    }
    if (entity.deployment_rage_active) {
      // The amber marker is a read-only Core projection of the Super
      // Barbarian's serialized deployment window. It neither advances nor
      // prolongs the rule timer, including at non-x1 playback speeds.
      SDL_SetRenderDrawColor(renderer, 255, 174, 50, 255);
      SDL_RenderRect(renderer, &destination);
    }
    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_FRect hp{destination.x, destination.y - 5, width, 3};
    SDL_RenderFillRect(renderer, &hp);
    SDL_SetRenderDrawColor(renderer, 60, 220, 95, 255);
    hp.w = float(width * entity.hp / entity.max_hp);
    SDL_RenderFillRect(renderer, &hp);
    if ((ranges || entity.id == selected_defender) && entity.side == Side::Defender && entity.attack_range > 0) {
      const float radius = float(entity.attack_range * zoom);
      SDL_SetRenderDrawColor(renderer, 220, 90, 90, 255);
      SDL_FRect range{destination.x + width / 2 - radius, destination.y + height / 2 - radius, 2 * radius, 2 * radius};
      SDL_RenderRect(renderer, &range);
    }
  }
  if (show_projectiles) for (const auto& trace : traces) {
    // Amber/green travel is a clearly cosmetic post-impact replay of an
    // unmeasured logical shot, never an inferred flight rule. Violet remains
    // the documented immediate Inferno beam; cyan moving orbs below are Core
    // projectiles with sourced flight.
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const float progress = cosmetic_projectile_progress(trace, wall_now_ms);
    const Vec2 displayed{
      trace.origin.x + (trace.impact.x - trace.origin.x) * progress,
      trace.origin.y + (trace.impact.y - trace.origin.y) * progress,
    };
    const SDL_Color trace_colour = trace.kind == ProjectileTraceKind::Beam ? SDL_Color{223, 118, 255, 190}
      : trace.kind == ProjectileTraceKind::Healing ? SDL_Color{91, 232, 155, 125}
      : trace.kind == ProjectileTraceKind::SourcedImpact ? SDL_Color{105, 225, 255, 155}
      : SDL_Color{255, 184, 70, 105};
    SDL_SetRenderDrawColor(renderer, trace_colour.r, trace_colour.g, trace_colour.b, trace_colour.a);
    SDL_RenderLine(renderer, ox + float(trace.origin.x) * zoom, oy + float(trace.origin.y) * zoom,
                   ox + float(displayed.x) * zoom, oy + float(displayed.y) * zoom);
    if (trace.kind != ProjectileTraceKind::Beam) {
      const SDL_Color orb_colour = trace.kind == ProjectileTraceKind::Healing
        ? SDL_Color{142, 255, 191, 220} : trace.kind == ProjectileTraceKind::SourcedImpact
        ? SDL_Color{155, 245, 255, 235} : SDL_Color{255, 220, 120, 205};
      SDL_SetRenderDrawColor(renderer, orb_colour.r, orb_colour.g, orb_colour.b, orb_colour.a);
      const float diameter = std::clamp(zoom * .25f, 3.0f, 7.0f);
      draw_projectile_orb(renderer, ox + float(displayed.x) * zoom, oy + float(displayed.y) * zoom, diameter);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  }
  if (show_projectiles) for (const auto& projectile : projectiles) {
    // The line and moving orb consume the read-only Core projection.  The
    // Viewer deliberately has no projectile update code: pausing, drawing at
    // 30 FPS or interpolating a frame cannot change hit timing or damage.
    // Provenance chooses the colour, not homing: a future sourced
    // non-homing trajectory must remain cyan without changing its flight rule.
    const SDL_Color colour = projectile.speed_sourced ? SDL_Color{105, 225, 255, 235} : SDL_Color{255, 225, 115, 190};
    // The complete target endpoint is part of the read-only Core projection.
    // It therefore remains available even when the target sprite is omitted by
    // the adaptive renderer, without performing a GUI-side combat lookup.
    const Vec2 endpoint = projectile.target_position;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, Uint8(colour.a / 4));
    SDL_RenderLine(renderer, ox + float(projectile.origin.x) * zoom, oy + float(projectile.origin.y) * zoom,
                   ox + float(endpoint.x) * zoom, oy + float(endpoint.y) * zoom);
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, Uint8(colour.a / 2));
    SDL_RenderLine(renderer, ox + float(projectile.origin.x) * zoom, oy + float(projectile.origin.y) * zoom,
                   ox + float(projectile.position.x) * zoom, oy + float(projectile.position.y) * zoom);
    const float diameter = std::clamp(zoom * .34f, 4.0f, 9.0f);
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    draw_projectile_orb(renderer, ox + float(projectile.position.x) * zoom, oy + float(projectile.position.y) * zoom, diameter);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  }
}

} // namespace cocsim::viewer
