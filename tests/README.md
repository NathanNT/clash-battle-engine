# Tests

New tests are organized by responsibility:

- `content_registry_tests.cpp`: serialized identifiers and names.
- `scenario_io_tests.cpp`: scenarios and replays.
- `determinism_tests.cpp`: command ordering, snapshots, and hashes.
- `deployment_replay_tests.cpp`: the one-tile global margin around every
  defender, including walls. It covers a free central cell, the Core command
  log, battle completion, metrics, and replay with an identical hash.
- `targeting_tests.cpp`: acquisition of targets reachable by stationary
  defences, including when another area of the village is already engaged.
- `permanent_building_tests.cpp`: passive permanent buildings, geometry,
  snapshots, replays, and deterministic hashes.
- `permanent_variant_tests.cpp`: all current Supercharge catalogue families,
  the explicit scenario selector, observations, snapshots, and replays. It
  verifies state transport only, not undocumented special combat behaviour.
- `air_defense_tests.cpp`: air defence, altitude filtering, identifier
  tie-breaking, logical projectiles, snapshots, and replays.
- `monolith_tests.cpp`: base damage plus a percentage of maximum HP, logical
  projectiles, snapshots, replays, and deterministic hashes.
- `super_wall_breaker_tests.cpp`: guaranteed explosion on reaching the target
  or on destruction, sourced inherited wall damage, snapshots, replays, and hashes.
- `super_barbarian_tests.cpp`: T+ms deployment Rage duration, speed, and
  damage, followed by snapshot/replay across the expiry boundary.
- `super_giant_tests.cpp`: defence priority, ×5 Wall impact, shared path,
  snapshot/replay, and deterministic hashes.
- `super_archer_tests.cpp`: six-tile acquisition, the fixed 12-tile Sharp Shot
  ray, ordered piercing, and deterministic snapshots/replays.
- `rocket_balloon_tests.cpp`: T+ms deployment boosters, secondarily sourced
  speed, deterministic snapshots, replays, and hashes.
- `inferno_dragon_tests.cpp`: independent three-stage beam, sourced cadence,
  explicitly documented lock approximation, deterministic snapshots, replays,
  and hashes.
- `super_wizard_tests.cpp`: Chain Magic branches from the primary target, 40%
  secondary damage, stable ordering, snapshots, replays, and hashes.
- `super_wizard_tower_tests.cpp`: TH18 merged-defense Chain Magic branches
  from one primary to up to 15 nearby targets, with source-backed range,
  damage decay, snapshots, replays, and deterministic hashes. Exact client
  selection, trajectory, collision and visuals remain open.
- `multi_archer_tower_tests.cpp`: source-backed three-arrow allocation for one,
  two, or three targets, plus snapshot/replay deterministic hashes. Exact
  client two-target selection, trajectory and collision remain open.
- `multi_gear_tower_tests.cpp`: source-backed normal and Fast Attack mode
  selection, deterministic 16-ms burst quantization, air/ground targeting,
  snapshots, replays, and hashes. Client timing, trajectory and collision
  remain open.
- `scattershot_tests.cpp`: source-backed direct impact and rear-cone falloff,
  altitude filtering, snapshots, replays, and deterministic hashes. Ammunition,
  projectile flight, collision and client target selection remain open.
- `bomb_tower_tests.cpp`: source-backed ground-only direct splash and delayed
  one-second destruction explosion, including snapshot/replay/hash persistence.
  Projectile trajectory, collision, target selection and client tick order remain open.
- `x_bow_tests.cpp`: source-backed normal and air-and-ground modes, current
  TH18 Supercharges, range/altitude filtering, snapshots, replays, and hashes.
  Ammunition persistence, projectile flight, collision and client tick order remain open.
- `inferno_tower_tests.cpp`: source-backed normal/multi-target beam modes,
  current TH18 Supercharges, six-target cap, ramp stages, snapshots, replays,
  and hashes. Beam acquisition, collision, persistent ammunition and client tick
  order remain open.
- `super_minion_tests.cpp`: eight 10.25-tile Long Shots, explicit fallback to
  ordinary damage, return to normal range, snapshots, replays, and hashes.
- `super_bowler_tests.cpp`: three splash impacts at the documented fixed range,
  deterministic ordering, snapshots, replays, and hashes.
