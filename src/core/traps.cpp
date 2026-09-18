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

void BattleState::update_traps() {
  for(auto& trap:entities_) {
    if(!alive(trap)||!trap.concealed||(trap.kind!=Kind::Bomb&&trap.kind!=Kind::GiantBomb&&trap.kind!=Kind::AirBomb&&trap.kind!=Kind::SeekingAirMine&&trap.kind!=Kind::SpringTrap&&trap.kind!=Kind::GigaBomb&&trap.kind!=Kind::TornadoTrap&&trap.kind!=Kind::SkeletonTrap)||(trap.activation_housing>0&&deployed_housing_<trap.activation_housing)) continue;
    const Entity* trigger=nullptr;
    for(const auto& candidate:entities_) {
      const bool altitude_matches=trap.kind==Kind::GigaBomb||trap.kind==Kind::TornadoTrap
        ||(trap.kind==Kind::SkeletonTrap?(trap.target_type==TargetType::Air?candidate.flying:!candidate.flying)
           :((trap.kind==Kind::AirBomb||trap.kind==Kind::SeekingAirMine)?candidate.flying:!candidate.flying));
      if(!(alive(candidate)&&candidate.side==Side::Attacker&&!candidate.underground&&altitude_matches&&dist(trap.pos,candidate.pos)<=trap.trigger_range)) continue;
      const auto* current_stats=data_.find(candidate.kind,candidate.level);
      const auto* trigger_stats=trigger?data_.find(trigger->kind,trigger->level):nullptr;
      const int capacity=current_stats?current_stats->housing_space:0, best_capacity=trigger_stats?trigger_stats->housing_space:0;
      const bool nearer=!trigger||dist(trap.pos,candidate.pos)<dist(trap.pos,trigger->pos)||(dist(trap.pos,candidate.pos)==dist(trap.pos,trigger->pos)&&candidate.id<trigger->id);
      const bool better=trap.kind==Kind::SpringTrap?(capacity>best_capacity||(capacity==best_capacity&&nearer)):nearer;
      if(better) trigger=&candidate;
    }
    if(!trigger) continue;
    trap.concealed=false;
    emit(EventType::TargetChanged,trap.id,trigger->id,trap.trigger_range,"trap triggered");
    if(trap.kind==Kind::SeekingAirMine) {
      const auto* stats=data_.find(trap.kind,trap.level);
      // Supercell publishes this trap's 3.5-tile/s projectile speed.  Its
      // position therefore belongs to the deterministic Core state rather
      // than being a GUI animation or an immediate damage shortcut.
      if(stats&&stats->projectile_speed_tiles_per_second>0) {
        const auto projectile_id=next_projectile_id_++;
        projectiles_.push_back({projectile_id,trap.id,trigger->id,trap.damage,0.0,trap.pos,trap.pos,stats->projectile_speed_tiles_per_second,now_ms_,0,true});
        emit(EventType::Projectile,trap.id,trigger->id,trap.damage,"seeking air mine launched",projectile_id);
        trap.hp=0;
        emit(EventType::Destroyed,trap.id,trap.id,0,to_string(trap.kind));
        continue;
      }
    }
    if(trap.kind==Kind::TornadoTrap) {
      const auto* stats=data_.find(trap.kind,trap.level);
      if(!stats||stats->effect_duration<=0) { trap.hp=0; emit(EventType::Destroyed,trap.id,trap.id,0,to_string(trap.kind)); continue; }
      // The imported total is applied uniformly on the fixed rule ticks. The
      // public sources do not specify a deterministic vortex trajectory, so
      // displacement deliberately remains outside this verified submodel.
      trap.next_action=now_ms_+stats->effect_duration;
      emit(EventType::Impact,trap.id,trigger->id,trap.damage,"tornado trap activated");
      continue;
    }
    if(trap.kind==Kind::SkeletonTrap) {
      const auto* stats=data_.find(trap.kind,trap.level);
      if(stats) queue_trap_skeletons(trap.id,*stats,trap.pos,trap.side,trap.target_type);
      trap.hp=0;
      emit(EventType::Impact,trap.id,trigger->id,stats?stats->spawned_units:0,"skeleton trap spawns skeletons");
      emit(EventType::Destroyed,trap.id,trap.id,0,to_string(trap.kind));
      continue;
    }
    std::vector<EntityId> victims{trigger->id};
    if(trap.kind==Kind::AirBomb) {
      victims.clear();
      for(const auto& candidate:entities_) if(alive(candidate)&&candidate.side==Side::Attacker&&candidate.flying&&dist(candidate.pos,trigger->pos)<=trap.splash_radius) victims.push_back(candidate.id);
      std::sort(victims.begin(),victims.end());
    }
    if(trap.kind==Kind::Bomb||trap.kind==Kind::GiantBomb) {
      victims.clear();
      for(const auto& candidate:entities_) if(alive(candidate)&&candidate.side==Side::Attacker&&!candidate.flying&&dist(candidate.pos,trap.pos)<=trap.splash_radius) victims.push_back(candidate.id);
      std::sort(victims.begin(),victims.end());
    }
    if(trap.kind==Kind::GigaBomb) {
      victims.clear();
      for(const auto& candidate:entities_) if(alive(candidate)&&candidate.side==Side::Attacker&&dist(candidate.pos,trap.pos)<=trap.splash_radius) victims.push_back(candidate.id);
      std::sort(victims.begin(),victims.end());
    }
    if(trap.kind==Kind::SpringTrap) {
      const auto* trap_stats=data_.find(trap.kind,trap.level);
      const auto* victim_stats=data_.find(trigger->kind,trigger->level);
      const bool eject=victim_stats&&trap_stats&&victim_stats->housing_space<=trap_stats->spring_capacity;
      damage(trap.id,trigger->id,eject?trigger->max_hp:trap.damage);
      emit(EventType::Impact,trap.id,trigger->id,eject?trigger->max_hp:trap.damage,eject?"spring trap ejects":"spring trap damage");
    } else for(const auto victim:victims) damage(trap.id,victim,trap.damage);
    trap.hp=0;
    if(trap.kind!=Kind::SpringTrap) emit(EventType::Impact,trap.id,trigger->id,trap.damage,trap.kind==Kind::AirBomb?"air bomb":trap.kind==Kind::SeekingAirMine?"seeking air mine":trap.kind==Kind::GiantBomb?"giant bomb":trap.kind==Kind::GigaBomb?"giga bomb":"bomb");
    emit(EventType::Destroyed,trap.id,trap.id,0,to_string(trap.kind));
  }
}
void BattleState::update_tornado_traps() {
  for(auto& trap:entities_) if(alive(trap)&&trap.kind==Kind::TornadoTrap&&!trap.concealed) {
    const auto* stats=data_.find(trap.kind,trap.level);
    if(!stats||stats->effect_duration<=0||now_ms_>=trap.next_action) { trap.hp=0; emit(EventType::Destroyed,trap.id,trap.id,0,to_string(trap.kind)); continue; }
    const auto ticks=std::max<Milliseconds>(1,stats->effect_duration/kTickMs);
    const double per_tick=trap.damage/static_cast<double>(ticks);
    for(const auto& target:entities_) if(alive(target)&&!target.underground&&target.side==Side::Attacker&&dist(target.pos,trap.pos)<=trap.splash_radius) damage(trap.id,target.id,per_tick);
    emit(EventType::Impact,trap.id,0,per_tick,"tornado trap damage");
  }
}
} // namespace cocsim

