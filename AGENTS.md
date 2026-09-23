# CoCSim empty-engine invariants

- Rules use only fixed `kTickMs = 16` integer logical time. SDL, Python, wall time, and floating delta time may never make simulation decisions.
- Commands require future tick-aligned effective times and stable sequence ordering. Entity ID must resolve ties if entities are added in a later authorized integration.
- `GameData` remains immutable during simulation. The active ruleset contains no combat statistics or entities.
- Viewer speed may execute more ticks per rendered second but may not change the 16 ms rule tick.
- Snapshots and replays serialize values, never pointers or raw STL memory. State hashes cover future-influencing logical state.
- Viewer and Python adapters call `cocsim_core`; they contain no combat rules.
- Reject populated legacy scenarios and content commands. The old normalized catalogue is archived, not an active fallback.
- Do not repopulate until the version-pinned behavior/conversion catalogue is complete and the user explicitly starts integration.
