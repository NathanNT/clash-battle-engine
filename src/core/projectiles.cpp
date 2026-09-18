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

void BattleState::update_projectiles() {
  std::vector<Projectile> active;
  active.reserve(projectiles_.size());
  for (auto p : projectiles_) {
    auto* target = entity(p.target);
    // `homing` is a rule contract, not a rendering hint. A future speed value
    // is therefore unable to make an ordinary ranged attack follow a target
    // unless its source explicitly supplies that behavior and the launch path
    // creates a homing projectile. Non-homing delayed effects resolve only on
    // their serialised impact tick.
    const bool target_lost = !target || !alive(*target) || target->underground;
    bool impact = (!p.homing || p.speed_tiles_per_second <= 0.0)
      ? p.impact == now_ms_ : target_lost;
    if (!impact && p.homing && p.speed_tiles_per_second > 0.0) {
      // This flight is evaluated exclusively in the rule clock.  Homing uses
      // the target's tick position; no GUI/wall-clock interpolation can alter
      // collision, damage or the future state hash.
      const Vec2 aim = target->pos;
      const double dx = aim.x - p.position.x, dy = aim.y - p.position.y;
      const double distance = std::hypot(dx, dy);
      const double step = p.speed_tiles_per_second * double(kTickMs) / 1000.0;
      if (distance <= step) { p.position = aim; impact = true; }
      else if (distance > 0.0) { p.position.x += dx * step / distance; p.position.y += dy * step / distance; }
    }
    if (!impact) { active.push_back(p); continue; }
    if(p.fixed_impact) {
      const auto* owner=entity(p.owner);
      if(owner) for(auto& victim:entities_) if(alive(victim)&&!victim.underground
          &&victim.side!=owner->side&&!victim.flying
          &&dist(victim.pos,p.endpoint)<=p.splash_radius) damage(p.owner,victim.id,p.damage);
    } else if (target && alive(*target) && !target->underground) {
      const auto* owner=entity(p.owner);
      // The only current friendly projectile is the Healer's unmeasured
      // logical T+10 ms effect. Side is immutable entity identity, including
      // for an owner that died after launch, so this remains snapshot/replay
      // deterministic without adding mutable combat statistics to entities.
      if(owner&&owner->side==target->side) {
        const auto before=target->hp;
        target->hp=std::min(target->max_hp,target->hp+p.damage);
        emit(EventType::Healed,p.owner,p.target,target->hp-before,"projectile heal",p.id);
      } else {
        if(p.piercing) {
          // Sharp Shot's source establishes a fixed straight 12-tile ray and
          // damage to every target it crosses. The public material does not
          // publish hitbox widths, so use the existing Core entity radii and
          // retain this explicitly partial geometry rather than claiming an
          // exact client collision contract. Sort ray order, then entity id,
          // before applying damage to remove container-order dependence.
          const auto dx=p.endpoint.x-p.origin.x,dy=p.endpoint.y-p.origin.y;
          const auto length_sq=dx*dx+dy*dy;
          std::vector<std::pair<double,EntityId>> hits;
          if(length_sq>0.0) for(const auto& candidate:entities_) {
            if(!alive(candidate)||candidate.underground||candidate.side!=target->side) continue;
            const auto offset_x=candidate.pos.x-p.origin.x,offset_y=candidate.pos.y-p.origin.y;
            const auto projection=(offset_x*dx+offset_y*dy)/length_sq;
            if(projection<0.0||projection>1.0) continue;
            const auto closest_x=p.origin.x+projection*dx,closest_y=p.origin.y+projection*dy;
            if(std::hypot(candidate.pos.x-closest_x,candidate.pos.y-closest_y)<=candidate.radius)
              hits.emplace_back(projection,candidate.id);
          }
          std::sort(hits.begin(),hits.end(),[](const auto& left,const auto& right) {
            return left.first==right.first?left.second<right.second:left.first<right.first;
          });
          for(const auto& [unused,id]:hits) { (void)unused; damage(p.owner,id,p.damage); }
        } else {
          damage(p.owner, p.target, p.damage);
        }
        // Scattershot is not the usual circular splash. Its pinned per-level
        // payload supplies the direct/inner/outer falloff damage, while the
        // separately sourced immutable behavior contract supplies a 90-degree
        // rear cone, one-tile inner boundary, five-tile outer boundary and an
        // altitude filter. The direction is the serialised launch origin to
        // the impact target; no Viewer-derived geometry participates.
        const auto* scattershot=owner&&owner->kind==Kind::Scattershot
          ? data_.find(owner->kind,owner->level,owner->supercharged?"supercharged":"normal") : nullptr;
        if(scattershot) {
          const auto impact_pos=target->pos;
          const auto dx=impact_pos.x-p.origin.x,dy=impact_pos.y-p.origin.y;
          const auto length=std::hypot(dx,dy);
          const auto cosine=std::cos(scattershot->scattershot_cone_angle_degrees*std::numbers::pi/360.0);
          if(length>0.0) for(auto& other:entities_) {
            if(other.id==p.target||!alive(other)||other.underground||other.side!=target->side) continue;
            if(scattershot->scattershot_same_altitude_only&&other.flying!=target->flying) continue;
            const auto ox=other.pos.x-impact_pos.x,oy=other.pos.y-impact_pos.y;
            const auto distance=std::hypot(ox,oy);
            if(distance<=0.0||distance>scattershot->scattershot_cone_range) continue;
            const auto forward=(ox*dx+oy*dy)/(distance*length);
            if(forward<cosine) continue;
            const auto inner=std::min(distance,scattershot->scattershot_inner_range);
            const auto damage_at_inner=p.damage+(scattershot->scattershot_direct_min_damage-p.damage)
              *inner/scattershot->scattershot_inner_range;
            const auto amount=distance<=scattershot->scattershot_inner_range?damage_at_inner:
              scattershot->scattershot_splash_max_damage+(scattershot->scattershot_splash_min_damage-scattershot->scattershot_splash_max_damage)
              *(distance-scattershot->scattershot_inner_range)/(scattershot->scattershot_cone_range-scattershot->scattershot_inner_range);
            damage(p.owner,other.id,amount);
          }
        } else if (p.splash_radius > 0) {
          const auto pos = target->pos;
          for (auto& e : entities_) if (e.id != p.target && alive(e) && !e.underground && e.side == target->side && dist(e.pos, pos) <= p.splash_radius) damage(p.owner, e.id, p.damage);
        }
      }
    }
    emit(EventType::Impact, p.owner, p.target, p.damage, "projectile impact",p.id);
  }
  projectiles_ = std::move(active);
}
void BattleState::update_death_explosions() {
  auto pending=std::move(death_explosions_); death_explosions_.clear();
  for(const auto& explosion:pending) {
    if(explosion.impact!=now_ms_) { death_explosions_.push_back(explosion); continue; }
    for(auto& victim:entities_) if(alive(victim)&&!victim.underground&&victim.side!=explosion.side&&(!explosion.ground_only||!victim.flying)&&dist(victim.pos,explosion.position)<=explosion.radius) damage(explosion.owner,victim.id,explosion.damage);
    emit(EventType::Impact,explosion.owner,0,explosion.damage,"death damage");
  }
}
} // namespace cocsim
