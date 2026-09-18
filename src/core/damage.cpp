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

double BattleState::scaled_damage(EntityId actor,EntityId target,double amount) const { const auto* a=entity(actor); const auto* t=entity(target); if(!a||!t) return amount; const auto* attacker_stats=data_.find(a->kind,a->level); const auto* target_stats=data_.find(t->kind,t->level); if(!attacker_stats||!target_stats) return amount; if(attacker_stats->resource_damage_multiplier!=1.0&&(target_stats->category==EntityCategory::Resource||t->kind==Kind::TownHall)) amount*=attacker_stats->resource_damage_multiplier; if(attacker_stats->wall_damage_multiplier!=1.0&&target_stats->category==EntityCategory::Wall) amount*=attacker_stats->wall_damage_multiplier; return amount; }
void BattleState::damage(EntityId a,EntityId t,double amount) { auto* target=entity(t); if(!target||!alive(*target)) return; amount=scaled_damage(a,t,amount); target->hp=std::max(0.0,target->hp-amount);emit(EventType::Damaged,a,t,amount,"damage");if(target->hp==0) { const auto death_pos=target->pos; const auto death_side=target->side; const auto death_damage=target->death_damage; const auto death_radius=target->death_splash_radius; const auto death_delay=target->death_damage_delay; const auto death_kind=target->kind; const auto death_level=target->level; const auto* death_stats=data_.find(death_kind,death_level); const bool ground_only=death_stats&&death_stats->death_damage_ground_only; emit(EventType::Destroyed,a,t,0,to_string(death_kind)); if(death_damage>0&&death_radius>0) { if(death_delay>0) death_explosions_.push_back({t,death_pos,death_side,death_damage,death_radius,now_ms_+death_delay,ground_only}); else { for(auto& victim:entities_) if(alive(victim)&&!victim.underground&&victim.side!=death_side&&(!ground_only||!victim.flying)&&dist(victim.pos,death_pos)<=death_radius) damage(t,victim.id,death_damage); emit(EventType::Impact,t,0,death_damage,"death damage"); } } if(death_kind==Kind::Golem) queue_golemites(t,death_level,death_pos,death_side); } }
} // namespace cocsim

