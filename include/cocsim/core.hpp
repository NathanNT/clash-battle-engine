#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace cocsim {
using Milliseconds = std::int64_t;
using EntityId = std::uint32_t;
using ProjectileId = std::uint32_t;
constexpr Milliseconds kTickMs = 10; // rules-v0: all events are quantised to this interval.
constexpr int kHomeVillageBuildTiles = 44;
constexpr int kHomeVillageDeploymentBorderTiles = 3;
constexpr int kHomeVillageTotalTiles = kHomeVillageBuildTiles + 2 * kHomeVillageDeploymentBorderTiles;

enum class Kind : std::uint8_t {
#define COCSIM_KIND(symbol, serialized_name) symbol,
#include "cocsim/detail/content_ids.inc"
#undef COCSIM_KIND
  Count
};
enum class Side : std::uint8_t { Attacker, Defender };
enum class EntityCategory : std::uint8_t { Troop, Defense, Resource, Wall, Other, Trap };
enum class TargetFocus : std::uint8_t { Any, Defenses, Resources, Walls, FriendlyTroops, DefensesOnly };
enum class TargetType : std::uint8_t { Ground, Air, Both };
// Count is a sentinel, never a serialised spell.  Keeping the bound separate
// from the final concrete spell avoids invalid snapshot/replay ranges when
// another source-validated Home Village spell is added.
enum class SpellKind : std::uint8_t {
#define COCSIM_SPELL(symbol, serialized_name) symbol,
#include "cocsim/detail/spell_ids.inc"
#undef COCSIM_SPELL
  Count
};
enum class CommandType : std::uint8_t { Deploy, CastSpell, Wait, EndBattle };
enum class EventType : std::uint8_t { Accepted, Rejected, Deployed, TargetChanged, Attack, Projectile, Impact, Damaged, Healed, Destroyed, Finished };