- `bowler_tests.cpp`: two fixed impact centres, absence of Super Bowler's third
  impact, snapshots, replays, and hashes. Bounce path and splash radius remain
  explicitly open.
- `ice_golem_tests.cpp`: sourced death Freeze durations, attacker-only shared
  Freeze effect, snapshots, replays, and deterministic hashes. Propagation and
  exact target eligibility remain explicitly open.
- `apprentice_warden_tests.cpp`: primary-sourced Life Aura range and level
  values, strongest-only overlap, ground-only eligibility, snapshots, replays,
  and deterministic hashes. Aura-transition timing remains explicitly open.
- `super_hog_rider_tests.cpp`: sourced parent and child levels, deterministic
  parent-death split, non-deployable child records, snapshots, replays, and
  hashes. Launch geometry and Super Hog wall-jump fidelity remain open.
- `super_miner_tests.cpp`: sourced damage stages, shared underground route and
  ground-only death blast, snapshots, replays, and deterministic hashes.
  Stage boundaries, underground timing, and bomb delay remain explicitly open.
- `miner_tests.cpp`: L12 source-bounded tunnel baseline, Wall bypass, trap
  avoidance, snapshots, replays, and deterministic hashes. Surface timing,
  range conflict and detailed underground interactions remain open.
- `super_valkyrie_tests.cpp`: sourced L12 baseline attack, snapshot and replay
  behavior. Death Rage duration, splash radius and first-hit delay remain open.
- `valkyrie_tests.cpp`: L12 source-bounded ground splash baseline, snapshot,
  replay, and deterministic hash. Client pathing, collision and animation timing
  remain explicitly open.
- `dragon_rider_tests.cpp`: May-primary L6 direct-parent and defense-priority
  baseline, snapshots, replays, and deterministic hashes. The post-July numeric
  observation is data-only; flight, projectile, death impact, range conflict
  and visual timing remain explicitly open.
- `super_yeti_tests.cpp`: dated L8 parent baseline, snapshot and replay
  behavior. Electromite spawning remains disabled because source descriptions
  conflict on the threshold and omit a deterministic lifecycle.
- `super_witch_tests.cpp`: dated L8 parent baseline, snapshot and replay
  behavior. Big Boy summoning and splash radius remain deliberately incomplete.
- `ice_hound_tests.cpp`: L5-8 parent priority and death split into sourced L1
  Ice Pups, plus snapshot and replay determinism. Slow, freeze geometry, and
  spawn placement remain deliberately incomplete.
- `yeti_tests.cpp`: Yeti L1-8, the sourced 600-damage Yetimite threshold, and
  spawned Yetimite L1-8 defense damage, wall bypass, self-destruction, snapshot
  and replay determinism. Child attack timing and spawn formation remain open.
- `witch_tests.cpp`: Witch L1-8 direct parent baseline, explicit absence of an
  unsourced Skeleton proxy, and snapshot/replay determinism. Skeleton values,
  summoning lifecycle, splash and projectile timing remain open.
- `lava_hound_tests.cpp`: Lava Hound L1-8 direct parent baseline, sourced Air
  Defense priority, L1 Lava Pup counts through L8, deterministic death queue,
  and split snapshot/replay determinism. Parent death-damage geometry, split
  geometry/timing, projectile timing, and visual fidelity remain open.
- `headhunter_tests.cpp`: Headhunter L1-4 direct parent baseline, preserved
  Hero targeting metadata, building fallback without a fake Hero, and
  snapshot/replay determinism. Hero damage, poison, wall hopping, projectile
  timing, and visual fidelity remain open.
- `druid_tests.cpp`: Druid L1-6 human healing metadata, timed and defeat
  transforms into the spawned L1-6 Bear, the Spring Trap ejection exception,
  snapshots, replays, and deterministic hashes. Healing chains, transform
  ordering and visual fidelity remain explicitly open.
- `baby_dragon_tests.cpp`: L1-12 direct parent data, the frozen 4.5-tile
  allied-air isolation predicate, x2 damage and x1.5 attack cadence adapter,
  snapshots, replays, and deterministic hashes. Transition/wind-up timing,
  projectile/splash geometry, spell interactions and visual fidelity remain
  explicitly open.
