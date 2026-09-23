#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "cocsim/core.hpp"

namespace py = pybind11;
using namespace cocsim;

namespace {
Scenario defaults(std::uint64_t seed) {
  Scenario s; s.seed=seed; s.width=20; s.height=20; s.duration_ms=30000; s.defenders={{Kind::Cannon,1,{12,10}}};
  s.army={{Kind::Barbarian,1,10},{Kind::Archer,1,5},{Kind::Giant,1,2},{Kind::Wizard,1,2},{Kind::WallBreaker,1,2},{Kind::Golem,1,1},{Kind::Pekka,1,1},{Kind::Balloon,1,2},{Kind::Dragon,1,1},{Kind::BabyDragon,1,1},{Kind::Miner,1,1},{Kind::Healer,1,1},{Kind::Goblin,1,4},{Kind::Minion,1,2},{Kind::HogRider,1,2},{Kind::Valkyrie,1,2},{Kind::DragonRider,1,1}};
  s.spells={{SpellKind::Rage,1,1},{SpellKind::Heal,1,1},{SpellKind::Lightning,1,1},{SpellKind::Freeze,1,1},{SpellKind::Haste,1,1},{SpellKind::Jump,1,1},{SpellKind::Earthquake,1,1}}; return s;
}
Scenario from_py(const py::dict& in,std::uint64_t seed) {
  Scenario s; s.seed=seed; s.width=in.contains("width")?py::cast<int>(in["width"]):20; s.height=in.contains("height")?py::cast<int>(in["height"]):20; s.duration_ms=in.contains("duration_ms")?py::cast<Milliseconds>(in["duration_ms"]):30000;
  if(s.width<=0||s.height<=0||s.duration_ms<=0) throw std::invalid_argument("invalid scenario dimensions");
  for(const auto& value:py::cast<py::list>(in["defenders"])){const auto d=py::cast<py::dict>(value);const auto k=parse_kind(py::cast<std::string>(d["kind"]));if(!k)throw std::invalid_argument("unknown defender");s.defenders.push_back({*k,py::cast<int>(d["level"]),{py::cast<double>(d["x"]),py::cast<double>(d["y"])},d.contains("mode")?py::cast<std::string>(d["mode"]):"normal",d.contains("variant")?py::cast<std::string>(d["variant"]):"normal"});}
  if(in.contains("non_combat_obstacles"))for(const auto& value:py::cast<py::list>(in["non_combat_obstacles"])){const auto d=py::cast<py::dict>(value);const auto k=parse_kind(py::cast<std::string>(d["kind"]));if(!k)throw std::invalid_argument("unknown non-combat obstacle");s.non_combat_obstacles.push_back({*k,{py::cast<double>(d["x"]),py::cast<double>(d["y"])},d.contains("variant")?py::cast<std::string>(d["variant"]):"normal"});}
  for(const auto& value:py::cast<py::list>(in["army"])){const auto d=py::cast<py::dict>(value);const auto k=parse_kind(py::cast<std::string>(d["kind"]));if(!k)throw std::invalid_argument("unknown army");s.army.push_back({*k,py::cast<int>(d["level"]),py::cast<int>(d["count"]),d.contains("mode")?py::cast<std::string>(d["mode"]):"normal"});}
  if(in.contains("hero_loadouts"))for(const auto& value:py::cast<py::list>(in["hero_loadouts"])){const auto d=py::cast<py::dict>(value);const auto hero=parse_kind(py::cast<std::string>(d["hero"]));if(!hero)throw std::invalid_argument("unknown Hero loadout");HeroLoadout row;row.hero=*hero;if(d.contains("pet")){const auto pet=py::cast<py::dict>(d["pet"]);row.pet=HeroSupportChoice{py::cast<std::string>(pet["id"]),py::cast<int>(pet["level"])};}if(d.contains("equipment"))for(const auto& item:py::cast<py::list>(d["equipment"])){const auto equipment=py::cast<py::dict>(item);row.equipment.push_back({py::cast<std::string>(equipment["id"]),py::cast<int>(equipment["level"])});}s.hero_loadouts.push_back(std::move(row));}
  if(in.contains("spells"))for(const auto& value:py::cast<py::list>(in["spells"])){const auto d=py::cast<py::dict>(value);const auto k=parse_spell(py::cast<std::string>(d["kind"]));if(!k)throw std::invalid_argument("unknown spell");s.spells.push_back({*k,py::cast<int>(d["level"]),py::cast<int>(d["count"])});}
  return s;
}
const char* event_name(EventType type) {
  switch (type) {
    case EventType::Accepted: return "accepted";
    case EventType::Rejected: return "rejected";
    case EventType::Deployed: return "deployed";
    case EventType::TargetChanged: return "target_changed";
    case EventType::Attack: return "attack";
    case EventType::Projectile: return "projectile";
    case EventType::Impact: return "impact";
    case EventType::Damaged: return "damaged";
    case EventType::Healed: return "healed";
    case EventType::Destroyed: return "destroyed";
    case EventType::Finished: return "finished";
  }
  return "invalid";
}
class NativeBattle {
 public:
  NativeBattle():state_(GameData::v0(),defaults(1)){}
  void reset(std::uint64_t seed){state_=BattleState(GameData::v0(),defaults(seed));}
  void reset_scenario(const py::dict& scenario,std::uint64_t seed){state_=BattleState(GameData::v0(),from_py(scenario,seed));}
  bool deploy(const std::string& name,double x,double y){const auto kind=parse_kind(name);if(!kind)throw std::invalid_argument("unknown troop");const auto slot=std::find_if(state_.scenario().army.begin(),state_.scenario().army.end(),[&](const ArmySlot& row){return row.kind==*kind&&row.count>0;});return slot!=state_.scenario().army.end()&&state_.submit({CommandType::Deploy,state_.now()+kTickMs,0,0,*kind,slot->level,{x,y}});}
  bool cast_spell(const std::string& name,double x,double y){const auto kind=parse_spell(name);if(!kind)throw std::invalid_argument("unknown spell");const auto slot=std::find_if(state_.scenario().spells.begin(),state_.scenario().spells.end(),[&](const SpellSlot& row){return row.kind==*kind&&row.count>0;});if(slot==state_.scenario().spells.end())return false;Command command;command.type=CommandType::CastSpell;command.requested_ms=state_.now()+kTickMs;command.level=slot->level;command.position={x,y};command.spell=*kind;return state_.submit(command);}
  bool end_battle(){return state_.submit({CommandType::EndBattle,state_.now()+kTickMs});}
  std::int64_t advance(int ticks){py::gil_scoped_release release;return state_.advance_ticks(static_cast<std::uint32_t>(ticks));}
  py::dict availability()const{py::dict result,troops,spells;for(const auto& slot:state_.scenario().army){const py::str name(to_string(slot.kind));troops[name]=troops.contains(name)?py::cast<int>(troops[name])+slot.count:slot.count;}for(const auto& slot:state_.scenario().spells){const py::str name(to_string(slot.kind));spells[name]=spells.contains(name)?py::cast<int>(spells[name])+slot.count:slot.count;}result["troops"]=troops;result["spells"]=spells;return result;}
  py::dict observation() const {
    py::dict result;
    py::list entities, obstacles, spell_effects, death_explosions, projectiles;
    for (const auto& entity : state_.observe()) {
      py::dict row;
      row["id"] = entity.id; row["kind"] = to_string(entity.kind); row["level"] = entity.level;
      row["side"] = entity.side == Side::Attacker ? "attacker" : "defender";
      row["x"] = entity.position.x; row["y"] = entity.position.y;
      row["hp"] = entity.hp; row["max_hp"] = entity.max_hp;
      row["supercharged"] = entity.supercharged; row["underground"] = entity.underground;
      row["flying"] = entity.flying;
      row["invisible_to_defenses"] = entity.invisible_to_defenses;
      row["deployment_rage_active"] = entity.deployment_rage_active;
      entities.append(row);
    }
    for (const auto& obstacle : state_.scenario().non_combat_obstacles) {
      py::dict row;
      row["kind"] = to_string(obstacle.kind); row["x"] = obstacle.position.x; row["y"] = obstacle.position.y;
      row["variant"] = obstacle.variant;
      obstacles.append(row);
    }
    for (const auto& effect : state_.observe_spell_effects()) {
      py::dict row;
      row["kind"] = to_string(effect.kind); row["x"] = effect.position.x; row["y"] = effect.position.y;
      row["radius_tiles"] = effect.radius; row["expires_ms"] = effect.expires_ms;
      spell_effects.append(row);
    }
    for (const auto& effect : state_.observe_death_explosions()) {
      py::dict row;
      row["x"] = effect.position.x; row["y"] = effect.position.y;
      row["radius_tiles"] = effect.radius; row["impact_ms"] = effect.impact_ms;
      row["ground_only"] = effect.ground_only;
      death_explosions.append(row);
    }
    for (const auto& projectile : state_.observe_projectiles()) {
      py::dict row;
      row["id"] = projectile.id; row["owner_id"] = projectile.owner; row["target_id"] = projectile.target;
      row["origin_x"] = projectile.origin.x; row["origin_y"] = projectile.origin.y;
      row["x"] = projectile.position.x; row["y"] = projectile.position.y;
      // This endpoint is derived from the immutable Core read model.  It lets
      // RL clients observe homing intent without gaining any mutation path.
      row["target_x"] = projectile.target_position.x; row["target_y"] = projectile.target_position.y;
      row["speed_tiles_per_second"] = projectile.speed_tiles_per_second; row["speed_sourced"] = projectile.speed_sourced;
      row["launched_ms"] = projectile.launched_ms; row["scheduled_impact_ms"] = projectile.scheduled_impact_ms; row["homing"] = projectile.homing;
      projectiles.append(row);
    }
    result["entities"] = entities; result["non_combat_obstacles"] = obstacles;
    result["spell_effects"] = spell_effects; result["death_explosions"] = death_explosions;
    result["projectiles"] = projectiles; result["time_ms"] = state_.now();
    py::list loadouts;
    for (const auto& choice : state_.scenario().hero_loadouts) {
      py::dict row;
      row["hero"] = to_string(choice.hero);
      if (choice.pet) { py::dict pet; pet["id"] = choice.pet->id; pet["level"] = choice.pet->level; row["pet"] = pet; }
      py::list equipment;
      for (const auto& item : choice.equipment) { py::dict equipped; equipped["id"] = item.id; equipped["level"] = item.level; equipment.append(equipped); }
      row["equipment"] = equipment;
      loadouts.append(row);
    }
    result["hero_loadouts"] = loadouts;
    result["monolith_arrow_housing_projection"] = state_.monolith_arrow_deployed_housing();
    result["monolith_arrow_tier_projection"] = state_.monolith_arrow_housing_tier();
    result["monolith_arrow_damage_percent_projection"] = state_.monolith_arrow_damage_percent();
    result["state_hash"] = state_.state_hash();
    return result;
  }
  py::list projectile_views()const{
    py::list result;
    for(const auto& projectile:state_.observe_projectiles()){
      py::dict row; row["id"]=projectile.id; row["owner_id"]=projectile.owner; row["target_id"]=projectile.target;
      row["origin_x"]=projectile.origin.x; row["origin_y"]=projectile.origin.y; row["x"]=projectile.position.x; row["y"]=projectile.position.y;
      row["target_x"]=projectile.target_position.x; row["target_y"]=projectile.target_position.y; row["speed_tiles_per_second"]=projectile.speed_tiles_per_second; row["speed_sourced"]=projectile.speed_sourced;
      row["launched_ms"]=projectile.launched_ms; row["scheduled_impact_ms"]=projectile.scheduled_impact_ms; row["homing"]=projectile.homing; result.append(row);
    }
    return result;
  }
  py::dict events_since(std::size_t cursor) const {
    const auto& events = state_.events();
    if (cursor > events.size()) throw std::invalid_argument("event cursor is beyond the current log");
    py::list rows;
    for (std::size_t index = cursor; index < events.size(); ++index) {
      const auto& event = events[index];
      py::dict row;
      row["index"] = index; row["type"] = event_name(event.type); row["projectile_id"] = event.projectile_id;
      row["time_ms"] = event.time_ms; row["actor_id"] = event.actor;
      row["target_id"] = event.target; row["value"] = event.value;
      row["detail"] = event.detail;
      row["origin_x"] = event.origin.x; row["origin_y"] = event.origin.y;
      row["target_x"] = event.target_position.x; row["target_y"] = event.target_position.y;
      row["has_origin"] = event.has_origin; row["has_target_position"] = event.has_target_position;
      rows.append(row);
    }
    py::dict result;
    result["next_cursor"] = events.size(); result["events"] = rows;
    return result;
  }
  py::dict result()const{const auto value=state_.result();py::dict row;row["finished"]=value.finished;row["attackers_win"]=value.attackers_win;row["timed_out"]=value.timed_out;row["time_ms"]=value.time_ms;row["destruction"]=value.destruction;row["stars"]=value.stars;row["troops_remaining"]=value.troops_remaining;row["remaining_housing_space"]=value.remaining_housing_space;row["remaining_time_ms"]=value.remaining_time_ms;row["reason"]=value.reason;return row;}
 private: BattleState state_;
};
py::dict stats_row(const Stats& stat){
  py::dict row;
  row["kind"]=to_string(stat.kind); row["level"]=stat.level; row["supercharged"]=stat.attributes.supercharged;
  row["hp"]=stat.hp; row["damage"]=stat.damage; row["dps"]=stat.dps; row["healing_per_second"]=stat.healing_per_second;
  row["cooldown_ms"]=stat.cooldown; row["damage_spawn_threshold"]=stat.damage_spawn_threshold;
  row["defense_damage_multiplier"]=stat.defense_damage_multiplier; row["jumps_walls"]=stat.jumps_walls;
  const bool geared_up_burst_available=stat.burst_damage>0&&stat.burst_cooldown>0&&stat.burst_shots>0&&stat.burst_range>0;
  row["geared_up_burst_available"]=geared_up_burst_available; row["geared_up_burst_damage"]=stat.burst_damage;
  row["geared_up_burst_cooldown_ms"]=stat.burst_cooldown; row["geared_up_burst_shots"]=stat.burst_shots;
  row["geared_up_burst_pause_ms"]=stat.burst_pause; row["geared_up_burst_range_tiles"]=stat.burst_range;
  row["first_burst_delay_ms"]=stat.attributes.first_burst_delay; row["range_tiles"]=stat.range;
  row["movement_speed"]=stat.speed; row["movement_tiles_per_second"]=stat.movement_tiles_per_second;
  row["projectile_speed_tiles_per_second"]=stat.projectile_speed_tiles_per_second; row["projectile_speed_sourced"]=stat.projectile_speed_sourced;
  row["footprint_width"]=stat.footprint_width; row["footprint_height"]=stat.footprint_height;
  row["target_focus"]=to_string(stat.target_focus); row["target_type"]=to_string(stat.target_type);
  row["ranged"]=stat.ranged; row["splash"]=stat.splash; row["flying"]=stat.flying; row["heals"]=stat.heals;
  row["housing_space"]=stat.housing_space; row["activation_housing_space"]=stat.activation_housing_space;
  row["min_range_tiles"]=stat.min_range; row["splash_radius_tiles"]=stat.splash_radius;
  row["trigger_range_tiles"]=stat.trigger_range; row["reveal_destruction_percent"]=stat.reveal_destruction_percent;
  row["death_damage"]=stat.death_damage; row["death_splash_radius_tiles"]=stat.death_splash_radius;
  row["death_damage_delay_ms"]=stat.death_damage_delay; row["death_damage_ground_only"]=stat.death_damage_ground_only;
  row["spring_capacity"]=stat.spring_capacity; row["effect_duration_ms"]=stat.effect_duration;
  row["wall_damage_multiplier"]=stat.wall_damage_multiplier; row["wall_damage_per_hit"]=stat.wall_damage_per_hit;
  row["wall_attack_cooldown_ms"]=stat.wall_attack_cooldown; row["smashes_walls"]=stat.smashes_walls;
  row["resource_damage_multiplier"]=stat.resource_damage_multiplier; row["self_destruct_on_attack"]=stat.self_destruct_on_attack;
  row["isolation_radius_tiles"]=stat.isolation_radius; row["rage_damage_multiplier"]=stat.rage_damage_multiplier;
  row["rage_attack_speed_multiplier"]=stat.rage_attack_speed_multiplier; row["burrows"]=stat.burrows;
  row["defense_invisibility_duration_ms"]=stat.defense_invisibility_duration;
  row["deployment_rage_duration_ms"]=stat.deployment_rage_duration;
  row["deployment_rage_damage_multiplier"]=stat.deployment_rage_damage_multiplier;
  row["deployment_rage_movement_speed_multiplier"]=stat.deployment_rage_movement_speed_multiplier;
  return row;
}
py::list hero_support_rows() {
  const auto data = GameData::v0();
  py::list result;
  for (const auto& entry : data.hero_support_catalogue()) {
    py::dict row;
    row["id"] = entry.id;
    row["category"] = entry.category;
    row["hero"] = entry.hero;
    row["max_level"] = entry.max_level;
    row["support"] = entry.support;
    row["ability_type"] = entry.ability_type;
    result.append(row);
  }
  return result;
}
py::dict hero_support_level_row(const std::string& id, int level) {
  const auto data = GameData::v0();
  const auto* entry = data.find_hero_support(id);
  const auto* row = data.find_hero_support_level(id, level);
  if (!entry || !row) throw std::invalid_argument("unknown Hero support ID or level");
  const auto loads = py::module_::import("json").attr("loads");
  py::dict result;
  result["combat_status"] = "catalogued_only";
  result["content"] = loads(entry->normalized_content_json);
  result["level_record"] = loads(row->normalized_row_json);
  return result;
}
} // namespace