struct Vec2 { double x{}; double y{}; };
// Exact per-level metadata carried from the pinned catalogue.  A zero or an
// empty string means that the upstream entry does not define that attribute.
struct LevelAttributes {
  int town_hall_required{};
  int laboratory_required{};
  double upgrade_cost{};
  std::string upgrade_cost_resource;
  Milliseconds upgrade_time{};
  int xp_gained{};
  double capacity{};
  double production_rate{};
  int max_buildings{};
  int max_traps{};
  double gold_storage{};
  double elixir_storage{};
  double dark_elixir_storage{};
  int wall_rings{};
  Milliseconds first_burst_delay{};
  bool supercharged{};
  std::string image;
};
struct Stats { Kind kind{}; int level{}; double hp{}; double damage{}; double dps{}; double healing_per_second{}; double death_damage{}; Milliseconds cooldown{}; double range{}; double speed{}; double movement_tiles_per_second{}; double radius{0.45}; bool ranged{}; bool splash{}; bool flying{}; bool heals{}; TargetFocus target_focus{TargetFocus::Any}; TargetType target_type{TargetType::Ground}; EntityCategory category{EntityCategory::Other}; int footprint_width{1}; int footprint_height{1}; int housing_space{}; int activation_housing_space{}; double min_range{}; double splash_radius{}; int shots_per_burst{1}; Milliseconds time_between_bursts{}; double alternate_range{}; TargetType alternate_target_type{TargetType::Ground}; double death_splash_radius{}; Milliseconds death_damage_delay{}; double trigger_range{}; double reveal_destruction_percent{}; int spring_capacity{}; LevelAttributes attributes{}; double multi_target_range{}; double multi_target_damage{}; int multi_target_count{}; double burst_damage{}; Milliseconds burst_cooldown{}; int burst_shots{}; Milliseconds burst_pause{}; double burst_range{}; double resource_damage_multiplier{1.0}; double max_hp_damage_percent{}; bool death_damage_ground_only{}; int parent_spawn_count{}; int spawned_units{}; int spawned_unit_level{}; bool deployable{true}; double wall_damage_multiplier{1.0}; bool self_destruct_on_attack{}; Milliseconds effect_duration{}; double isolation_radius{}; double rage_damage_multiplier{1.0}; double rage_attack_speed_multiplier{1.0}; bool burrows{}; Milliseconds defense_invisibility_duration{}; double wall_damage_per_hit{}; Milliseconds wall_attack_cooldown{}; bool smashes_walls{}; double inferno_initial_damage{}; double inferno_second_damage{}; double inferno_max_damage{}; Milliseconds inferno_second_stage_at{}; Milliseconds inferno_max_stage_at{}; double projectile_speed_tiles_per_second{}; bool projectile_speed_sourced{}; double scattershot_direct_min_damage{}; double scattershot_splash_max_damage{}; double scattershot_splash_min_damage{}; double scattershot_cone_angle_degrees{}; double scattershot_cone_range{}; double scattershot_inner_range{}; bool scattershot_same_altitude_only{}; double aura_damage{}; double aura_range{}; Milliseconds aura_cooldown{}; bool aura_excludes_walls{}; bool aura_ignores_freeze{}; bool aura_ignores_rage{}; Milliseconds deployment_rage_duration{}; double deployment_rage_damage_multiplier{1.0}; double deployment_rage_movement_speed_multiplier{1.0}; double piercing_projectile_range{}; double chain_damage_multiplier{}; int chain_target_count{}; double chain_radius{}; int opening_long_shot_count{}; double opening_long_shot_range{}; double opening_long_shot_damage_multiplier{1.0}; int bounce_impact_count{}; double bounce_step{}; double bounce_splash_radius{}; };
// `mode` controls an alternate weapon mode (for example X-Bow air+ground),
// while `variant` selects a separately sourced catalogue record such as a
// supercharged building. They deliberately remain distinct in replays.
struct Placement { Kind kind{}; int level{1}; Vec2 position{}; std::string mode{"normal"}; std::string variant{"normal"}; };
// Non-combat entries are deliberately not Entity instances: they cannot be
// targeted, damaged, counted for destruction, or given invented hit points.
struct NonCombatPlacement { Kind kind{}; Vec2 position{}; };
struct NonCombatStats { Kind kind{}; int footprint_width{}; int footprint_height{}; int deployment_margin{}; std::string image; };
struct ArmySlot { Kind kind{}; int level{1}; int count{}; };
struct SpellStats { SpellKind kind{}; int level{}; double radius{}; double damage{}; double total_healing{}; double damage_multiplier{1.0}; double speed_multiplier{1.0}; Milliseconds duration{}; int pulses{1}; Milliseconds pulse_interval{}; double building_damage_percent{}; double troop_damage_percent{}; };
struct SpellSlot { SpellKind kind{}; int level{1}; int count{}; };
struct SpellEffectView { SpellKind kind{}; Vec2 position{}; double radius{}; Milliseconds expires_ms{}; };
// Read-only presentation/RL projection of a sourced delayed destruction
// effect.  It intentionally carries no damage application method: combat is
// resolved only by BattleState::update_death_explosions on its impact tick.
struct DeathExplosionView { Vec2 position{}; double radius{}; Milliseconds impact_ms{}; bool ground_only{}; };
// A read-only projection of an in-flight Core projectile. `position` and the
// derived current `target_position` change only on fixed combat ticks; the GUI
// may interpolate them for rendering but must never feed that interpolation
// back to BattleState. The scheduled impact is zero when a moving trajectory
// has no precomputable impact tick (for example, a homing projectile).
struct ProjectileView { ProjectileId id{}; EntityId owner{}; EntityId target{}; Vec2 origin{}; Vec2 position{}; Vec2 target_position{}; double speed_tiles_per_second{}; Milliseconds launched_ms{}; Milliseconds scheduled_impact_ms{}; bool speed_sourced{}; bool homing{}; };
struct Scenario { int format_version{1}; int width{40}; int height{40}; std::uint64_t seed{1}; Milliseconds duration_ms{180000}; std::vector<Placement> defenders; std::vector<NonCombatPlacement> non_combat_obstacles; std::vector<ArmySlot> army; std::vector<SpellSlot> spells; };
struct Command { CommandType type{CommandType::Wait}; Milliseconds requested_ms{}; Milliseconds effective_ms{}; std::uint64_t sequence{}; Kind kind{Kind::Barbarian}; int level{1}; Vec2 position{}; SpellKind spell{SpellKind::Rage}; };
// Event geometry is a read-only record of where the Core resolved the event.
// It is presentation/telemetry only: it is neither hashed nor snapshot state,
// and cannot feed a future combat decision.
struct Event { EventType type{}; Milliseconds time_ms{}; EntityId actor{}; EntityId target{}; double value{}; std::string detail; Vec2 origin{}; Vec2 target_position{}; bool has_origin{}; bool has_target_position{}; ProjectileId projectile_id{}; };
struct EntityView { EntityId id{}; Kind kind{}; Side side{}; int level{}; Vec2 position{}; double hp{}; double max_hp{}; double attack_range{}; std::optional<EntityId> target; Milliseconds next_action_ms{}; EntityCategory category{EntityCategory::Other}; TargetFocus target_focus{TargetFocus::Any}; bool flying{}; bool heals{}; int footprint_width{1}; int footprint_height{1}; bool supercharged{}; bool underground{}; bool invisible_to_defenses{}; bool deployment_rage_active{}; };
struct Snapshot { std::string canonical; }; // versioned portable canonical representation; no pointers/STL dumps.
struct BattleResult { bool finished{}; bool attackers_win{}; bool timed_out{}; Milliseconds time_ms{}; double destruction{}; int stars{}; int troops_remaining{}; Milliseconds remaining_time_ms{}; std::string reason; };

