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

void BattleState::update_entities() {
  // The old 100-ms refresh is nearest to six 16-ms steps (96 ms).
  constexpr Milliseconds kPathRefreshMs=6*kTickMs;
  // A Druid's human form remains active for the sourced logical duration.
  // The Bear enters through the ordinary stable spawn queue at the exact
  // fixed-tick deadline. Its complete state is therefore Core state and the
  // Viewer or replay timer cannot affect either the deadline or entity order.
  for(auto& druid:entities_) if(alive(druid)&&druid.kind==Kind::Druid
      &&druid.druid_transform_at>0&&now_ms_>=druid.druid_transform_at) {
    const auto id=druid.id;
    const auto level=druid.level;
    const auto position=druid.pos;
    const auto side=druid.side;
    druid.hp=0;
    queue_spawn(Kind::DruidBear,id,level,position,side);
    emit(EventType::TargetChanged,id,0,0,"druid transforms after human duration");
  }
  // Apprentice Warden's Life Aura is a continuously evaluated attacker-side
  // HP bonus for nearby ground troops. The official contract fixes the radius,
  // percentages and non-stacking strongest-aura rule. Recomputing from the
  // immutable catalogue and logical positions keeps an aura transition fully
  // deterministic and restores the same result after a snapshot or replay.
  for(auto& candidate:entities_) {
    if(!alive(candidate)||candidate.side!=Side::Attacker||candidate.flying) continue;
    const auto* candidate_stats=data_.find(candidate.kind,candidate.level,candidate.supercharged?"supercharged":"normal");
    if(!candidate_stats||candidate_stats->category!=EntityCategory::Troop) continue;
    double best_percent=0.0;
    for(const auto& warden:entities_) {
      if(!alive(warden)||warden.id==candidate.id||warden.side!=Side::Attacker||warden.kind!=Kind::ApprenticeWarden) continue;
      const auto* warden_stats=data_.find(warden.kind,warden.level,warden.supercharged?"supercharged":"normal");
      if(warden_stats&&warden_stats->life_aura_range>0.0&&warden_stats->life_aura_hp_increase_percent>best_percent&&dist(candidate.pos,warden.pos)<=warden_stats->life_aura_range)
        best_percent=warden_stats->life_aura_hp_increase_percent;
    }
    const double desired_max_hp=candidate_stats->hp*(1.0+best_percent/100.0);
    if(desired_max_hp>candidate.max_hp) candidate.hp+=desired_max_hp-candidate.max_hp;
    candidate.max_hp=desired_max_hp;
    candidate.hp=std::min(candidate.hp,candidate.max_hp);
  }
  const auto destruction=result().destruction;
  for(auto& hidden:entities_) if(alive(hidden)&&hidden.concealed&&hidden.kind==Kind::HiddenTesla) {
    const auto* stats=data_.find(hidden.kind,hidden.level,hidden.supercharged?"supercharged":"normal");
    if(stats&&stats->reveal_destruction_percent>0&&destruction>=stats->reveal_destruction_percent) {
      hidden.concealed=false; emit(EventType::TargetChanged,hidden.id,0,destruction,"hidden tesla revealed by destruction"); continue;
    }
    for(const auto& attacker:entities_) if(alive(attacker)&&!attacker.underground&&attacker.side==Side::Attacker&&dist(hidden.pos,attacker.pos)<=hidden.trigger_range) { hidden.concealed=false; emit(EventType::TargetChanged,hidden.id,attacker.id,hidden.trigger_range,"hidden tesla revealed"); break; }
  }
  for(auto& a:entities_) {
    if(!alive(a)||a.cooldown==0) continue;
    const auto* own_stats=data_.find(a.kind,a.level,a.supercharged?"supercharged":"normal");
    bool frozen=false; double rage_damage=1.0,rage_speed=1.0,movement_speed=1.0;
    for(const auto& effect:spell_effects_) if(dist(a.pos,effect.pos)<=effect.radius) {
      if(effect.kind==SpellKind::Freeze&&a.side==Side::Defender) frozen=true;
      if(effect.kind==SpellKind::Rage&&a.side==Side::Attacker) { rage_damage*=effect.damage_multiplier; rage_speed*=effect.speed_multiplier; movement_speed*=effect.speed_multiplier; }
      if(effect.kind==SpellKind::Haste&&a.side==Side::Attacker) { rage_speed*=effect.speed_multiplier; movement_speed*=effect.speed_multiplier; }
    }
    // Supercell's launch contract defines a deployment-only eight-second
    // Rage.  The deadline is Core logical state (and Snapshot V15 state), so
    // the boundary is a strict T+ms comparison, independent of rendering.
    if(own_stats&&own_stats->deployment_rage_duration>0&&a.deployment_rage_until>now_ms_) {
      rage_damage*=own_stats->deployment_rage_damage_multiplier;
      movement_speed*=own_stats->deployment_rage_movement_speed_multiplier;
    }
    // Electro Titan's passive aura is a separate, periodic area attack. The
    // source fixes its 0.4-second cadence and 3.5-tile radius, explicitly
    // excluding Walls while including both ground and air targets. It is not
    // changed by Rage and continues through Freeze, so it executes before the
    // actor's ordinary frozen/action branch. `next_aura_action` is logical
    // state, serialised with the entity; rendering never advances this timer.
    if(own_stats&&own_stats->aura_damage>0.0&&own_stats->aura_range>0.0&&own_stats->aura_cooldown>0&&(!frozen||own_stats->aura_ignores_freeze)&&now_ms_>=a.next_aura_action) {
      const double aura_damage=own_stats->aura_damage*(own_stats->aura_ignores_rage?1.0:rage_damage);
      std::vector<EntityId> victims;
      for(const auto& candidate:entities_) if(alive(candidate)&&candidate.side!=a.side&&!candidate.concealed&&!candidate.underground&&(!own_stats->aura_excludes_walls||candidate.kind!=Kind::Wall)&&dist(a.pos,candidate.pos)<=own_stats->aura_range) victims.push_back(candidate.id);
      std::sort(victims.begin(),victims.end());
      for(const auto id:victims) { emit(EventType::Attack,a.id,id,aura_damage,"electro titan aura"); damage(a.id,id,aura_damage); }
      if(!victims.empty()) emit(EventType::Impact,a.id,0,aura_damage,"electro titan aura");
      a.next_aura_action=now_ms_+own_stats->aura_cooldown;
    }
    // Baby Dragon Tantrum and Dragon Duke Royal Rampage apply while no other
    // allied air unit is within their separately sourced isolation radius.
    // The predicate uses only serialised logical entity state at the fixed
    // tick, so adapters and replay restoration share one deterministic path.
    if(own_stats&&own_stats->isolation_radius>0.0) {
      const bool isolated=isolated_from_allied_flying(a,*own_stats);
      if(isolated) { rage_damage*=own_stats->rage_damage_multiplier; rage_speed*=own_stats->rage_attack_speed_multiplier; }
    }
    if(frozen) continue;
    if(a.side==Side::Defender&&a.activation_housing>0&&deployed_housing_<a.activation_housing) continue;
    const EntityId previous_target=a.target.value_or(0);
    auto t=a.target?entity(*a.target):nullptr;
    const bool stationary_defense_target_out_of_range = t&&a.side==Side::Defender&&a.speed<=0.0
      &&dist(a.pos,t->pos)>a.range+a.radius+t->radius;
    if(!t||!alive(*t)||t->underground||(a.side==Side::Defender&&t->invisible_to_defenses_until>now_ms_)||(a.heals&&t->hp>=t->max_hp)||stationary_defense_target_out_of_range) {
      const auto id=choose_target(a); a.target=id?std::optional<EntityId>(id):std::nullopt;
      a.waypoint.reset(); a.next_path=0;
      if(id!=previous_target) emit(EventType::TargetChanged,a.id,id,0,id?"acquired":"lost");
      t=id?entity(id):nullptr;
    }
    // Single-target Inferno damage is a three-stage beam tied to one
    // continuous lock. The sourced rule resets the beam if that lock is lost;
    // retaining this logical timer in the entity makes replay/snapshot state
    // deterministic without putting combat statistics outside GameData.
    const bool inferno_ramp=own_stats&&a.multi_target_count==0&&own_stats->inferno_initial_damage>0.0
      &&own_stats->inferno_second_damage>0.0&&own_stats->inferno_max_damage>0.0
      &&own_stats->inferno_second_stage_at>0&&own_stats->inferno_max_stage_at>own_stats->inferno_second_stage_at;
    if(inferno_ramp&&a.target.value_or(0)!=previous_target) {
      a.inferno_lock_elapsed=0;
      a.inferno_lock_target=a.target.value_or(0);
    }
    if(!t) { a.underground=false; continue; }
    const auto d=dist(a.pos,t->pos);
    if(d<a.min_range) { a.target.reset(); a.waypoint.reset(); a.next_path=0; if(inferno_ramp){a.inferno_lock_elapsed=0;a.inferno_lock_target=0;} continue; }
    const bool burrows=own_stats&&own_stats->burrows;
    const bool smashes_walls=own_stats&&own_stats->smashes_walls;
    const bool jumps_walls=own_stats&&own_stats->jumps_walls;
    // Super Minion's opening Long Shots use the sourced 10.25-tile reach.
    // Their unverified bonus multiplier is an explicit immutable catalogue
    // fallback, while this counter is future-affecting serialized state.
    const bool opening_long_shot=own_stats&&a.opening_long_shots_remaining>0
      &&own_stats->opening_long_shot_count>0&&own_stats->opening_long_shot_range>0.0;
    const double effective_range=opening_long_shot?own_stats->opening_long_shot_range:a.range;
    if(d>effective_range+a.radius+t->radius) {
      if(inferno_ramp) { a.inferno_lock_elapsed=0; a.inferno_lock_target=0; }
      a.underground=burrows;
      // Most defenses are immobile (speed == 0), but trap-spawned skeletons
      // are defender-side mobile combatants.  Movement therefore depends on
      // the sourced speed, never on the entity side.
      if(a.speed>0) {
        std::optional<Vec2> waypoint;
        if(smashes_walls&&!a.flying) {
          const Entity* touched_wall=nullptr;
          for(const auto& wall:entities_) if(alive(wall)&&wall.side!=a.side&&wall.kind==Kind::Wall&&dist(a.pos,wall.pos)<=a.radius+wall.radius) {
            if(!touched_wall||dist(a.pos,wall.pos)<dist(a.pos,touched_wall->pos)||(dist(a.pos,wall.pos)==dist(a.pos,touched_wall->pos)&&wall.id<touched_wall->id)) touched_wall=&wall;
          }
          if(touched_wall) {
            if(now_ms_>=a.next_action&&own_stats->wall_damage_per_hit>0&&own_stats->wall_attack_cooldown>0) {
              const auto applied=scaled_damage(a.id,touched_wall->id,own_stats->wall_damage_per_hit);
              emit(EventType::Attack,a.id,touched_wall->id,applied,"root rider wall touch");
              damage(a.id,touched_wall->id,own_stats->wall_damage_per_hit);
              a.next_action=now_ms_+own_stats->wall_attack_cooldown;
            }
            continue;
          }
          waypoint=t->pos;
        }
        else if(a.flying||burrows) waypoint=t->pos;
        else {
          if(!a.waypoint||now_ms_>=a.next_path||dist(a.pos,*a.waypoint)<0.05) {
            a.waypoint=next_path_waypoint(a,*t,0,jumps_walls); a.next_path=now_ms_+kPathRefreshMs;
          }
          waypoint=a.waypoint;
        }
        if(!waypoint) {
          const auto wall=jumps_walls?0:blocking_wall(a,*t);
          if(wall) { a.target=wall; a.waypoint.reset(); a.next_path=0; emit(EventType::TargetChanged,a.id,wall,0,"blocking wall"); }
          continue;
        }
        const auto dx=waypoint->x-a.pos.x,dy=waypoint->y-a.pos.y; const auto l=std::sqrt(dx*dx+dy*dy);
        const auto step=a.speed*movement_speed*(static_cast<double>(kTickMs)/1000.0);
        if(l>0){a.pos.x+=dx/l*std::min(step,l);a.pos.y+=dy/l*std::min(step,l);}
      }
      continue;
    }
    a.underground=false;
    const bool inferno_single=inferno_ramp;
    if(inferno_single&&a.inferno_lock_target!=t->id) { a.inferno_lock_target=t->id; a.inferno_lock_elapsed=0; }
    // Supercell's April 2026 note changes only the first geared-up Mortar
    // burst: it waits 0.5 seconds instead of firing immediately. `next_action`
    // is initially zero and is already canonical snapshot/hash state, so this
    // marker survives restore without a GUI timer or an extra mutable stat.
    const bool geared_mortar=a.kind==Kind::Mortar&&own_stats&&own_stats->burst_shots>0
      &&a.shots_per_burst==own_stats->burst_shots&&a.cooldown==own_stats->burst_cooldown;
    if(geared_mortar&&a.shots_in_burst==0&&a.next_action==0&&own_stats->attributes.first_burst_delay>0)
      a.next_action=now_ms_+own_stats->attributes.first_burst_delay;
    if(now_ms_<a.next_action) { if(inferno_single) a.inferno_lock_elapsed+=kTickMs; continue; }
    // All cadence changes are quantised to the fixed rule tick.  This is
    // deliberately a ceil rather than a wall-clock duration: no action can
    // occur between T+ms rule ticks.
    const auto action_cooldown=std::max<Milliseconds>(kTickMs,static_cast<Milliseconds>(std::ceil(static_cast<double>(a.cooldown)/rage_speed/static_cast<double>(kTickMs)))*kTickMs);
    a.next_action=now_ms_+action_cooldown;
    if(a.heals) {
      // The current version-pinned Healer source establishes its 0.7 s
      // cadence, friendly ground-only eligibility and healing rate, but not
      // launch/impact timing. Keep that unresolved timing in the shared
      // deterministic projectile pipeline rather than inventing a direct hit.
      const auto amount=a.healing*(static_cast<double>(a.cooldown)/1000.0);
      const auto projectile_id=next_projectile_id_++;
      projectiles_.push_back({projectile_id,a.id,t->id,amount,0.0,a.pos,a.pos,0.0,now_ms_,now_ms_+kTickMs,false});
      emit(EventType::Projectile,a.id,t->id,amount,"logical healing projectile",projectile_id);
      continue;
    }
    double damage_amount=a.damage*rage_damage;
    if(opening_long_shot) damage_amount*=own_stats->opening_long_shot_damage_multiplier;
    // Monolith's published contract is additive: each impact carries its
    // level-specific base damage plus a percentage of the selected target's
    // maximum hitpoints.  Compute the value before creating the serialised
    // projectile, so snapshot/replay preserve the future impact exactly and
    // no render-time or wall-clock value participates in combat.
    if(own_stats&&own_stats->max_hp_damage_percent>0.0)
      damage_amount+=t->max_hp*own_stats->max_hp_damage_percent/100.0;
    if(inferno_single) {
      if(a.inferno_lock_elapsed<own_stats->inferno_second_stage_at) damage_amount=own_stats->inferno_initial_damage;
      else if(a.inferno_lock_elapsed<own_stats->inferno_max_stage_at) damage_amount=own_stats->inferno_second_damage;
      else damage_amount=own_stats->inferno_max_damage;
    }
    std::vector<EntityId> targets{t->id};
    // Super Wizard Chain Magic branches from the first victim instead of
    // hopping from one secondary victim to another. The public evidence fixes
    // 40% secondary damage and a ten-victim interpretation; source material
    // does not publish a measured radius, so the frozen delta explicitly uses
    // the three-tile base range as a replaceable proxy. Victims are selected
    // once at launch, nearest to the primary then entity ID, and the normal
    // serialised next-tick projectile path carries their future damage.
    const bool chain_magic=own_stats&&own_stats->chain_damage_multiplier>0.0
      &&own_stats->chain_target_count>1&&own_stats->chain_radius>0.0;
    const bool electro_dragon_chain=a.kind==Kind::ElectroDragon&&chain_magic;
    if(electro_dragon_chain) {
      // Electro Dragon lightning hops from each victim to the next rather
      // than branching around the primary target.  The primary source caps
      // the full chain at five victims and forbids repeats.  The secondary
      // geometry is expressed as the allowed empty gap between footprints,
      // then equal candidates resolve by maximum HP and finally entity ID.
      EntityId previous=t->id;
      while(targets.size()<static_cast<std::size_t>(own_stats->chain_target_count)) {
        const auto* previous_entity=entity(previous);
        if(!previous_entity) break;
        const Entity* chosen=nullptr;
        double chosen_distance{};
        for(const auto& candidate:entities_) {
          if(!alive(candidate)||candidate.side==a.side||candidate.concealed||candidate.underground
              ||std::find(targets.begin(),targets.end(),candidate.id)!=targets.end()
              ||(a.target_type==TargetType::Air&&!candidate.flying)
              ||(a.target_type==TargetType::Ground&&candidate.flying)) continue;
          const auto candidate_distance=dist(previous_entity->pos,candidate.pos);
          if(candidate_distance>own_stats->chain_radius+previous_entity->radius+candidate.radius) continue;
          if(!chosen||candidate_distance<chosen_distance
              ||(candidate_distance==chosen_distance&&(candidate.max_hp>chosen->max_hp
                  ||(candidate.max_hp==chosen->max_hp&&candidate.id<chosen->id)))) {
            chosen=&candidate;
            chosen_distance=candidate_distance;
          }
        }
        if(!chosen) break;
        targets.push_back(chosen->id);
        previous=chosen->id;
      }
    } else if(chain_magic) {
      for(const auto& candidate:entities_) if(alive(candidate)&&candidate.side!=a.side
          &&!candidate.concealed&&!candidate.underground&&candidate.id!=t->id
          &&(a.target_type==TargetType::Both||(a.target_type==TargetType::Air?candidate.flying:!candidate.flying))
          &&dist(t->pos,candidate.pos)<=own_stats->chain_radius+candidate.radius) targets.push_back(candidate.id);
      std::stable_sort(targets.begin()+1,targets.end(),[&](EntityId left,EntityId right){
        const auto* l=entity(left); const auto* r=entity(right);
        const auto dl=dist(t->pos,l->pos),dr=dist(t->pos,r->pos);
        return dl==dr?left<right:dl<dr;
      });
      if(targets.size()>static_cast<std::size_t>(own_stats->chain_target_count))
        targets.resize(static_cast<std::size_t>(own_stats->chain_target_count));
    }
    if(a.multi_target_count>1) {
      for(const auto& candidate:entities_) if(alive(candidate)&&candidate.side!=a.side&&!candidate.concealed&&!candidate.underground&&!(a.side==Side::Defender&&candidate.invisible_to_defenses_until>now_ms_)&&candidate.id!=t->id&&(a.target_type==TargetType::Both||(a.target_type==TargetType::Air?candidate.flying:!candidate.flying))&&dist(a.pos,candidate.pos)<=a.range+a.radius+candidate.radius) targets.push_back(candidate.id);
      std::stable_sort(targets.begin()+1,targets.end(),[&](EntityId left,EntityId right){const auto* l=entity(left);const auto* r=entity(right);const auto dl=dist(a.pos,l->pos),dr=dist(a.pos,r->pos);return dl==dr?left<right:dl<dr;});
      if(targets.size()>static_cast<std::size_t>(a.multi_target_count)) targets.resize(static_cast<std::size_t>(a.multi_target_count));
      // Supercell specifies that the Multi-Archer Tower still fires all three
      // arrows under capacity: three at one target, or one target twice when
      // only two are available.  The official rule leaves that doubled target
      // unspecified, so the already captured primary (distance then entity ID)
      // is repeated deterministically.  Other multi-target weapons retain
      // their own source-defined distinct-target contracts.
      if(a.kind==Kind::MultiArcherTower)
        while(targets.size()<static_cast<std::size_t>(a.multi_target_count)) targets.push_back(targets.front());
    }
    // The ordinary Bowler's two impacts and the Super Bowler's three impacts
    // are emitted as one deterministic launch-time ray. Public sources do
    // not define their exact bounce geometry or timing, so GameData carries a
    // versioned forward-step proxy. The ordinary Bowler deliberately keeps a
    // zero-radius lower bound until a splash radius is sourced.
    const bool bouncing_boulder=own_stats&&own_stats->bounce_impact_count>1
      &&own_stats->bounce_step>0.0
      &&(a.kind==Kind::Bowler||a.kind==Kind::SuperBowler);
    if(bouncing_boulder) {
      const auto dx=t->pos.x-a.pos.x,dy=t->pos.y-a.pos.y;
      const auto length=std::hypot(dx,dy);
      if(length>0.0) for(int impact_index=0;impact_index<own_stats->bounce_impact_count;++impact_index) {
        const Vec2 center{t->pos.x+dx/length*own_stats->bounce_step*impact_index,
                          t->pos.y+dy/length*own_stats->bounce_step*impact_index};
        const auto projectile_id=next_projectile_id_++;
        projectiles_.push_back({projectile_id,a.id,0,damage_amount,own_stats->bounce_splash_radius,a.pos,a.pos,0.0,now_ms_,now_ms_+kTickMs,false,center,false,true});
        const bool super_bowler=a.kind==Kind::SuperBowler;
        emit(EventType::Attack,a.id,t->id,scaled_damage(a.id,t->id,damage_amount),super_bowler?"super bowler triple strike":"bowler double strike");
        emit(EventType::Projectile,a.id,0,damage_amount,super_bowler?"super bowler fixed bounce":"bowler fixed bounce",projectile_id);
      }
      if(a.shots_per_burst>1 && ++a.shots_in_burst>=a.shots_per_burst) { a.shots_in_burst=0; a.next_action=now_ms_+a.time_between_bursts; }
      continue;
    }
    // The Inferno Tower's normal and multi-target attacks are continuous
    // beams, rather than travelling ammunition.  The public combat rendering
    // terminology and the pinned secondary data both describe beams; keeping
    // this exception here prevents a fabricated one-tick projectile delay.
    // All other ranged weapons continue through the serialised projectile
    // path until a versioned flight contract says otherwise.
    // Super Miner shares the sourced three-stage damage schedule, but is a
    // melee drill.  It must retain the normal contact-damage route instead of
    // being presented or scheduled as an Inferno beam.
    const bool inferno_beam=a.kind==Kind::InfernoDragon || a.kind==Kind::InfernoTower;
    for(std::size_t target_index=0;target_index<targets.size();++target_index) {
      const auto target_id=targets[target_index];
      const auto target_damage=electro_dragon_chain
        ? damage_amount*std::pow(own_stats->chain_damage_multiplier,static_cast<double>(target_index))
        : chain_magic&&target_id!=t->id ? damage_amount*own_stats->chain_damage_multiplier : damage_amount;
      const auto applied_damage=scaled_damage(a.id,target_id,target_damage);
      emit(EventType::Attack,a.id,target_id,applied_damage,inferno_beam?"inferno beam":a.kind==Kind::SuperMiner?"super miner drill":opening_long_shot?"super minion long shot":electro_dragon_chain&&target_index>0?"electro dragon chain":chain_magic&&target_id!=t->id?"super wizard chain":a.multi_target_count>1?"multi-target attack":"attack");
      if(a.ranged&&!inferno_beam) {
        // The only current flight contract is the Mine's dedicated trap path
        // above. Even if a future catalogue import brings a numeric speed for
        // an ordinary weapon, it must not silently become a homing shot: its
        // launch, endpoint and target-loss rules need their own sourced Core
        // contract first. Until then this remains the explicit logical
        // Next-tick projectile used by GUI and RL.
        const auto projectile_id=next_projectile_id_++;
        // Sharp Shot has separately sourced acquisition (6 tiles) and fixed
        // projectile (12 tiles) ranges. Capture the whole ray at launch: a
        // later target move, render frame or target retarget cannot bend it.
        // The collision-width approximation is documented in the reference;
        // its endpoint and piercing flag are future-affecting snapshot state.
        const bool sharp_shot=own_stats&&own_stats->piercing_projectile_range>0.0;
        Vec2 endpoint=a.pos;
        if(sharp_shot) {
          const auto dx=t->pos.x-a.pos.x,dy=t->pos.y-a.pos.y;
          const auto length=std::hypot(dx,dy);
          if(length>0.0) {
            endpoint.x+=dx*own_stats->piercing_projectile_range/length;
            endpoint.y+=dy*own_stats->piercing_projectile_range/length;
          }
        }
        projectiles_.push_back({projectile_id,a.id,target_id,target_damage,a.splash?a.splash_radius:0.0,a.pos,a.pos,0.0,now_ms_,now_ms_+kTickMs,false,endpoint,sharp_shot});
        emit(EventType::Projectile,a.id,target_id,applied_damage,sharp_shot?"super archer sharp shot":opening_long_shot?"super minion long shot":electro_dragon_chain&&target_index>0?"electro dragon chain":chain_magic&&target_id!=t->id?"super wizard chain":"logical projectile",projectile_id);
      }
      else { damage(a.id,target_id,target_damage); }
    }
    if(!a.ranged&&a.splash&&a.splash_radius>0) { const auto impact=t->pos; for(auto& other:entities_) if(other.id!=t->id&&alive(other)&&other.side==t->side&&dist(other.pos,impact)<=a.splash_radius) damage(a.id,other.id,damage_amount); }
    if(const auto* stats=data_.find(a.kind,a.level);stats&&stats->self_destruct_on_attack&&alive(a)) {
      // The Wall Breaker's sourced attack is already an area blast.  Its
      // self-destruction must not cause a second death-damage blast.
      a.hp=0; emit(EventType::Destroyed,a.id,a.id,0,to_string(a.kind));
    }
    if(a.shots_per_burst>1 && ++a.shots_in_burst>=a.shots_per_burst) { a.shots_in_burst=0; a.next_action=now_ms_+a.time_between_bursts; }
    // The official contract only calls these the first eight attacks. Count a
    // captured launch, not target retention or animation completion: those
    // lifecycle details are unknown and this deterministic rule is documented
    // as a replaceable approximation in the versioned reference.
    if(opening_long_shot) --a.opening_long_shots_remaining;
    if(inferno_single) a.inferno_lock_elapsed+=kTickMs;
  }
}
} // namespace cocsim
