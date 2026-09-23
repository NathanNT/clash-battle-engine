#pragma once

#include "cocsim/core.hpp"

namespace cocsim {

struct BattleState::Entity { EntityId id{}; Kind kind{}; Side side{}; int level{}; Vec2 pos{}; double hp{}, max_hp{}; double damage{}, healing{}, death_damage{}, death_splash_radius{}, trigger_range{}, range{}, min_range{}, speed{}, radius{}, splash_radius{}, yeti_damage_since_spawn{}; Milliseconds cooldown{}, next_action{}, next_path{}, time_between_bursts{}, death_damage_delay{}, invisible_to_defenses_until{}, inferno_lock_elapsed{}, next_aura_action{}, deployment_rage_until{}, druid_transform_at{}; EntityId inferno_lock_target{}; int activation_housing{}, shots_per_burst{1}, shots_in_burst{}, earthquake_hits{}, multi_target_count{}, opening_long_shots_remaining{}, remaining_yetimites{}; bool ranged{}, splash{}, flying{}, heals{}, concealed{}, supercharged{}, underground{}; TargetFocus target_focus{TargetFocus::Any}; TargetType target_type{TargetType::Ground}; bool defenses{}; std::optional<EntityId> target; std::optional<Vec2> waypoint; };
struct BattleState::Projectile { ProjectileId id{}; EntityId owner{}, target{}; double damage{}, splash_radius{}; Vec2 origin{}, position{}; double speed_tiles_per_second{}; Milliseconds launched{}, impact{}; bool homing{}; Vec2 endpoint{}; bool piercing{}; bool fixed_impact{}; };
struct BattleState::DeathExplosion { EntityId owner{}; Vec2 position{}; Side side{}; double damage{}, radius{}; Milliseconds impact{}; bool ground_only{}; };
struct BattleState::SpellEffect { SpellKind kind{}; Vec2 pos{}; double radius{}; double damage_multiplier{1.0}, speed_multiplier{1.0}, healing_per_pulse{}; Milliseconds expires{}, next_pulse{}, pulse_interval{}; int pulses{}; };

} // namespace cocsim
