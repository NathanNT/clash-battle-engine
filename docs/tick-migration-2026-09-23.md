# Fixed step migration: 10 ms to 16 ms

The Core now uses a fixed 16 ms logical step. This is an engine scheduling
change for later data and behavior work. The frozen 2026-09-17 reference,
additive source deltas, gameplay formulas, and projectile algorithms are
unchanged. No claim is made about the native game's internal tick duration.

## Dependencies audited and handling

| Dependency | 16 ms handling |
| --- | --- |
| Core clock, command effective times, `advance_to`, and stable sequence | `kTickMs` is 16; commands still require a future aligned time. Core rejects unaligned advances and commands. Entity ID remains the tie break. |
| Catalogue seconds to cooldowns, burst gaps, death delays, rage windows, aura periods, invisibility and other timed stats | Existing nearest-tick `llround` conversion now uses 16. Source seconds and formulas stay intact. |
| Action cooldowns under speed modifiers and Inferno lock | The existing cooldown ceiling uses 16; lock elapsed accumulates 16 per tick. No combat formula changed. |
| Movement and projectile travel | The existing speed-in-tiles-per-second calculation uses `kTickMs / 1000`; each step now covers 16 ms. Projectile launch, travel, collision and impact logic are unchanged. An unsourced logical projectile still impacts on the next tick, now 16 ms later. |
| Path refresh | The old 100 ms internal refresh becomes six ticks, 96 ms, the closest representable interval. |
| Spells, traps, and expiry | Raw sourced spell milliseconds remain raw; tick checks resolve the first boundary at or after the deadline. Tornado damage is distributed over the new number of ticks while retaining its existing total. |
| Scenario timeout and Headless `--until` | Scenario duration remains its supplied millisecond value. The first boundary at or after it terminates the battle. The CLI rounds a requested inspection endpoint up to the next boundary; Core retains strict alignment. The demo's 1000 ms deployment becomes 1008 ms. |
| Viewer pacing and queued commands | The bridge divides presentation debt by 16 and schedules on the next Core tick. Playback multipliers still vary the number of executed ticks, never the duration of one tick. |
| RL decisions | `tick_ms` comes from the native module. Decision intervals must be multiples of it; the default is 96 ms, nearest to the old 100 ms default. |
| Snapshot and replay | New snapshots use V22 and bind their fingerprint to the 16 ms grid. Replay V5 records `tick_ms:16`; older formats or a mismatched grid are rejected. This prevents a 10 ms schedule being silently reinterpreted. |

## Rounding and observed differences

Nearest-tick catalogue conversion may now differ from its source duration by
up to 8 ms (the former 10 ms grid allowed up to 5 ms). Examples in milliseconds:

| Source duration | Previous grid | New grid |
| ---: | ---: | ---: |
| 100 | 100 | 96 |
| 130 | 130 | 128 |
| 190 | 190 | 192 |
| 250 | 250 | 256 |
| 500 | 500 | 496 |
| 700 | 700 | 704 |
| 750 | 750 | 752 |
| 1000 | 1000 | 1008 |
| 1500 | 1500 | 1504 |
| 2500 | 2500 | 2496 |
| 5250 | 5250 | 5248 |
| 7000 | 7000 | 7008 |

Raw deadlines, such as scenario timeouts and spell expiry, are observed on the
first tick at or after the deadline, less than 16 ms later. Repeated nearest
tick cooldowns may accumulate drift over multiple actions. A one-tick logical
projectile's interval changes from 10 to 16 ms solely because the fixed step
changed; its algorithm has not been revised.

The recorded 10 ms Barbarian baseline (`scenarios/barbarian-baseline.json`,
deployment at 10 ms, requested checkpoint 1000 ms) first attacked at 480 ms
and impacted at 490 ms. The corresponding 16 ms run (deployment at 16 ms,
checkpoint 1008 ms) first attacks at 496 ms and impacts at 512 ms. Both are
active at the checkpoint with zero destruction, zero stars, and one deployed
troop. `cocsim_tick_migration_tests` freezes this comparison and checks command
alignment, catalogue rounding, snapshot restoration, replay hashes, and outcome.

These measured outcomes are regression examples, not a guarantee that every
battle ends identically across grids: rounding can change event ordering near
a boundary. The new game database, native projectile behavior, and native game
tick model require separate evidence and work.
