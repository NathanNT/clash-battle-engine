#include "cocsim/core.hpp"
#include "detail/battle_state_internal.hpp"
#include "detail/geometry.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace cocsim {

using detail::dist;

void BattleState::queue_spawn(Kind kind, EntityId parent, int level, Vec2 position, Side side) {
  if(data_.find(kind,level)) pending_spawns_.push_back({kind,parent,level,position,side});
}
void BattleState::queue_golemites(EntityId parent, int level, Vec2 position, Side side) {
  const auto* stats=data_.find(Kind::Golemite,level);
  if(!stats || stats->parent_spawn_count<=0) return;
  for(int i=0;i<stats->parent_spawn_count;++i) queue_spawn(Kind::Golemite,parent,level,position,side);
}
void BattleState::queue_super_hog_rider_subunits(EntityId parent, int level, Vec2 position, Side side) {
  // Supercell establishes the split but not its exact client-ordering or
  // launch coordinates. Both children therefore enter through the shared
  // deterministic spawn queue at the destroyed parent's logical position.
  // The documented visual launch remains an explicit fidelity gap.
  queue_spawn(Kind::SuperHog,parent,level,position,side);
  queue_spawn(Kind::SuperRider,parent,level,position,side);
}
void BattleState::queue_ice_pups(EntityId parent, const Stats& stats, Vec2 position, Side side) {
  // Public sources fix the count but not delay or individual coordinates.
  // The shared value-only queue keeps that missing visual contract explicit.
  for(int i=0;i<stats.spawned_units;++i) queue_spawn(Kind::IcePup,parent,1,position,side);
}
void BattleState::queue_lava_pups(EntityId parent, const Stats& stats, Vec2 position, Side side) {
  // The public description says circular spread but supplies neither timing nor
  // coordinates. The deterministic queue retains that gap explicitly.
  for(int i=0;i<stats.spawned_units;++i) queue_spawn(Kind::LavaPup,parent,1,position,side);
}
void BattleState::queue_yetimites(EntityId parent, int level, int count, Vec2 position, Side side) {
  // The dated public table maps child level directly to the parent level. The
  // source does not publish individual launch positions or a delay, so requests
  // enter the existing stable logical queue at the parent position.
  for (int i = 0; i < count; ++i) queue_spawn(Kind::Yetimite, parent, level, position, side);
}
void BattleState::queue_trap_skeletons(EntityId parent, const Stats& stats, Vec2 position, Side side, TargetType target_type) {
  if(stats.spawned_units<=0||stats.spawned_unit_level<=0) return;
  const auto kind=target_type==TargetType::Air?Kind::TrapSkeletonAir:Kind::TrapSkeletonGround;
  // Public tables state that skeletons materialize around the trap but do not
  // give coordinates.  The shared logical origin is deterministic; the missing
  // visual/collision geometry is documented as an open fidelity gap.
  for(int i=0;i<stats.spawned_units;++i) queue_spawn(kind,parent,stats.spawned_unit_level,position,side);
}
void BattleState::resolve_spawns() {
  for(const auto& request:pending_spawns_) {
    const auto* st=data_.find(request.kind,request.level);
    if(!st) continue;
    Entity e; e.id=static_cast<EntityId>(next_id_++); e.kind=request.kind; e.side=request.side; e.level=request.level; e.pos=request.position;
    e.hp=e.max_hp=st->hp; e.damage=st->damage; e.healing=st->healing_per_second; e.death_damage=st->death_damage; e.death_splash_radius=st->death_splash_radius; e.death_damage_delay=st->death_damage_delay;
    e.trigger_range=st->trigger_range; e.cooldown=st->cooldown; e.range=st->range; e.min_range=st->min_range; e.speed=st->movement_tiles_per_second; e.radius=st->radius;
    e.splash_radius=st->splash_radius; e.time_between_bursts=st->time_between_bursts; e.shots_per_burst=st->shots_per_burst; e.opening_long_shots_remaining=st->opening_long_shot_count; e.activation_housing=st->activation_housing_space; e.next_aura_action=st->aura_cooldown>0?now_ms_+st->aura_cooldown:0; e.druid_transform_at=st->druid_transform_after>0?now_ms_+st->druid_transform_after:0;
    e.ranged=st->ranged; e.splash=st->splash; e.flying=st->flying; e.heals=st->heals; e.target_focus=st->target_focus; e.target_type=st->target_type; e.defenses=st->target_focus==TargetFocus::Defenses;
    if (request.kind == Kind::Yeti) e.remaining_yetimites = st->spawned_units;
    entities_.push_back(e); emit(EventType::Deployed,e.id,request.parent,0,to_string(request.kind)+" spawned");
  }
  pending_spawns_.clear();
}
} // namespace cocsim