class GameData {
public:
  static GameData v0();
  const Stats* find(Kind kind, int level, const std::string& variant = "normal") const;
  const NonCombatStats* find_non_combat(Kind kind) const;
  const SpellStats* find_spell(SpellKind kind, int level) const;
  int max_level(Kind kind) const;
  const std::vector<Stats>& supported() const { return stats_; }
  const std::vector<SpellStats>& supported_spells() const { return spells_; }
private: std::vector<Stats> stats_; std::vector<NonCombatStats> non_combat_; std::vector<SpellStats> spells_; friend class BattleState;
};

class BattleState {
public:
  BattleState(GameData data, Scenario scenario);
  ~BattleState();
  BattleState(const BattleState&) = delete;
  BattleState& operator=(const BattleState&) = delete;
  BattleState(BattleState&&) noexcept;
  BattleState& operator=(BattleState&&) noexcept;
  bool submit(Command command, std::string* error = nullptr);
  Milliseconds advance_ticks(std::uint32_t count);
  Milliseconds advance_to(Milliseconds time_ms);
  Snapshot snapshot() const;
  bool restore(const Snapshot& snapshot, std::string* error = nullptr);
  std::vector<EntityView> observe() const;
  std::vector<SpellEffectView> observe_spell_effects() const;
  std::vector<DeathExplosionView> observe_death_explosions() const;
  std::vector<ProjectileView> observe_projectiles() const;
  const std::vector<Event>& events() const { return events_; }
  // Accepted player input is a value-only, time-aligned command log. Adapters
  // may persist it with `scenario()` as a replay without accessing entities.
  const std::vector<Command>& commands() const { return commands_; }
  BattleResult result() const;
  std::uint64_t state_hash() const;
  Milliseconds now() const { return now_ms_; }
  int deployed_housing() const { return deployed_housing_; }
  const Scenario& scenario() const { return scenario_; }
private:
  struct Entity;
  struct Projectile; struct SpellEffect; struct DeathExplosion;
  struct PendingSpawn { Kind kind{}; EntityId parent{}; int level{}; Vec2 position{}; Side side{}; };
  static bool alive(const Entity& entity);
  void tick(); void apply_due_commands(); void deploy(const Command&); void cast_spell(const Command&); void update_spells(); void update_traps(); void update_tornado_traps(); void update_entities(); void update_projectiles(); void update_death_explosions();
  Entity* entity(EntityId); const Entity* entity(EntityId) const; EntityId choose_target(const Entity&) const;
  std::optional<Vec2> next_path_waypoint(const Entity& attacker, const Entity& target, EntityId ignored_obstacle = 0) const; EntityId blocking_wall(const Entity& attacker, const Entity& target) const;
  double scaled_damage(EntityId actor, EntityId target, double amount) const; void damage(EntityId actor, EntityId target, double amount); void queue_spawn(Kind kind, EntityId parent, int level, Vec2 position, Side side); void queue_golemites(EntityId parent, int level, Vec2 position, Side side); void queue_trap_skeletons(EntityId parent, const Stats&, Vec2 position, Side side, TargetType); void resolve_spawns(); void finish_if_needed(); std::uint64_t scenario_fingerprint() const; void emit(EventType, EntityId, EntityId, double, std::string, ProjectileId projectile_id = 0);
  GameData data_; Scenario scenario_; Milliseconds now_ms_{}; std::uint64_t next_id_{1}; ProjectileId next_projectile_id_{1}; int deployed_housing_{}; bool finished_{}; bool attackers_win_{}; bool timed_out_{};
  std::vector<Entity> entities_; std::vector<Projectile> projectiles_; std::vector<DeathExplosion> death_explosions_; std::vector<SpellEffect> spell_effects_; std::vector<PendingSpawn> pending_spawns_; std::vector<Command> commands_; std::size_t command_cursor_{}; bool ended_by_player_{}; std::vector<Event> events_;
};

std::string to_string(Kind); std::string to_string(SpellKind); std::string to_string(EntityCategory); std::string to_string(TargetFocus); std::string to_string(TargetType); std::optional<Kind> parse_kind(const std::string&); std::optional<SpellKind> parse_spell(const std::string&); std::string to_json(const Scenario&); std::string to_json(const BattleResult&); bool load_scenario(const std::string& path, Scenario& out, std::string& error); bool save_replay(const std::string& path, const Scenario&, const std::vector<Command>&, std::string& error); bool load_replay(const std::string& path, Scenario&, std::vector<Command>&, std::string& error);
} // namespace cocsim
