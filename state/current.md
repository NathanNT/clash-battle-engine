# Current state — 2026-09-23

Active ruleset: `clash-battle-engine-16ms-v1`. Core clock, board, geometry, commands, observations, events, state hashes, snapshots, replay, Headless CLI, Viewer, and optional RL adapter are present. No combat entities or statistics are loaded.

The Viewer renders the board on the main canvas with a separate controls panel. The build and CTest suite pass 6/6 tests; CLI replay and snapshot restoration yield the same terminal hash; the optional Python adapter passes an observation and snapshot round trip. No combat fidelity claim applies to the current ruleset.
