#include "cocsim/core.hpp"
#include "detail/json_reader.hpp"

#include <algorithm>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <string>

namespace cocsim {
namespace {

// The public movement value uses eight units per Home Village tile/second.
// Preserve the catalogue value and derive physical movement once in immutable
// GameData.
constexpr double kCatalogueMovementSpeedUnitsPerTilePerSecond = 8.0;

using detail::array_body;
using detail::bool_field;
using detail::number_field;
using detail::object_after_key;
using detail::objects_in;
using detail::read_file;
using detail::string_field;

EntityCategory category_from_catalogue(const std::string& category) {
  if (category == "troop") return EntityCategory::Troop;
  if (category == "defense") return EntityCategory::Defense;
  if (category == "resource") return EntityCategory::Resource;
  if (category == "wall") return EntityCategory::Wall;
  if (category == "trap") return EntityCategory::Trap;
  return EntityCategory::Other;
}

TargetFocus focus_from_catalogue(const std::string& focus) {
  if (focus == "defenses") return TargetFocus::Defenses;
  if (focus == "defenses_only") return TargetFocus::DefensesOnly;
  if (focus == "resources") return TargetFocus::Resources;
  if (focus == "walls") return TargetFocus::Walls;
  if (focus == "friendly_troops") return TargetFocus::FriendlyTroops;
  return TargetFocus::Any;
}

TargetType target_type_from_catalogue(const std::string& target_type) {
  return target_type == "both" ? TargetType::Both
       : target_type == "air" ? TargetType::Air
                               : TargetType::Ground;
}

} // namespace
GameData GameData::v0() { // Runtime data are the pinned, normalized catalogue—not combat-class literals.
  static std::optional<GameData> cached;
  if(cached) return *cached;
  std::string error;
  const auto document=read_file(std::string(COCSIM_SOURCE_DIR)+"/data/catalogue.normalized.json",error);
  const auto contents=array_body(document,"contents");
  if(!contents) throw std::runtime_error("cannot load normalized catalogue: "+error);
  GameData d;
  for(const auto& content:objects_in(*contents)) {
    const auto id=string_field(content,"id");
    const auto kind=id?parse_kind(*id):std::nullopt;
    if(!kind) continue;
    const auto levels=array_body(content,"levels");
    if(!levels) continue;
    int footprint_w=1,footprint_h=1;
    if(const auto size=string_field(content,"footprint_tiles")) {
      const auto separator=size->find('x');
      if(separator!=std::string::npos) try { footprint_w=std::stoi(size->substr(0,separator)); footprint_h=std::stoi(size->substr(separator+1)); }
      catch(...) { throw std::runtime_error("invalid catalogue footprint"); }
    }
    // A source-marked, non-targetable entry has no combat levels. Load only
    // its placement metadata; it never receives an Entity or combat stats.
    if(objects_in(*levels).empty()) {
      if(!bool_field(content,"targetable").value_or(true)) {
        const auto margin=number_field(content,"deployment_margin_tiles");
        if(!margin||*margin<0) throw std::runtime_error("invalid non-combat obstacle metadata");
        d.non_combat_.push_back({*kind,footprint_w,footprint_h,static_cast<int>(*margin),string_field(content,"image").value_or("")});
      }
      continue;
    }
    const auto range=number_field(content,"range_tiles").value_or(0.0);
    const auto speed=number_field(content,"movement_speed").value_or(0.0);
    const auto attack_seconds=number_field(content,"attack_speed_seconds").value_or(0.0);
    const bool flying=bool_field(content,"flying").value_or(false);
    const bool heals=bool_field(content,"heals").value_or(false);
    // Projectile and area semantics follow the source data rather than
    // hand-maintained Kind tables. The Healer is excluded from the damage
    // weapon classification because it follows its own friendly-projectile
    // branch, not because its long-range effect bypasses projectiles_.
    const bool splash=string_field(content,"damage_type").value_or("")=="splash";
    const auto category=category_from_catalogue(string_field(content,"category").value_or(""));
    const auto focus=focus_from_catalogue(string_field(content,"target_focus").value_or("any"));
    auto target_type=target_type_from_catalogue(string_field(content,"target_type").value_or("ground"));
    double alternate_range=0.0, multi_target_range=0.0, burst_range=0.0; Milliseconds burst_cooldown=0,burst_pause=0; int burst_shots=0, declared_multi_target_count=0; auto alternate_target_type=TargetType::Ground;
    if(*kind==Kind::XBow) { target_type=TargetType::Ground; if(const auto modes=array_body(content,"modes")) for(const auto& mode:objects_in(*modes)) if(string_field(mode,"id").value_or("")=="airAndGround") { alternate_range=number_field(mode,"range_tiles").value_or(0.0); alternate_target_type=target_type_from_catalogue(string_field(mode,"target_type").value_or("both")); } }
    if(*kind==Kind::InfernoTower) if(const auto modes=array_body(content,"modes")) for(const auto& mode:objects_in(*modes)) if(string_field(mode,"id").value_or("")=="multiTarget") multi_target_range=number_field(mode,"range_tiles").value_or(0.0);
    Milliseconds inferno_second_stage_at=0,inferno_max_stage_at=0;
    if(const auto behavior=object_after_key(content,"behavior")) if(const auto ramp=object_after_key(*behavior,"single_target_ramp")) {
      inferno_second_stage_at=static_cast<Milliseconds>(std::llround(number_field(*ramp,"stage_two_after_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
      inferno_max_stage_at=static_cast<Milliseconds>(std::llround(number_field(*ramp,"stage_three_after_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
    }
    if(const auto ramp=object_after_key(content,"inferno_ramp")) {
      inferno_second_stage_at=static_cast<Milliseconds>(std::llround(number_field(*ramp,"stage_two_after_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
      inferno_max_stage_at=static_cast<Milliseconds>(std::llround(number_field(*ramp,"stage_three_after_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
    }
    double scattershot_cone_angle=0.0,scattershot_cone_range=0.0,scattershot_inner_range=0.0; bool scattershot_same_altitude_only=false;
    if(*kind==Kind::Scattershot) if(const auto behavior=object_after_key(content,"behavior")) {
      scattershot_cone_angle=number_field(*behavior,"rear_cone_angle_degrees").value_or(0.0);
      scattershot_cone_range=number_field(*behavior,"rear_cone_range_tiles").value_or(0.0);
      scattershot_inner_range=number_field(*behavior,"inner_falloff_range_tiles").value_or(0.0);
      scattershot_same_altitude_only=bool_field(*behavior,"same_altitude_only").value_or(false);
    }
    double aura_range=0.0; Milliseconds aura_cooldown=0; bool aura_excludes_walls=false,aura_ignores_freeze=false,aura_ignores_rage=false;
    if(*kind==Kind::ElectroTitan) {
      aura_range=number_field(content,"aura_range_tiles").value_or(0.0);
      aura_cooldown=static_cast<Milliseconds>(std::llround(number_field(content,"aura_attack_speed_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
      if(const auto behavior=object_after_key(content,"behavior")) {
        aura_excludes_walls=bool_field(*behavior,"aura_excludes_walls").value_or(false);
        aura_ignores_freeze=bool_field(*behavior,"aura_ignores_freeze").value_or(false);
        aura_ignores_rage=bool_field(*behavior,"aura_ignores_rage").value_or(false);
      }
    }
    if(const auto modes=array_body(content,"modes")) for(const auto& mode:objects_in(*modes)) {
      const auto mode_id=string_field(mode,"id").value_or("");
      if(*kind==Kind::MultiArcherTower&&mode_id=="normal") declared_multi_target_count=static_cast<int>(number_field(mode,"number_of_targets").value_or(0.0));
      if(mode_id=="gearedUpBurst"||(*kind==Kind::MultiGearTower&&mode_id=="fastAttack")) { burst_range=number_field(mode,"range_tiles").value_or(0.0); burst_cooldown=static_cast<Milliseconds>(std::llround(number_field(mode,"attack_speed_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs; burst_pause=static_cast<Milliseconds>(std::llround(number_field(mode,"time_between_bursts_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs; burst_shots=static_cast<int>(number_field(mode,"shots_per_burst").value_or(0.0)); }
    }
    const auto housing_space=static_cast<int>(number_field(content,"housing_space").value_or(0.0));
    const auto activation_housing_space=static_cast<int>(number_field(content,"activation_housing_space").value_or(0.0));
    const auto min_range=number_field(content,"min_range_tiles").value_or(0.0);
    const auto splash_radius=number_field(content,"splash_radius_tiles").value_or(0.0);
    const auto shots_per_burst=static_cast<int>(number_field(content,"shots_per_burst").value_or(1.0));
    const auto burst_seconds=number_field(content,"time_between_bursts_seconds").value_or(0.0);
    const auto time_between_bursts=static_cast<Milliseconds>(std::llround(burst_seconds*1000.0/kTickMs))*kTickMs;
    const auto death_splash_radius=number_field(content,"death_splash_radius_tiles").value_or(0.0);
    const auto death_damage_delay=static_cast<Milliseconds>(std::llround(number_field(content,"death_damage_delay_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
    const auto trigger_range=number_field(content,"trigger_range_tiles").value_or(0.0);
    const auto reveal_destruction_percent=number_field(content,"reveal_destruction_percent").value_or(0.0);
    const auto resource_damage_multiplier=number_field(content,"resource_damage_multiplier").value_or(1.0);
    const auto projectile_speed_field=number_field(content,"projectile_speed_tiles_per_second");
    const auto projectile_speed_tiles_per_second=projectile_speed_field.value_or(0.0);
    const bool projectile_speed_sourced=projectile_speed_field.has_value();
    const auto first_burst_delay=static_cast<Milliseconds>(std::llround(number_field(content,"first_burst_delay_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
    const bool death_damage_ground_only=bool_field(content,"death_damage_ground_only").value_or(false);
    const bool deployable=string_field(content,"support").value_or("")!="spawned_only";
    const auto wall_damage_multiplier=number_field(content,"wall_damage_multiplier").value_or(1.0);
    const bool self_destruct_on_attack=bool_field(content,"self_destruct_on_attack").value_or(false);
    // These immutable catalogue fields describe Baby Dragon's Tantrum.  They
    // intentionally remain GameData rather than per-entity combat statistics:
    // the active state is recomputed from the serialized logical positions.
    const auto isolation_radius=number_field(content,"isolation_radius_tiles").value_or(0.0);
    const auto rage_damage_multiplier=number_field(content,"rage_damage_multiplier").value_or(1.0);
    const auto rage_attack_speed_multiplier=number_field(content,"rage_attack_speed_multiplier").value_or(1.0);
    // Super Barbarian's deployment Rage is a rule duration, not a presentation
    // effect.  Preserve the public source speed unit in the catalogue and
    // derive the same immutable multiplier used by every Core adapter.
    const auto deployment_rage_duration=static_cast<Milliseconds>(std::llround(number_field(content,"deployment_rage_duration_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
    const auto deployment_rage_damage_multiplier=number_field(content,"deployment_rage_damage_multiplier").value_or(1.0);
    const auto deployment_rage_speed_increase=number_field(content,"deployment_rage_movement_speed_increase").value_or(0.0);
    const auto deployment_rage_movement_speed_multiplier=speed>0.0?(speed+deployment_rage_speed_increase)/speed:1.0;
    const bool burrows=bool_field(content,"burrows").value_or(false);
    const bool troop=category==EntityCategory::Troop;
    const bool trap=category==EntityCategory::Trap;
    const double radius=troop?(*kind==Kind::Giant?.65:(*kind==Kind::Archer?.35:.45)):std::max(footprint_w,footprint_h)/2.0;
    for(const auto& level:objects_in(*levels)) {
      const auto n=number_field(level,"level"),hp=number_field(level,"hitpoints");
      // Traps have no hit-point statistic in the source: they are armed state,
      // not attackable buildings.  A private positive sentinel only lets the
      // generic entity container retain the armed state; it is never exposed
      // as catalogue data or damageable by combat rules.
      if(!n||(!hp&&!trap)|| (hp&&*hp<=0)) throw std::runtime_error("invalid catalogue level");
      const auto integer=[&](const std::string& key){ return static_cast<int>(number_field(level,key).value_or(0.0)); };
      LevelAttributes attributes;
      attributes.town_hall_required=integer("town_hall_required");
      attributes.laboratory_required=integer("laboratory_required");
      attributes.upgrade_cost=number_field(level,"upgrade_cost").value_or(0.0);
      attributes.upgrade_cost_resource=string_field(level,"upgrade_cost_resource").value_or("");
      attributes.upgrade_time=static_cast<Milliseconds>(integer("upgrade_time_seconds"))*1000;
      attributes.xp_gained=integer("xp_gained");
      attributes.capacity=number_field(level,"capacity").value_or(0.0);
      attributes.production_rate=number_field(level,"production_rate").value_or(0.0);
      attributes.max_buildings=integer("max_buildings");
      attributes.max_traps=integer("max_traps");
      attributes.wall_rings=integer("wall_rings");
      attributes.first_burst_delay=first_burst_delay;
      attributes.supercharged=string_field(level,"variant").value_or("normal")=="supercharged";
      attributes.image=string_field(level,"image").value_or("");
      if(const auto storage=object_after_key(level,"storage_capacity")) {
        attributes.gold_storage=number_field(*storage,"gold").value_or(0.0);
        attributes.elixir_storage=number_field(*storage,"elixir").value_or(0.0);
        attributes.dark_elixir_storage=number_field(*storage,"darkElixir").value_or(0.0);
      }
      double multi_damage=0.0,burst_damage=0.0; int multi_count=0;
      if(const auto mode_stats=object_after_key(level,"mode_stats")) if(const auto multi=object_after_key(*mode_stats,"multiTarget")) { multi_damage=number_field(*multi,"damage_per_shot").value_or(0.0); multi_count=static_cast<int>(number_field(*multi,"number_of_targets").value_or(0.0)); }
      if(const auto mode_stats=object_after_key(level,"mode_stats")) if(const auto burst=object_after_key(*mode_stats,*kind==Kind::MultiGearTower?"fastAttack":"gearedUpBurst")) burst_damage=number_field(*burst,"damage_per_shot").value_or(0.0);
      double scattershot_direct_min_damage=0.0,scattershot_splash_max_damage=0.0,scattershot_splash_min_damage=0.0;
      if(*kind==Kind::Scattershot) if(const auto mode_stats=object_after_key(level,"mode_stats")) if(const auto normal=object_after_key(*mode_stats,"normal")) {
        scattershot_direct_min_damage=number_field(*normal,"damage_per_shot_min").value_or(0.0);
        scattershot_splash_max_damage=number_field(*normal,"splash_damage_max").value_or(0.0);
        scattershot_splash_min_damage=number_field(*normal,"splash_damage_min").value_or(0.0);
      }
      double aura_damage=0.0;
      if(*kind==Kind::ElectroTitan) if(const auto mode_stats=object_after_key(level,"mode_stats")) if(const auto aura=object_after_key(*mode_stats,"aura")) aura_damage=number_field(*aura,"damage_per_shot").value_or(0.0);
      // Town Hall armaments are nested in the source record. Read that object
      // intentionally: a flat regex match must not accidentally pick only its
      // `dps` while losing range, cadence, damage and target type.
      // `object_after_key` searches forward for an object; guard JSON null so
      // a null weapon cannot accidentally capture the following mode_stats.
      const auto weapon=level.find("\"weapon\": null")==std::string::npos?object_after_key(level,"weapon"):std::nullopt;
      const auto weapon_levels=weapon?array_body(*weapon,"levels"):std::nullopt;
      const auto weapon_level=weapon_levels?std::optional<std::string>(objects_in(*weapon_levels).empty()?std::string{}:objects_in(*weapon_levels).front()):std::nullopt;
      const auto weapon_number=[&](const std::string& key){return weapon_level&&!weapon_level->empty()?number_field(*weapon_level,key):std::nullopt;};
      // Town Hall armaments declare their simultaneous target count directly
      // on the nested weapon. Preserve it as immutable catalogue data so the
      // shared attack loop emits one deterministic projectile per target.
      const auto level_multi_target_count=weapon
        ? static_cast<int>(number_field(*weapon,"targets").value_or(multi_count))
        : (multi_count>0?multi_count:declared_multi_target_count);
      const auto level_damage=number_field(level,"damage_per_shot").value_or(number_field(level,"damage").value_or(weapon_number("damagePerHit").value_or(0.0)));
      const auto level_dps=weapon_number("dps").value_or(number_field(level,"dps").value_or(0.0));
      const auto level_range=weapon?number_field(*weapon,"range").value_or(range):range;
      const bool level_ranged=level_range>1.0&&!heals;
      const double level_attack_seconds=weapon?number_field(*weapon,"attackSpeed").value_or(attack_seconds):attack_seconds;
      const auto level_cooldown=static_cast<Milliseconds>(std::llround(level_attack_seconds*1000.0/kTickMs))*kTickMs;
      const auto level_target_type=weapon?target_type_from_catalogue(string_field(*weapon,"targetType").value_or(string_field(content,"target_type").value_or("ground"))):target_type;
      const auto level_death_damage=weapon?number_field(*weapon,"deathDamage").value_or(number_field(level,"death_damage").value_or(0.0)):number_field(level,"death_damage").value_or(0.0);
      const auto level_death_splash_radius=weapon?number_field(*weapon,"deathDamageRadius").value_or(death_splash_radius):death_splash_radius;
      const auto level_splash=number_field(level,"damage_radius_tiles").value_or(splash_radius);
      double inferno_initial_damage=0.0,inferno_second_damage=0.0,inferno_max_damage=0.0;
      if(inferno_second_stage_at>0&&inferno_max_stage_at>inferno_second_stage_at)
        if(const auto mode_stats=object_after_key(level,"mode_stats")) if(const auto normal=object_after_key(*mode_stats,"normal")) {
          inferno_initial_damage=number_field(*normal,"dps_initial").value_or(number_field(*normal,"dps").value_or(0.0))*level_attack_seconds;
          inferno_second_damage=number_field(*normal,"dps_after_1p5_seconds").value_or(0.0)*level_attack_seconds;
          inferno_max_damage=level_damage;
          if(inferno_second_damage<=0.0) if(const auto stage_two=object_after_key(*mode_stats,"stage2"))
            inferno_second_damage=number_field(*stage_two,"dps").value_or(0.0)*level_attack_seconds;
          if(inferno_max_damage<=0.0) if(const auto stage_three=object_after_key(*mode_stats,"stage3"))
            inferno_max_damage=number_field(*stage_three,"dps").value_or(0.0)*level_attack_seconds;
        }
      const auto spring_capacity=integer("spring_capacity");
      const auto effect_duration=static_cast<Milliseconds>(std::llround(number_field(level,"effect_duration_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
      const auto defense_invisibility_duration=static_cast<Milliseconds>(std::llround(number_field(content,"defense_invisibility_duration_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
      const auto level_wall_damage_multiplier=number_field(level,"wall_damage_multiplier").value_or(wall_damage_multiplier);
      const auto max_hp_damage_percent=number_field(level,"monolith_bonus_damage_percent").value_or(0.0);
      const auto wall_damage_per_hit=number_field(content,"wall_damage_per_hit").value_or(0.0);
      const auto wall_attack_cooldown=static_cast<Milliseconds>(std::llround(number_field(content,"wall_attack_speed_seconds").value_or(0.0)*1000.0/kTickMs))*kTickMs;
      const bool smashes_walls=bool_field(content,"smashes_walls").value_or(false);
      const auto piercing_projectile_range=number_field(content,"piercing_projectile_range_tiles").value_or(0.0);
      const auto chain_damage_multiplier=number_field(content,"chain_damage_multiplier").value_or(0.0);
      const auto chain_target_count=static_cast<int>(number_field(content,"chain_target_count").value_or(0.0));
      const auto chain_radius=number_field(content,"chain_radius_tiles").value_or(0.0);
      const auto opening_long_shot_count=static_cast<int>(number_field(content,"opening_long_shot_count").value_or(0.0));
      const auto opening_long_shot_range=number_field(content,"opening_long_shot_range_tiles").value_or(0.0);
      const auto opening_long_shot_damage_multiplier=number_field(content,"opening_long_shot_damage_multiplier").value_or(1.0);
      const auto bounce_impact_count=static_cast<int>(number_field(content,"bounce_impact_count").value_or(0.0));
      const auto bounce_step=number_field(content,"bounce_step_tiles").value_or(0.0);
      const auto bounce_splash_radius=number_field(content,"bounce_splash_radius_tiles").value_or(0.0);
      if(*kind==Kind::Scattershot&&(scattershot_direct_min_damage<=0.0||scattershot_splash_max_damage<=0.0||scattershot_splash_min_damage<=0.0||scattershot_cone_angle<=0.0||scattershot_cone_range<=0.0||scattershot_inner_range<=0.0||!scattershot_same_altitude_only)) throw std::runtime_error("incomplete Scattershot cone contract");
      if(*kind==Kind::ElectroTitan&&(aura_damage<=0.0||aura_range<=0.0||aura_cooldown<=0||!aura_excludes_walls||!aura_ignores_freeze||!aura_ignores_rage)) throw std::runtime_error("incomplete Electro Titan aura contract");
      d.stats_.push_back({*kind,static_cast<int>(*n),hp.value_or(1.0),level_damage,level_dps,number_field(level,"healing_per_second").value_or(0.0),level_death_damage,level_cooldown,level_range,speed,speed/kCatalogueMovementSpeedUnitsPerTilePerSecond,radius,level_ranged,splash,flying,heals,focus,level_target_type,category,footprint_w,footprint_h,housing_space,activation_housing_space,min_range,level_splash,shots_per_burst,time_between_bursts,alternate_range,alternate_target_type,level_death_splash_radius,death_damage_delay,trigger_range,reveal_destruction_percent,spring_capacity,std::move(attributes),multi_target_range,multi_damage,level_multi_target_count,burst_damage,burst_cooldown,burst_shots,burst_pause,burst_range,resource_damage_multiplier,max_hp_damage_percent,death_damage_ground_only,integer("parent_spawn_count"),integer("spawned_units"),integer("spawned_unit_level"),deployable,level_wall_damage_multiplier,self_destruct_on_attack,effect_duration,isolation_radius,rage_damage_multiplier,rage_attack_speed_multiplier,burrows,defense_invisibility_duration,wall_damage_per_hit,wall_attack_cooldown,smashes_walls,inferno_initial_damage,inferno_second_damage,inferno_max_damage,inferno_second_stage_at,inferno_max_stage_at,projectile_speed_tiles_per_second,projectile_speed_sourced,scattershot_direct_min_damage,scattershot_splash_max_damage,scattershot_splash_min_damage,scattershot_cone_angle,scattershot_cone_range,scattershot_inner_range,scattershot_same_altitude_only,aura_damage,aura_range,aura_cooldown,aura_excludes_walls,aura_ignores_freeze,aura_ignores_rage,deployment_rage_duration,deployment_rage_damage_multiplier,deployment_rage_movement_speed_multiplier,piercing_projectile_range,chain_damage_multiplier,chain_target_count,chain_radius,opening_long_shot_count,opening_long_shot_range,opening_long_shot_damage_multiplier,bounce_impact_count,bounce_step,bounce_splash_radius});
    }
  }
  if(const auto spell_entries=array_body(document,"spells")) for(const auto& spell:objects_in(*spell_entries)) {
    const auto id=string_field(spell,"id"); const auto kind=id?parse_spell(*id):std::nullopt; const auto levels=array_body(spell,"levels"); if(!kind||!levels) continue;
    const double radius=number_field(spell,"radius_tiles").value_or(0.0); const int pulses=static_cast<int>(number_field(spell,"number_of_pulses").value_or(1));
    const auto interval=static_cast<Milliseconds>(std::llround(number_field(spell,"time_between_pulses_seconds").value_or(0.0)*1000.0));
    const auto boost=static_cast<Milliseconds>(std::llround(number_field(spell,"boost_time_seconds").value_or(0.0)*1000.0));
    for(const auto& level:objects_in(*levels)) { const auto n=number_field(level,"level"); if(!n) continue; const auto level_radius=number_field(level,"radius_tiles").value_or(radius); const auto declared=static_cast<Milliseconds>(std::llround(number_field(level,"spell_duration_seconds").value_or(boost/1000.0)*1000.0)); const auto duration=std::max(declared, static_cast<Milliseconds>(pulses)*interval); d.spells_.push_back({*kind,static_cast<int>(*n),level_radius,number_field(level,"damage").value_or(0.0),number_field(level,"total_healing").value_or(0.0),number_field(level,"damage_increase").value_or(100.0)/100.0,1.0+number_field(level,"speed_increase").value_or(0.0)/100.0,duration,pulses,interval,number_field(level,"building_damage_percent").value_or(0.0),number_field(level,"troop_damage_percent").value_or(0.0)}); }
  }
  if(d.stats_.empty()) throw std::runtime_error("normalized catalogue contains no supported entries");
  cached=d;
  return d;
}
const Stats* GameData::find(Kind kind,int level,const std::string& variant) const { const bool supercharged=variant=="supercharged"; for(const auto& s:stats_) if(s.kind==kind&&s.level==level&&s.attributes.supercharged==supercharged) return &s; return nullptr; }
const NonCombatStats* GameData::find_non_combat(Kind kind) const { for(const auto& s:non_combat_) if(s.kind==kind) return &s; return nullptr; }
const SpellStats* GameData::find_spell(SpellKind kind,int level) const { for(const auto& s:spells_) if(s.kind==kind&&s.level==level) return &s; return nullptr; }
int GameData::max_level(Kind kind) const { int result=0; for(const auto& s:stats_) if(s.kind==kind) result=std::max(result,s.level); return result; }

} // namespace cocsim
