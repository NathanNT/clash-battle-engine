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
- `super_minion_tests.cpp`: eight 10.25-tile Long Shots, explicit fallback to
  ordinary damage, return to normal range, snapshots, replays, and hashes.
- `super_bowler_tests.cpp`: three splash impacts at the documented fixed range,
  deterministic ordering, snapshots, replays, and hashes.
- `super_dragon_tests.cpp`: dated aggregate cycle, primary target, altitude,
  snapshots, replays, and hashes. Roast splash remains explicitly open.
- `performance_smoke_tests.cpp`: a performance and determinism guard under load.
- `viewer_thread_tests.cpp`: SDL-free asynchronous GUI/Core boundary.
- `viewer_selection_tests.cpp`: GUI selection on the Core footprint, with stable identifier tie-breaking.
- `viewer_projectile_visual_tests.cpp`: cosmetic phase for shots with unknown
  speed, without Viewer state or clocks in Core.
- `viewer_demo_scenario_tests.cpp`: default TH18 demo, materializable levels,
  bounds, non-overlapping footprints, and Core deployment commands.

`core_tests.cpp` is the historical regression suite. It remains executed, but
it is frozen: new behavior must go in a domain-specific file using the shared
`test_support.hpp` utilities. Fully splitting it is not required to add content
and would be a risky refactor with no immediate functional benefit.