PYBIND11_MODULE(_cocsim,module){py::class_<NativeBattle>(module,"NativeBattle").def(py::init<>()).def("reset",&NativeBattle::reset).def("reset_scenario",&NativeBattle::reset_scenario).def("deploy",&NativeBattle::deploy).def("cast_spell",&NativeBattle::cast_spell).def("end_battle",&NativeBattle::end_battle).def("advance",&NativeBattle::advance).def("availability",&NativeBattle::availability).def("observation",&NativeBattle::observation).def("projectile_views",&NativeBattle::projectile_views).def("events_since",&NativeBattle::events_since).def("result",&NativeBattle::result);module.def("catalogue_stats",[]{const auto data=GameData::v0();py::list result;for(const auto& stat:data.supported())result.append(stats_row(stat));return result;});module.def("catalogue_spells",[]{const auto data=GameData::v0();py::list result;for(const auto& spell:data.supported_spells()){py::dict row;row["kind"]=to_string(spell.kind);row["level"]=spell.level;row["radius_tiles"]=spell.radius;row["damage"]=spell.damage;row["total_healing"]=spell.total_healing;row["damage_multiplier"]=spell.damage_multiplier;row["speed_multiplier"]=spell.speed_multiplier;row["duration_ms"]=spell.duration;row["pulses"]=spell.pulses;row["pulse_interval_ms"]=spell.pulse_interval;row["building_damage_percent"]=spell.building_damage_percent;row["troop_damage_percent"]=spell.troop_damage_percent;result.append(row);}return result;});module.def("hero_support_catalogue",&hero_support_rows);module.def("hero_support_level",&hero_support_level_row);module.attr("tick_ms")=kTickMs;}
