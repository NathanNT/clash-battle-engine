# Clash Battle Engine invariants

- Rules use only fixed `kTickMs = 16` integer logical time. SDL, Python, wall time, and floating delta time may never make simulation decisions.
- Commands require future tick-aligned effective times and stable sequence ordering. Entity ID must resolve ties when entities are introduced.
- `GameData` remains immutable during simulation. Combat statistics require catalogue provenance.
- Viewer speed may execute more ticks per rendered second but may not change the rule tick.
- Snapshots and replays serialize values, never pointers or raw STL memory. State hashes cover future-influencing logical state.
- Viewer and Python adapters call `clash_battle_engine_core`; they contain no combat rules.
- Reject unsupported scenario and content schemas. Never fall back to unspecified data.
- Add combat content only after the version-pinned behavior/conversion catalogue is complete and the user explicitly starts integration.