- `pekka_tests.cpp`: P.E.K.K.A L1-13 direct-parent data, no resurrection of
  the historical Hidden Tesla x2 modifier, snapshots, replays, and
  deterministic hashes. Current source version, geometry, timing, targeting,
  spell/trap interactions and visual fidelity remain explicitly open.
- `goblin_tests.cpp`: Goblin L1-10 direct-parent data, resource priority and
  x2 resource damage through the shared Core path, snapshots, replays, and
  deterministic hashes. Target ties, pathfinding, collision, interactions and
  visual fidelity remain explicitly open.
- `minion_tests.cpp`: Minion L1-14 direct-parent data and its sourced Seeking
  Air Mine immunity through the shared Core path, snapshots, replays, and
  deterministic hashes. Range conflict, projectile, flight geometry, target
  ties, remaining interactions and visual fidelity remain explicitly open.
- `hog_rider_tests.cpp`: Hog Rider L1-15 direct-parent data, defense priority
  and sourced Wall bypass through the shared Core path, snapshots, replays,
  and deterministic hashes. Exact route selection, jump timing, collision,
  target ties, interactions and visual fidelity remain explicitly open.
- `super_dragon_tests.cpp`: dated aggregate cycle, primary target, altitude,
  snapshots, replays, and hashes. Roast splash remains explicitly open.
- `electro_dragon_tests.cpp`: five distinct sequential chain victims,
  decreasing damage, altitude eligibility, snapshots, replays, and hashes.
  Jump geometry, flight timing, and death lightning remain explicitly open.
- `dragon_tests.cpp`: current L13 direct-parent values and range, shared-Core
  attack, snapshots, replays, hashes, and CLI scenario. Splash remains open.
- `balloon_tests.cpp`: L1-13 direct-parent values, 1.2-tile normal splash and
  ground-only death-bomb metadata through the shared Core path, snapshots,
  replays, hashes, and CLI scenario. Flight, impact/death timing, collision,
  target ties and visual fidelity remain explicitly open.
- `golem_tests.cpp`: Golem L1-15 and spawned-only Golemite L1-15 values, the
  sourced four-child L15 split through the shared deterministic queue,
  snapshots, replays, hashes, and CLI scenario. Split/explosion timing and
  geometry, collision, retargeting, child assets, and child-table fidelity
  remain explicitly open.
- `wizard_tests.cpp`: Wizard L1-14 direct-parent values and 0.3-tile normal
  splash through the shared Core path, snapshots, replays, hashes, and CLI
  scenario. Projectile/splash-boundary geometry, timing, collision, target
  ties and visual fidelity remain explicitly open.
- `giant_tests.cpp`: Giant L1-14 values, Defense priority and no-Defense
  fallback through the shared Core path, snapshots, replays, hashes, and CLI
  scenario. Route topology, collision, target loss/ties, timing and visual
  fidelity remain explicitly open.
- `archer_tests.cpp`: Archer L1-14 values and sourced reach beyond Walls through
  the shared Core path, snapshots, replays, hashes, and CLI scenario.
  Projectile/Wall-boundary geometry, timing, collision, target ties and visual
  fidelity remain explicitly open.
- `barbarian_tests.cpp`: Barbarian L1-13 direct-parent values through the shared
  Core path, snapshots, replays, hashes, and CLI scenario. Route topology,
  collision, target loss/ties, timing and visual fidelity remain explicitly open.
- `healer_tests.cpp`: Healer L1-11 values, current 4.5-tile range and
  ground-recipient restriction through the shared Core path, snapshots,
  replays, hashes, and CLI scenario. Target selection/lock, chain geometry,
  timing, collision and visual fidelity remain explicitly open.
- `wall_breaker_tests.cpp`: Wall Breaker L1-14 values, ×40 Wall impact,
  two-tile area and post-impact self-destruction through the shared Core path,
  snapshots, replays, hashes, and CLI scenario. Protected-Wall topology,
  timing, collision and visual fidelity remain explicitly open.
- `electro_titan_tests.cpp`: Electro Titan L1-5 values and the sourced periodic
  aura exceptions through the shared Core path, snapshots, replays, hashes and
  CLI scenario. Pulse boundaries, timing, collision and visual fidelity remain
  explicitly open.
