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

bool BattleState::isolated_from_allied_flying(const Entity& actor,const Stats& stats) const {
  return stats.isolation_radius>0.0&&std::none_of(entities_.begin(),entities_.end(),[&](const Entity& ally){
    return ally.id!=actor.id&&alive(ally)&&ally.side==actor.side&&ally.flying&&dist(actor.pos,ally.pos)<=stats.isolation_radius;
  });
}
double BattleState::scaled_damage(EntityId actor,EntityId target,double amount) const { const auto* a=entity(actor); const auto* t=entity(target); if(!a||!t) return amount; const auto* attacker_stats=data_.find(a->kind,a->level); const auto* target_stats=data_.find(t->kind,t->level); if(!attacker_stats||!target_stats) return amount; if(attacker_stats->resource_damage_multiplier!=1.0&&(target_stats->category==EntityCategory::Resource||t->kind==Kind::TownHall)) amount*=attacker_stats->resource_damage_multiplier; if(attacker_stats->defense_damage_multiplier!=1.0&&target_stats->category==EntityCategory::Defense) amount*=attacker_stats->defense_damage_multiplier; if(attacker_stats->wall_damage_multiplier!=1.0&&target_stats->category==EntityCategory::Wall) amount*=attacker_stats->wall_damage_multiplier; if(attacker_stats->category==EntityCategory::Trap&&t->kind==Kind::DragonDuke&&target_stats->rage_trap_damage_multiplier!=1.0&&isolated_from_allied_flying(*t,*target_stats)) amount*=target_stats->rage_trap_damage_multiplier; return amount; }
void BattleState::damage(EntityId actor, EntityId target_id, double amount) {
  auto* target = entity(target_id);
  if (!target || !alive(*target)) return;
  if (const auto* attacker = entity(actor); attacker && !can_attack_target(*attacker, *target)) return;
  amount = scaled_damage(actor, target_id, amount);
  target->hp = std::max(0.0, target->hp - amount);
  emit(EventType::Damaged, actor, target_id, amount, "damage");
  if (target->kind == Kind::Yeti && target->remaining_yetimites > 0) {
    const auto* yeti_stats = data_.find(target->kind, target->level);
    if (yeti_stats && yeti_stats->damage_spawn_threshold > 0.0) {
      target->yeti_damage_since_spawn += amount;
      const int spawned = std::min(target->remaining_yetimites,
        static_cast<int>(std::floor(target->yeti_damage_since_spawn / yeti_stats->damage_spawn_threshold)));
      if (spawned > 0) {
        target->yeti_damage_since_spawn -= spawned * yeti_stats->damage_spawn_threshold;
        target->remaining_yetimites -= spawned;
        queue_yetimites(target_id, target->level, spawned, target->pos, target->side);
      }
    }
  }
  if (target->hp != 0) return;

  const auto death_pos = target->pos;
  const auto death_side = target->side;
  const auto death_damage = target->death_damage;
  const auto death_radius = target->death_splash_radius;
  const auto death_delay = target->death_damage_delay;
  const auto death_kind = target->kind;
  const auto death_level = target->level;
  const auto remaining_yetimites = target->remaining_yetimites;
  const auto* death_stats = data_.find(death_kind, death_level);
  const bool ground_only = death_stats && death_stats->death_damage_ground_only;
  const auto death_freeze_radius = death_stats ? death_stats->death_freeze_radius : 0.0;
  const auto death_freeze_duration = death_stats ? death_stats->death_freeze_duration : 0;
  // The documented Spring Trap exception applies to an ejection. A regular
  // non-ejecting Spring hit retains the published human-form defeat route.
  const auto* actor_stats = actor ? entity(actor) : nullptr;
  const bool spring_ejection = actor_stats&&actor_stats->kind==Kind::SpringTrap
    &&amount>=target->max_hp;
  if(death_kind==Kind::Druid&&death_stats&&death_stats->druid_transforms_on_death
      &&!spring_ejection) {
    queue_spawn(Kind::DruidBear,target_id,death_level,death_pos,death_side);
    emit(EventType::TargetChanged,target_id,0,0,"druid transforms on human defeat");
    return;
  }
  emit(EventType::Destroyed, actor, target_id, 0, to_string(death_kind));

  if (death_damage > 0 && death_radius > 0) {
    if (death_delay > 0) {
      death_explosions_.push_back({target_id, death_pos, death_side, death_damage,
                                   death_radius, now_ms_ + death_delay, ground_only});
    } else {
      for (auto& victim : entities_) {
        if (alive(victim) && !victim.underground && victim.side != death_side
            && (!ground_only || !victim.flying) && dist(victim.pos, death_pos) <= death_radius) {
          damage(target_id, victim.id, death_damage);
        }
      }
      emit(EventType::Impact, target_id, 0, death_damage, "death damage");
    }
  }
  if (death_freeze_radius > 0.0 && death_freeze_duration > 0 && death_side == Side::Attacker) {
    SpellEffect effect;
    effect.kind = SpellKind::Freeze;
    effect.pos = death_pos;
    effect.radius = death_freeze_radius;
    effect.expires = now_ms_ + death_freeze_duration;
    spell_effects_.push_back(effect);
    emit(EventType::Impact, target_id, 0, static_cast<double>(death_freeze_duration), "ice golem death freeze");
  }
  if (death_kind == Kind::Golem) queue_golemites(target_id, death_level, death_pos, death_side);
  if (death_kind == Kind::SuperHogRider) queue_super_hog_rider_subunits(target_id, death_level, death_pos, death_side);
  if (death_kind == Kind::IceHound && death_stats) queue_ice_pups(target_id, *death_stats, death_pos, death_side);
  if (death_kind == Kind::LavaHound && death_stats && death_side == Side::Attacker) queue_lava_pups(target_id, *death_stats, death_pos, death_side);
  if (death_kind == Kind::Yeti && remaining_yetimites > 0) queue_yetimites(target_id, death_level, remaining_yetimites, death_pos, death_side);
}
} // namespace cocsim

