#include "cocsim/core.hpp"
#include "detail/battle_state_internal.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace cocsim {

std::uint64_t BattleState::state_hash() const {
  // Snapshots bind to presentation choices, while the logical hash omits
  // values that cannot affect a future combat tick.
  auto canonical = snapshot().canonical;
  const auto fingerprint_start = canonical.find('\n') + 1;
  const auto fingerprint_end = canonical.find(' ', fingerprint_start);
  canonical.replace(fingerprint_start, fingerprint_end - fingerprint_start,
                    std::to_string(scenario_fingerprint(false)));
  std::uint64_t hash = 1469598103934665603ULL;
  for (const unsigned char byte : canonical) {
    hash ^= byte;
    hash *= 1099511628211ULL;
  }
  return hash;
}
std::uint64_t BattleState::scenario_fingerprint(bool include_loadout) const {
  // A snapshot restores into a caller-provided immutable Scenario. Bind the
  // canonical state to that layout, including non-combat placement obstacles.
  Scenario static_scenario = scenario_;
  // Reserves are runtime state and are serialized below; the immutable layout
  // is what must match before a snapshot can be restored.
  static_scenario.army.clear();
  static_scenario.spells.clear();
  if (!include_loadout) {
    static_scenario.hero_loadouts.clear();
    // A Hero Banner occupies the same Core obstacle geometry for every
    // selected image. Defensive Hero combat is not materialized yet.
    for (auto& obstacle : static_scenario.non_combat_obstacles)
      if (obstacle.kind == Kind::HeroBanner) obstacle.variant = "normal";
  }
  std::uint64_t hash = 1469598103934665603ULL;
  for (const unsigned char byte : to_json(static_scenario)) { hash ^= byte; hash *= 1099511628211ULL; }
  // The rule step changes future event times even for an identical scenario.
  for (const unsigned char byte : std::to_string(kTickMs)) { hash ^= byte; hash *= 1099511628211ULL; }
  return hash;
}
Snapshot BattleState::snapshot() const {
  std::ostringstream s; s<<std::setprecision(17)<<"COCSIM-SNAPSHOT-22\n"<<scenario_fingerprint()<<' '<<now_ms_<<' '<<next_id_<<' '<<next_projectile_id_<<' '<<deployed_housing_<<' '<<finished_<<' '<<attackers_win_<<' '<<timed_out_<<' '<<command_cursor_<<'\n';
  s<<scenario_.army.size()<<'\n'; for(const auto& a:scenario_.army) s<<int(a.kind)<<' '<<a.level<<' '<<a.count<<' '<<std::quoted(a.mode)<<'\n';
  s<<scenario_.spells.size()<<'\n'; for(const auto& a:scenario_.spells) s<<int(a.kind)<<' '<<a.level<<' '<<a.count<<'\n';
  s<<entities_.size()<<'\n'; for(const auto& e:entities_) s<<e.id<<' '<<int(e.kind)<<' '<<int(e.side)<<' '<<e.level<<' '<<e.pos.x<<' '<<e.pos.y<<' '<<e.hp<<' '<<e.max_hp<<' '<<e.damage<<' '<<e.healing<<' '<<e.range<<' '<<e.min_range<<' '<<e.speed<<' '<<e.radius<<' '<<e.splash_radius<<' '<<e.cooldown<<' '<<e.next_action<<' '<<e.next_path<<' '<<e.time_between_bursts<<' '<<e.activation_housing<<' '<<e.shots_per_burst<<' '<<e.shots_in_burst<<' '<<e.ranged<<' '<<e.splash<<' '<<e.flying<<' '<<e.heals<<' '<<int(e.target_focus)<<' '<<int(e.target_type)<<' '<<e.defenses<<' '<<(e.target?*e.target:0)<<' '<<bool(e.waypoint)<<' '<<(e.waypoint?e.waypoint->x:0)<<' '<<(e.waypoint?e.waypoint->y:0)<<' '<<e.trigger_range<<' '<<e.concealed<<' '<<e.earthquake_hits<<' '<<e.multi_target_count<<' '<<e.supercharged<<' '<<e.underground<<' '<<e.invisible_to_defenses_until<<' '<<e.inferno_lock_elapsed<<' '<<e.inferno_lock_target<<' '<<e.next_aura_action<<' '<<e.deployment_rage_until<<' '<<e.opening_long_shots_remaining<<' '<<e.remaining_yetimites<<' '<<e.yeti_damage_since_spawn<<' '<<e.druid_transform_at<<'\n';
  s<<projectiles_.size()<<'\n'; for(const auto& p:projectiles_) s<<p.id<<' '<<p.owner<<' '<<p.target<<' '<<p.damage<<' '<<p.splash_radius<<' '<<p.origin.x<<' '<<p.origin.y<<' '<<p.position.x<<' '<<p.position.y<<' '<<p.speed_tiles_per_second<<' '<<p.launched<<' '<<p.impact<<' '<<p.homing<<' '<<p.endpoint.x<<' '<<p.endpoint.y<<' '<<p.piercing<<' '<<p.fixed_impact<<'\n';
  s<<death_explosions_.size()<<'\n'; for(const auto& e:death_explosions_) s<<e.owner<<' '<<e.position.x<<' '<<e.position.y<<' '<<int(e.side)<<' '<<e.damage<<' '<<e.radius<<' '<<e.impact<<' '<<e.ground_only<<'\n';
  s<<spell_effects_.size()<<'\n'; for(const auto& e:spell_effects_) s<<int(e.kind)<<' '<<e.pos.x<<' '<<e.pos.y<<' '<<e.radius<<' '<<e.damage_multiplier<<' '<<e.speed_multiplier<<' '<<e.healing_per_pulse<<' '<<e.expires<<' '<<e.next_pulse<<' '<<e.pulse_interval<<' '<<e.pulses<<'\n';
  s<<commands_.size()<<'\n'; for(const auto& c:commands_) s<<int(c.type)<<' '<<c.requested_ms<<' '<<c.effective_ms<<' '<<c.sequence<<' '<<int(c.kind)<<' '<<c.level<<' '<<c.position.x<<' '<<c.position.y<<' '<<int(c.spell)<<'\n'; return {s.str()};
}
bool BattleState::restore(const Snapshot& snap,std::string* error) {
  std::istringstream s(snap.canonical); std::string magic;
  if(!(s>>magic)||!magic.starts_with("COCSIM-SNAPSHOT-")){if(error)*error="unsupported snapshot format";return false;}
  const auto version_text=magic.substr(std::string("COCSIM-SNAPSHOT-").size()); int version=0;
  if(version_text.empty()||!std::all_of(version_text.begin(),version_text.end(),[](unsigned char c){return c>='0'&&c<='9';})){if(error)*error="unsupported snapshot format";return false;}
  for(const char c:version_text) { if(version>22){if(error)*error="unsupported snapshot format";return false;} version=version*10+(c-'0'); }
  if(version<7||version>22){if(error)*error="unsupported snapshot format";return false;}
  const bool v3=version>=3,v4=version>=4,v5=version>=5,v6=version>=6,v7=version>=7,v8=version>=8,v9=version>=9,v10=version>=10,v11=version>=11,v12=version>=12,v13=version>=13,v14=version>=14,v15=version>=15,v16=version>=16,v17=version>=17,v18=version>=18,v19=version>=19,v20=version>=20,v21=version>=21; Scenario restore_scenario=scenario_; for(const auto& loadout:restore_scenario.hero_loadouts) for(auto& slot:restore_scenario.army) if(slot.kind==loadout.hero&&slot.count==0) slot.count=1; BattleState c(data_,restore_scenario); std::size_t n; int b;
  if(v7){std::uint64_t fingerprint{};if(!(s>>fingerprint)||fingerprint!=c.scenario_fingerprint()){if(error)*error="snapshot scenario mismatch";return false;}}
  if(!(s>>c.now_ms_>>c.next_id_)||(v13&&!(s>>c.next_projectile_id_))||!(s>>c.deployed_housing_>>b>>c.attackers_win_>>c.timed_out_>>c.command_cursor_)){if(error)*error="truncated snapshot header";return false;} c.finished_=b;
  if(!(s>>n)){if(error)*error="truncated army";return false;} c.scenario_.army.clear(); for(std::size_t i=0;i<n;++i){int k;ArmySlot a;if(!(s>>k>>a.level>>a.count)||(v21&&!(s>>std::quoted(a.mode)))||k<0||k>=int(Kind::Count)){if(error)*error="invalid army";return false;}a.kind=Kind(k);c.scenario_.army.push_back(a);}
  if(!(s>>n)){if(error)*error="truncated spells";return false;} c.scenario_.spells.clear(); for(std::size_t i=0;i<n;++i){int k;SpellSlot a;if(!(s>>k>>a.level>>a.count)||k<0||k>=int(SpellKind::Count)){if(error)*error="invalid spell reserve";return false;}a.kind=SpellKind(k);c.scenario_.spells.push_back(a);}
  if(!(s>>n)){if(error)*error="truncated entities";return false;}
  c.entities_.clear();
  for(std::size_t i=0;i<n;++i) {
    Entity e;
    int k,side,ranged,splash,flying,heals,focus,type,defenses;
    EntityId target;
    int has_waypoint;
    if(!(s>>e.id>>k>>side>>e.level>>e.pos.x>>e.pos.y>>e.hp>>e.max_hp>>e.damage>>e.healing>>e.range>>e.min_range>>e.speed>>e.radius>>e.splash_radius>>e.cooldown>>e.next_action>>e.next_path>>e.time_between_bursts>>e.activation_housing>>e.shots_per_burst>>e.shots_in_burst>>ranged>>splash>>flying>>heals>>focus>>type>>defenses>>target>>has_waypoint)){if(error)*error="truncated entity";return false;}
    double wx,wy;
    int concealed=0,underground=0;
    Milliseconds invisible_to_defenses_until=0,inferno_lock_elapsed=0,next_aura_action=0,deployment_rage_until=0,druid_transform_at=0;
    EntityId inferno_lock_target=0;
    if(!(s>>wx>>wy)||(v3&&!(s>>e.trigger_range>>concealed))||(v4&&!(s>>e.earthquake_hits))||(v5&&!(s>>e.multi_target_count))||(v6&&!(s>>e.supercharged))||(v8&&!(s>>underground))||(v9&&!(s>>invisible_to_defenses_until))||(v10&&!(s>>inferno_lock_elapsed>>inferno_lock_target))||(v14&&!(s>>next_aura_action))||(v15&&!(s>>deployment_rage_until))||(v17&&!(s>>e.opening_long_shots_remaining))||(v19&&!(s>>e.remaining_yetimites>>e.yeti_damage_since_spawn))||(v20&&!(s>>druid_transform_at))||k<0||k>=int(Kind::Count)||side<0||side>1||focus<0||focus>int(TargetFocus::Heroes)||type<0||type>int(TargetType::Both)){if(error)*error="invalid entity";return false;}
    e.kind=Kind(k); e.side=Side(side); e.ranged=ranged; e.splash=splash; e.flying=flying; e.heals=heals; e.target_focus=TargetFocus(focus); e.target_type=TargetType(type); e.defenses=defenses;
    if(const auto* st=c.data_.find(e.kind,e.level,e.supercharged?"supercharged":"normal")){e.death_damage=st->death_damage;e.death_splash_radius=st->death_splash_radius;e.death_damage_delay=st->death_damage_delay;e.speed=st->movement_tiles_per_second;if(!v3)e.trigger_range=st->trigger_range;if(!v14&&st->aura_cooldown>0) next_aura_action=c.now_ms_+st->aura_cooldown;if(!v17)e.opening_long_shots_remaining=st->opening_long_shot_count;if(!v19&&e.kind==Kind::Yeti)e.remaining_yetimites=st->spawned_units;}
    e.concealed=concealed; e.underground=underground; e.invisible_to_defenses_until=invisible_to_defenses_until; e.inferno_lock_elapsed=inferno_lock_elapsed; e.inferno_lock_target=inferno_lock_target; e.next_aura_action=next_aura_action; e.deployment_rage_until=deployment_rage_until; e.druid_transform_at=druid_transform_at;
    if(target)e.target=target;
    if(has_waypoint)e.waypoint=Vec2{wx,wy};
    c.entities_.push_back(e);
  }
  if(!(s>>n)){if(error)*error="truncated projectiles";return false;}
  c.projectiles_.clear();
  for(std::size_t i=0;i<n;++i){
    Projectile p;
    if(v12) {
      int homing;
      if((v13&&!(s>>p.id))||!(s>>p.owner>>p.target>>p.damage>>p.splash_radius>>p.origin.x>>p.origin.y>>p.position.x>>p.position.y>>p.speed_tiles_per_second>>p.launched>>p.impact>>homing)){if(error)*error="invalid projectile";return false;}
      if(v16&&!(s>>p.endpoint.x>>p.endpoint.y>>p.piercing)){if(error)*error="invalid ballistic projectile";return false;}
      if(v18&&!(s>>p.fixed_impact)){if(error)*error="invalid fixed projectile";return false;}
      p.homing=homing;
    } else {
      if(!(s>>p.owner>>p.target>>p.damage>>p.splash_radius>>p.impact)){if(error)*error="invalid projectile";return false;}
      if(const auto* owner=c.entity(p.owner)) p.origin=p.position=owner->pos;
      p.launched=c.now_ms_;
    }
    // Older projectile records had no endpoint. Non-piercing shots never
    // consult one, so migrate to the V16 default value rather than inventing
    // a trajectory endpoint from the current target position.
    if(!v16) p.endpoint={};
    if(!v18) p.fixed_impact=false;
    // A moving non-homing projectile needs a separately sourced endpoint and
    // a future impact tick. No such contract is implemented yet, so reject a
    // malformed snapshot rather than preserving an entity that can never
    // resolve. Homing projectiles deliberately use impact==0 because their
    // collision tick depends on target motion.
    if(p.speed_tiles_per_second<0.0 || (!p.homing && p.speed_tiles_per_second>0.0 && p.impact<=p.launched)) { if(error)*error="invalid ballistic projectile";return false; }
    if(!v13) p.id=c.next_projectile_id_++;
    c.projectiles_.push_back(p);
  }
  if(v13) {
    if(c.next_projectile_id_==0 || std::any_of(c.projectiles_.begin(),c.projectiles_.end(),[&](const Projectile& p){return p.id==0||p.id>=c.next_projectile_id_;})) { if(error)*error="invalid projectile id";return false; }
    for(std::size_t i=0;i<c.projectiles_.size();++i) for(std::size_t j=i+1;j<c.projectiles_.size();++j) if(c.projectiles_[i].id==c.projectiles_[j].id) { if(error)*error="duplicate projectile id";return false; }
  }
  c.death_explosions_.clear();
  if(v11) {
    if(!(s>>n)){if(error)*error="truncated death explosions";return false;}
    for(std::size_t i=0;i<n;++i){DeathExplosion e;int side,ground;if(!(s>>e.owner>>e.position.x>>e.position.y>>side>>e.damage>>e.radius>>e.impact>>ground)||side<0||side>1){if(error)*error="invalid death explosion";return false;}e.side=Side(side);e.ground_only=ground;c.death_explosions_.push_back(e);}
  }
  if(!(s>>n)){if(error)*error="truncated effects";return false;}c.spell_effects_.clear();for(std::size_t i=0;i<n;++i){SpellEffect e;int k;if(!(s>>k>>e.pos.x>>e.pos.y>>e.radius>>e.damage_multiplier>>e.speed_multiplier>>e.healing_per_pulse>>e.expires>>e.next_pulse>>e.pulse_interval>>e.pulses)||k<0||k>=int(SpellKind::Count)){if(error)*error="invalid effect";return false;}e.kind=SpellKind(k);c.spell_effects_.push_back(e);}
  if(!(s>>n)||c.command_cursor_>n){if(error)*error="truncated commands";return false;}c.commands_.clear();for(std::size_t i=0;i<n;++i){Command x;int t,k,sp;if(!(s>>t>>x.requested_ms>>x.effective_ms>>x.sequence>>k>>x.level>>x.position.x>>x.position.y>>sp)||t<0||t>int(CommandType::EndBattle)||k<0||k>=int(Kind::Count)||sp<0||sp>=int(SpellKind::Count)){if(error)*error="invalid command";return false;}x.type=static_cast<CommandType>(t);x.kind=Kind(k);x.spell=SpellKind(sp);c.commands_.push_back(x);}c.events_.clear();*this=std::move(c);return true;
}

} // namespace cocsim