- `thrower_tests.cpp`: Thrower L1-4 direct-parent values and shared ranged Core
  projectile path through snapshots, replays, hashes and CLI scenario.
  Projectile timing, special retargeting, geometry and visual fidelity remain
  explicitly open.
- `root_rider_tests.cpp`: Root Rider L1-4 values, Defense priority and sourced
  Wall-contact damage through the shared Core path, snapshots, replays, hashes
  and CLI scenario. Wall selection, route topology, collision and visual
  fidelity remain explicitly open.
- `sneaky_goblin_tests.cpp`: Sneaky Goblin L7-9 values, official resource ×2
  damage and five-second defense cloak through the shared Core path, snapshots,
  replays, hashes and CLI scenario. The sourced L10 stays catalogue-only under
  the frozen historical compatibility contract; detection, geometry and visual
  fidelity remain explicitly open.
- `performance_smoke_tests.cpp`: a performance and determinism guard under load.
- `viewer_thread_tests.cpp`: SDL-free asynchronous GUI/Core boundary.
- `viewer_selection_tests.cpp`: GUI selection on the Core footprint, with stable identifier tie-breaking.
- `viewer_projectile_visual_tests.cpp`: cosmetic phase for shots with unknown
  speed, without Viewer state or clocks in Core.
- `hero_defense_tests.cpp`: explicit static Core defender placements for all
  six Heroes, their shared base attacks, snapshots, and replays. It deliberately
  does not stand in for undocumented Hero Banner patrol behavior.
- `royal_champion_tests.cpp`: source-backed Wall crossing from both attack and
  explicit defender placements, plus snapshots and replays. It does not infer
  the missing Hero Banner patrol contract.
- `dragon_duke_tests.cpp`: Royal Rampage damage, cadence and current
  trap-origin 20 percent mitigation with isolated and paired flying states,
  plus defensive targetability, snapshots and replays.
- `minion_prince_tests.cpp`: ranged-or-air defensive targetability with
  explicit defender placements, snapshots and replays.
- `monolith_arrow_housing_tests.cpp`: source-backed read-only deployed housing
  tiers and 14/10/5 damage-percent projections across fixed ticks, snapshot
  restoration, replay and hashes.
- `content_registry_tests.cpp` and the Headless/RL validators verify the
  secondary-sourced Monolith Arrow Passive class and its pinned provenance.
- `hero_loadout_tests.cpp`: source-backed Pet and Equipment selection limits,
  eligible Heroes and levels, value replay, snapshot loadout binding and equal
  logical hashes for currently inert selections.
- `viewer_loadout_editor_tests.cpp`: ImGui control rendering, editable draft
  construction and Apply mouse press/release; `viewer_thread_tests.cpp` checks
  safe Core bridge replacement.
- `content_registry_tests.cpp` verifies that all 1,073 pinned Pet and Equipment
  levels have immutable provenance records in Core. The Headless and RL
  validators compare representative level records with the normalized source.
- `validate_rl_runtime.py` verifies that RL preserves the Grand Warden's
  ground/air scenario mode and all six visual Hero Banner variants, reading
  flying state and variant values back from Core. It also checks maximum-level
  records and deployed loadout observations for every Pet and Equipment item.
- `validate_hero_support_catalogue.py` checks all 54 maximum-level Headless
  records and compares 22 one-Hero selections with their saved fixed-tick
  replays, including identical Core state hashes.
- `validate_th18_active_reference.py` checks the final Royal Gem active-heal
  recheck link, source metadata and unresolved inference record in the
  additive TH18 reference chain.
- `viewer_loadout_picker_tests.cpp` drives actual ImGui mouse selection for a
  Pet, eligible Equipment choice, and both Grand Warden ground/air modes.
  `viewer_thread_tests.cpp` loads selected-loadout and both Warden-mode replays,
  then matches direct Core at fixed ticks and checks the flying flag and hashes.
- `viewer_demo_scenario_tests.cpp`: default TH18 demo, materializable levels,
  bounds, non-overlapping footprints, and Core deployment commands.

`core_tests.cpp` is the historical regression suite. It remains executed, but
it is frozen: new behavior must go in a domain-specific file using the shared
`test_support.hpp` utilities. Fully splitting it is not required to add content
and would be a risky refactor with no immediate functional benefit.
