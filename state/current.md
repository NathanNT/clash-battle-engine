# Current state — 2026-09-23

Active ruleset: `empty-16ms-v1`. No populated combat content is loaded. The archive baseline is `archive/pre-empty-engine-2026-09-23` at `db13f52f2c2c59867cd1685770db03d57d101266`.

Core clock, board, commands, observation, events, state hash, snapshot, replay, Headless, Viewer, and optional RL adapter are present. Frozen TH18 reference and old coverage matrix live only on the archive branch.

Verification: full Visual Studio Debug build passed; CTest 6/6 passed; CLI empty replay and snapshot restoration yielded the same terminal hash; optional Python extension built and round-tripped a snapshot; Viewer starts under SDL dummy video. No combat fidelity claim applies to the empty ruleset.
