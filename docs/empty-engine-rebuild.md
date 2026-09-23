# Empty engine rebuild (2026-09-23)

Archive branch: `archive/pre-empty-engine-2026-09-23`
Archive commit: `db13f52f2c2c59867cd1685770db03d57d101266`

The archive preserves the relevant pre-removal source, tests, reference deltas, catalogue, documentation, and Python adapter. Generated build output, caches, replays, downloaded assets, raw external data, and audit checkout were excluded. The archive commit was verified before `main` was fast-forwarded to it; no history was rewritten.

## Retained engine

Fixed 16 ms clock; deterministic tick and command progression; 50×50 board, build border, rectangles, and deterministic path search; strict empty scenario/replay codecs; value-only snapshots; event log; state hashes; CLI; SDL3/ImGui empty board and controls; optional Python/Gymnasium adapters; infrastructure tests and build tooling.

## Removed content

All predefined buildings, troops, siege machines, heroes, pets, spells, traps, equipment and gadgets; their hard-coded behavior and fixtures; normalized catalogue, importer, manifest, reference deltas, audit scripts, and entity-specific tests. The archive retains them. Main has no fallback to old catalogue data.

## Compatibility

The active scenario schema has `format_version: 1`, `ruleset: empty-16ms-v1`, and exactly `width`, `height`, `seed`, and `duration_ms`. Board size is 50×50; duration is positive and a multiple of 16 ms. Unknown fields and other rulesets fail. Replay JSON includes `tick_ms: 16`, scenario values, and accepted `wait`/`end_battle` commands with request time, future effective time, and sequence. Snapshots contain values and reconstruct Core state. Old populated scenarios, replays, snapshots, CLI content commands, and Python entity actions are intentionally incompatible.

## Verification

The Visual Studio Debug build and CTest suite cover board geometry, clock, command order, event restoration, hashes, strict codecs, replay/snapshot round trips, CLI, Viewer pacing, and dummy-video Viewer startup. The optional Python binding builds and its snapshot/observation path runs. See [current state](../state/current.md).

Do not repopulate the engine until the version-pinned combat behavior and conversion catalogue is complete and an integration pass is explicitly authorized.
