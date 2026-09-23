#include "cocsim/core.hpp"
#include "detail/battle_state_internal.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <limits>
#include <numeric>
#include <numbers>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>

namespace cocsim {
namespace {
bool is_home_hero(Kind kind) {
  return kind==Kind::BarbarianKing || kind==Kind::ArcherQueen
      || kind==Kind::GrandWarden || kind==Kind::RoyalChampion
      || kind==Kind::MinionPrince || kind==Kind::DragonDuke;
}
void validate_hero_loadouts(const GameData& data, const Scenario& scenario) {
  std::set<Kind> heroes;
  std::set<std::string> assigned_pets;
  std::set<std::string> assigned_equipment;
  const auto& catalogue = data.hero_support_catalogue();
  const auto find_choice = [&](const HeroSupportChoice& choice, const char* category) {
    const auto row = std::find_if(catalogue.begin(), catalogue.end(), [&](const HeroSupportCatalogEntry& entry) {
      return entry.id == choice.id && entry.category == category;
    });
    if (row == catalogue.end() || choice.level < 1 || choice.level > row->max_level)
      throw std::invalid_argument("unsupported Hero support item or level");
    return row;
  };
  for (const auto& loadout : scenario.hero_loadouts) {
    if (!is_home_hero(loadout.hero) || !heroes.insert(loadout.hero).second)
      throw std::invalid_argument("duplicate or unsupported Hero loadout");
    const auto slot = std::find_if(scenario.army.begin(), scenario.army.end(), [&](const ArmySlot& army) {
      return army.kind == loadout.hero;
    });
    if (slot == scenario.army.end() || slot->count != 1 ||
        std::count_if(scenario.army.begin(), scenario.army.end(), [&](const ArmySlot& army) {
          return army.kind == loadout.hero;
        }) != 1)
      throw std::invalid_argument("Hero loadout requires one unambiguous attacking Hero slot");
    if (loadout.pet) {
      find_choice(*loadout.pet, "pet");
      if (!assigned_pets.insert(loadout.pet->id).second)
        throw std::invalid_argument("duplicate Pet assignment is unsupported");
    }
    if (loadout.equipment.size() > 2)
      throw std::invalid_argument("Hero Equipment slot limit exceeded");
    for (const auto& choice : loadout.equipment) {
      const auto row = find_choice(choice, "hero-equipment");
      if (row->hero != to_string(loadout.hero) || !assigned_equipment.insert(choice.id).second)
        throw std::invalid_argument("invalid or duplicate Hero Equipment assignment");
    }
  }
}
}

int BattleState::monolith_arrow_deployed_housing() const {
  // The ordinary deployment counter excludes Heroes in the pinned catalogue.
  // Dead Heroes remain in entities_, so their deployed 25 housing still counts.
  // Spawned subunits never increment the ordinary deployment counter.
  const auto heroes=std::count_if(entities_.begin(),entities_.end(),[](const Entity& entity) {
    return entity.side==Side::Attacker && is_home_hero(entity.kind);
  });
  return deployed_housing_ + 25*static_cast<int>(heroes);
}

int BattleState::monolith_arrow_housing_tier() const {
  const int housing=monolith_arrow_deployed_housing();
  return housing<=180 ? 0 : housing<=250 ? 1 : 2;
}

int BattleState::monolith_arrow_damage_percent() const {
  // Supercell's June 2026 housing table gives these exact max-HP percentages.
  // This is a projection only: projectile timing and damage rounding are open.
  constexpr int percentages[] = {14, 10, 5};
  return percentages[monolith_arrow_housing_tier()];
}

bool BattleState::alive(const Entity& e) { return e.hp > 0.0; }
BattleState::~BattleState() = default;
BattleState::BattleState(BattleState&&) noexcept = default;
BattleState& BattleState::operator=(BattleState&&) noexcept = default;

BattleState::BattleState(GameData d, Scenario s):data_(std::move(d)),scenario_(std::move(s)) {
  validate_hero_loadouts(data_, scenario_);
  const int attacking_hero_slots=std::accumulate(scenario_.army.begin(),scenario_.army.end(),0,[&](int total,const ArmySlot& slot){return total+(is_home_hero(slot.kind)?slot.count:0);});
  if(attacking_hero_slots>4) throw std::invalid_argument("attacking Hero slot limit exceeded");
  for(const auto& p:scenario_.non_combat_obstacles) {
    const auto* meta=data_.find_non_combat(p.kind);
    if(!meta||p.position.x-meta->footprint_width/2.0<0||p.position.y-meta->footprint_height/2.0<0||p.position.x+meta->footprint_width/2.0>scenario_.width||p.position.y+meta->footprint_height/2.0>scenario_.height)
      throw std::invalid_argument("unsupported or out-of-bounds non-combat obstacle");
    if ((p.kind == Kind::HeroBanner && p.variant != "normal" && p.variant != "barbarian_king"
         && p.variant != "archer_queen" && p.variant != "grand_warden" && p.variant != "royal_champion"
         && p.variant != "minion_prince" && p.variant != "dragon_duke")
        || (p.kind != Kind::HeroBanner && p.variant != "normal"))
      throw std::invalid_argument("unsupported non-combat obstacle variant");
  }
  for(const auto& meta:data_.non_combat_) if(meta.max_placements>0
      && std::count_if(scenario_.non_combat_obstacles.begin(),scenario_.non_combat_obstacles.end(),[&](const auto& obstacle){return obstacle.kind==meta.kind;})>meta.max_placements)
    throw std::invalid_argument("non-combat obstacle quantity limit exceeded");
  for(const auto& p:scenario_.defenders) {
    const auto* st=data_.find(p.kind,p.level,p.variant);
    if(!st) throw std::invalid_argument("unsupported defender level or variant");
    Entity e;
    e.id=static_cast<EntityId>(next_id_++); e.kind=p.kind; e.side=Side::Defender; e.level=p.level; e.pos=p.position;
    e.supercharged=st->attributes.supercharged; e.hp=e.max_hp=st->hp; e.damage=st->damage; e.healing=st->healing_per_second;
    e.death_damage=st->death_damage; e.death_splash_radius=st->death_splash_radius; e.death_damage_delay=st->death_damage_delay;
    e.trigger_range=st->trigger_range;
    e.concealed=p.kind==Kind::HiddenTesla||p.kind==Kind::Bomb||p.kind==Kind::GiantBomb||p.kind==Kind::AirBomb||p.kind==Kind::SeekingAirMine||p.kind==Kind::SpringTrap||p.kind==Kind::GigaBomb||p.kind==Kind::TornadoTrap||p.kind==Kind::SkeletonTrap;
    e.cooldown=st->cooldown; e.range=st->range; e.min_range=st->min_range; e.speed=st->movement_tiles_per_second; e.radius=st->radius; e.splash_radius=st->splash_radius;
    e.time_between_bursts=st->time_between_bursts; e.shots_per_burst=st->shots_per_burst; e.opening_long_shots_remaining=st->opening_long_shot_count; e.activation_housing=st->activation_housing_space; e.next_aura_action=st->aura_cooldown>0?now_ms_+st->aura_cooldown:0;
    // Inferno Tower's multi-target count belongs exclusively to its selected
    // `multi_target` mode. Other nested weapons (notably Town Hall) expose
    // their count as the normal armament contract.
    e.multi_target_count=p.kind==Kind::InfernoTower?0:st->multi_target_count;
    e.ranged=st->ranged; e.splash=st->splash; e.flying=st->flying; e.heals=st->heals;
    e.target_focus=st->target_focus; e.target_type=st->target_type; e.defenses=st->target_focus==TargetFocus::Defenses;
    if (p.kind == Kind::Yeti) e.remaining_yetimites = st->spawned_units;
    if(p.kind==Kind::XBow&&p.mode=="air_and_ground") { e.range=st->alternate_range; e.target_type=st->alternate_target_type; }
    else if(p.kind==Kind::XBow&&p.mode!="normal") throw std::invalid_argument("unsupported X-Bow mode");
    if(p.kind==Kind::InfernoTower&&p.mode=="multi_target") { e.range=st->multi_target_range; e.damage=st->multi_target_damage; e.multi_target_count=st->multi_target_count; }
    else if(p.kind==Kind::InfernoTower&&p.mode!="normal") throw std::invalid_argument("unsupported Inferno Tower mode");
    if((p.kind==Kind::Mortar||p.kind==Kind::Cannon)&&p.mode=="geared_up_burst") {
      if(st->burst_damage<=0||st->burst_shots<=0||st->burst_cooldown<=0||st->burst_range<=0) throw std::invalid_argument("unsupported geared-up burst level");
      e.damage=st->burst_damage; e.cooldown=st->burst_cooldown; e.range=st->burst_range; e.shots_per_burst=st->burst_shots; e.time_between_bursts=st->burst_pause;
    } else if((p.kind==Kind::Mortar||p.kind==Kind::Cannon)&&p.mode!="normal") throw std::invalid_argument("unsupported defense mode");
    if(p.kind==Kind::MultiGearTower&&p.mode=="fast_attack") {
      if(st->burst_damage<=0||st->burst_shots<=0||st->burst_cooldown<=0||st->burst_range<=0) throw std::invalid_argument("unsupported Multi-Gear Tower fast-attack level");
      e.damage=st->burst_damage; e.cooldown=st->burst_cooldown; e.range=st->burst_range; e.shots_per_burst=st->burst_shots; e.time_between_bursts=st->burst_pause;
    } else if(p.kind==Kind::MultiGearTower&&p.mode!="normal") throw std::invalid_argument("unsupported Multi-Gear Tower mode");
    if(p.kind==Kind::ArcherTower&&p.mode!="normal") throw std::invalid_argument("unsupported Archer Tower mode: per-level geared-up damage is missing");
    if(p.kind==Kind::SkeletonTrap) {
      if(p.mode=="ground") e.target_type=TargetType::Ground;
      else if(p.mode=="air") e.target_type=TargetType::Air;
      else throw std::invalid_argument("Skeleton Trap mode must be ground or air");
    }
    entities_.push_back(e);
  }
}
void BattleState::emit(EventType t,EntityId a,EntityId b,double v,std::string d,ProjectileId projectile_id) { Event event{t,now_ms_,a,b,v,std::move(d)}; event.projectile_id=projectile_id; if(const auto* actor=entity(a)){event.origin=actor->pos;event.has_origin=true;} if(const auto* target=entity(b)){event.target_position=target->pos;event.has_target_position=true;} events_.push_back(std::move(event)); }
BattleState::Entity* BattleState::entity(EntityId id) { for(auto& e:entities_) if(e.id==id) return &e; return nullptr; } const BattleState::Entity* BattleState::entity(EntityId id) const { for(const auto& e:entities_) if(e.id==id) return &e; return nullptr; }
bool BattleState::submit(Command c,std::string* err) {
  if(c.requested_ms<=now_ms_ || c.requested_ms%kTickMs) { if(err)*err="command time must be a future multiple of 16 ms"; emit(EventType::Rejected,0,0,0,"unrepresentable command time"); return false; }
  // Older callers may populate `sequence` as a local field while leaving the
  // effective time at zero. That is an unscheduled command, not a replay
  // contract. A nonzero effective time is the unambiguous serialized form.
  const bool serialized_order = c.effective_ms != 0;
  if (serialized_order) {
    // Replays carry the exact Core schedule. No adapter is permitted to
    // reinterpret its timestamp or same-tick order while loading it.
    if (c.effective_ms != c.requested_ms || c.sequence == 0
        || std::any_of(commands_.begin(), commands_.end(), [&](const Command& existing) { return existing.sequence == c.sequence; })) {
      if(err)*err="invalid serialized command schedule";
      emit(EventType::Rejected,0,0,0,"invalid serialized command schedule");
      return false;
    }
  } else {
    c.effective_ms=c.requested_ms;
    std::uint64_t highest_sequence{};
    for (const auto& existing : commands_) highest_sequence = std::max(highest_sequence, existing.sequence);
    c.sequence=highest_sequence+1;
  }
  commands_.push_back(c);
  std::stable_sort(commands_.begin()+static_cast<std::ptrdiff_t>(command_cursor_),commands_.end(),[](auto&a,auto&b){return a.effective_ms==b.effective_ms?a.sequence<b.sequence:a.effective_ms<b.effective_ms;});
  emit(EventType::Accepted,0,0,0,"command accepted");
  return true;
}
void BattleState::apply_due_commands() { while(command_cursor_<commands_.size() && commands_[command_cursor_].effective_ms==now_ms_) { const auto& c=commands_[command_cursor_++]; if(c.type==CommandType::Deploy) deploy(c); else if(c.type==CommandType::CastSpell) cast_spell(c); else if(c.type==CommandType::EndBattle) { finished_=true; ended_by_player_=true; emit(EventType::Finished,0,0,0,"player ended battle"); return; } } }
void BattleState::deploy(const Command& c) {
  const auto* st=data_.find(c.kind,c.level);
  if(!st || !st->deployable || c.position.x<0||c.position.y<0||c.position.x>=scenario_.width||c.position.y>=scenario_.height) { emit(EventType::Rejected,0,0,0,"unsupported troop or invalid deployment"); return; }
  for(const auto& obstacle:scenario_.non_combat_obstacles) { const auto* meta=data_.find_non_combat(obstacle.kind); if(meta&&c.position.x>=obstacle.position.x-meta->footprint_width/2.0-meta->deployment_margin&&c.position.x<obstacle.position.x+meta->footprint_width/2.0+meta->deployment_margin&&c.position.y>=obstacle.position.y-meta->footprint_height/2.0-meta->deployment_margin&&c.position.y<obstacle.position.y+meta->footprint_height/2.0+meta->deployment_margin) { emit(EventType::Rejected,0,0,0,"deployment blocked by non-combat obstacle"); return; } }
  // Every targetable building, including each wall cell, owns the same one-tile
  // deployment exclusion ring. Any point outside those Core rectangles is
  // deployable; Viewer, CLI, replay and RL never maintain their own map.
  for(const auto& defender:entities_) if(alive(defender)&&defender.side==Side::Defender) { const auto* defender_stats=data_.find(defender.kind,defender.level); if(defender_stats&&defender_stats->category==EntityCategory::Trap) continue; const double width=defender_stats?defender_stats->footprint_width:1.0, height=defender_stats?defender_stats->footprint_height:1.0; constexpr double margin=1.0; if(c.position.x>=defender.pos.x-width/2.0-margin&&c.position.x<defender.pos.x+width/2.0+margin&&c.position.y>=defender.pos.y-height/2.0-margin&&c.position.y<defender.pos.y+height/2.0+margin) { emit(EventType::Rejected,0,defender.id,0,"deployment blocked within one tile of defender"); return; } }
  auto slot=std::find_if(scenario_.army.begin(),scenario_.army.end(),[&](auto&s){return s.kind==c.kind&&s.level==c.level&&s.count>0;}); if(slot==scenario_.army.end()) { emit(EventType::Rejected,0,0,0,"army reserve exhausted"); return; }
  const bool warden_air = c.kind == Kind::GrandWarden && slot->mode == "air";
  if ((c.kind == Kind::GrandWarden && slot->mode != "normal" && slot->mode != "ground" && slot->mode != "air")
      || (c.kind != Kind::GrandWarden && slot->mode != "normal")) { emit(EventType::Rejected,0,0,0,"unsupported army mode"); return; }
  --slot->count; deployed_housing_+=st->housing_space; Entity e; e.id=static_cast<EntityId>(next_id_++);e.kind=c.kind;e.side=Side::Attacker;e.level=c.level;e.pos=c.position;e.hp=e.max_hp=st->hp;e.damage=st->damage;e.healing=st->healing_per_second;e.death_damage=st->death_damage;e.death_splash_radius=st->death_splash_radius;e.death_damage_delay=st->death_damage_delay;e.trigger_range=st->trigger_range;e.cooldown=st->cooldown;e.range=st->range;e.min_range=st->min_range;e.speed=st->movement_tiles_per_second;e.radius=st->radius;e.splash_radius=st->splash_radius;e.time_between_bursts=st->time_between_bursts;e.activation_housing=st->activation_housing_space;e.shots_per_burst=st->shots_per_burst;e.opening_long_shots_remaining=st->opening_long_shot_count;e.invisible_to_defenses_until=now_ms_+st->defense_invisibility_duration;e.next_aura_action=st->aura_cooldown>0?now_ms_+st->aura_cooldown:0;e.deployment_rage_until=st->deployment_rage_duration>0?now_ms_+st->deployment_rage_duration:0;e.druid_transform_at=st->druid_transform_after>0?now_ms_+st->druid_transform_after:0;e.ranged=st->ranged;e.splash=st->splash;e.flying=st->flying||warden_air;e.heals=st->heals;e.target_focus=st->target_focus;e.target_type=st->target_type;e.defenses=st->target_focus==TargetFocus::Defenses;if(c.kind==Kind::Yeti)e.remaining_yetimites=st->spawned_units;entities_.push_back(e); emit(EventType::Deployed,e.id,0,0,to_string(c.kind));
}
void BattleState::finish_if_needed(){ if(finished_)return; bool defs=false,att=false,troop_reserve=false,spell_reserve=false;for(const auto&e:entities_){if(!alive(e))continue;if(e.side==Side::Defender)defs=true;else att=true;}for(const auto&s:scenario_.army)troop_reserve|=s.count>0;for(const auto&s:scenario_.spells)spell_reserve|=s.count>0;if(!defs&&projectiles_.empty()&&death_explosions_.empty()){finished_=true;attackers_win_=true;emit(EventType::Finished,0,0,0,"all defenders destroyed");}else if(!att&&!troop_reserve&&!spell_reserve&&!projectiles_.size()&&death_explosions_.empty()){finished_=true;emit(EventType::Finished,0,0,0,"no attackers or reserves");}else if(now_ms_>=scenario_.duration_ms){finished_=true;timed_out_=true;emit(EventType::Finished,0,0,0,"scenario time limit");} }
void BattleState::tick(){ if(finished_)return;apply_due_commands();if(finished_)return;update_spells();update_projectiles();update_death_explosions();update_traps();update_tornado_traps();update_entities();resolve_spawns();finish_if_needed();if(!finished_){now_ms_+=kTickMs;finish_if_needed();} }
Milliseconds BattleState::advance_ticks(std::uint32_t n){const auto start=now_ms_;for(std::uint32_t i=0;i<n&&!finished_;++i)tick();return now_ms_-start;} Milliseconds BattleState::advance_to(Milliseconds t){if(t<now_ms_||t%kTickMs)throw std::invalid_argument("advance target must be a future multiple of 16 ms");return advance_ticks(static_cast<std::uint32_t>((t-now_ms_)/kTickMs));}
std::vector<EntityView> BattleState::observe()const{std::vector<EntityView>o;for(auto&e:entities_)if(alive(e)&&!e.concealed){const auto* stats=data_.find(e.kind,e.level,e.supercharged?"supercharged":"normal");o.push_back({e.id,e.kind,e.side,e.level,e.pos,e.hp,e.max_hp,e.range,e.target,e.next_action,stats?stats->category:EntityCategory::Other,stats?stats->target_focus:TargetFocus::Any,e.flying,e.heals,stats?stats->footprint_width:1,stats?stats->footprint_height:1,e.supercharged,e.underground,e.invisible_to_defenses_until>now_ms_,e.deployment_rage_until>now_ms_});}return o;}
std::vector<SpellEffectView> BattleState::observe_spell_effects() const { std::vector<SpellEffectView> result; for(const auto& effect:spell_effects_) result.push_back({effect.kind,effect.pos,effect.radius,effect.expires}); return result; }
std::vector<DeathExplosionView> BattleState::observe_death_explosions() const { std::vector<DeathExplosionView> result; result.reserve(death_explosions_.size()); for(const auto& effect:death_explosions_) result.push_back({effect.position,effect.radius,effect.impact,effect.ground_only}); return result; }
std::vector<ProjectileView> BattleState::observe_projectiles() const { std::vector<ProjectileView> result; result.reserve(projectiles_.size()); for(const auto& projectile:projectiles_) { const auto* target=entity(projectile.target); const auto* owner=entity(projectile.owner); const auto* owner_stats=owner?data_.find(owner->kind,owner->level):nullptr; result.push_back({projectile.id,projectile.owner,projectile.target,projectile.origin,projectile.position,projectile.fixed_impact?projectile.endpoint:(target?target->pos:projectile.position),projectile.speed_tiles_per_second,projectile.launched,projectile.impact,owner_stats&&owner_stats->projectile_speed_sourced,projectile.homing}); } return result; }
BattleResult BattleState::result()const{int initial=0,dead=0,troops=0,reserve=0,remaining_housing_space=0;bool has_town_hall=false,town_hall_destroyed=false;for(const auto&e:entities_){if(e.side==Side::Defender){++initial;if(!alive(e)){++dead;if(e.kind==Kind::TownHall)town_hall_destroyed=true;}if(e.kind==Kind::TownHall)has_town_hall=true;}else if(alive(e)){++troops;const auto* stats=data_.find(e.kind,e.level,e.supercharged?"supercharged":"normal");if(stats&&stats->deployable)remaining_housing_space+=stats->housing_space;}}for(const auto& slot:scenario_.army){reserve+=slot.count;const auto* stats=data_.find(slot.kind,slot.level);if(stats&&stats->deployable)remaining_housing_space+=slot.count*stats->housing_space;}const double destruction=initial?100.0*dead/initial:100.0;const int stars=(has_town_hall&&town_hall_destroyed?1:0)+(destruction>=50.0?1:0)+(destruction>=100.0?1:0);const bool player_ended=ended_by_player_||(finished_&&command_cursor_>0&&commands_[command_cursor_-1].type==CommandType::EndBattle);return {finished_,attackers_win_,timed_out_,now_ms_,destruction,stars,troops+reserve,remaining_housing_space,std::max<Milliseconds>(0,scenario_.duration_ms-now_ms_),finished_?(attackers_win_?"victory":timed_out_?"time limit":player_ended?"player ended battle":"army exhausted"):"active"};}
} // namespace cocsim
