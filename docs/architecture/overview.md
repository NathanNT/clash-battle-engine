# CoCSim architecture

## Dependency direction

```text
versioned data ──> cocsim_core <── CLI
                          ^         Viewer
                          └──────── RL/Python
```

`cocsim_core` is the sole owner of combat rules. The Viewer, CLI, and RL
adapter submit value commands and read value projections; none reimplements a
combat calculation.

## Core modules

The split remains direct and does not require an extra framework:

- `content_registry`: stable names and identifier conversion. Serialized order
  and C++ names come together from `content_ids.inc` and `spell_ids.inc`;
- `game_data`: immutable catalogue materialization and provenance;
- `scenario_io`: versioned scenarios and replays;
- `core`: tick loop, commands, and public projections;
- `movement`: geometry, pathfinding, and walls;
- `targeting`: deterministic target selection;
- `combat` and `damage`: attack cadence and damage application;
- `projectiles`, `spells`, `traps`, and `spawning`: focused mechanics;
- `snapshot`: value serialization, migrations, and deterministic hashes.

A module is extracted only when it has a clear responsibility. The project
does not require a generic ECS, plugin system, or scripting language.

## Deterministic battle contract

- Combat decisions use only integer `kTickMs` simulation time (currently 16 ms).
- Every command has a future, tick-aligned effective time and a stable sequence.
- Equal choices resolve through stable entity identifiers.
- `GameData` is immutable for the lifetime of a battle.
- Snapshots and replays serialize values, never pointers or raw STL memory.
- The logical hash contains only state that can influence the future.
- A playback multiplier processes more fixed ticks per rendered second; it
  never changes the tick duration.

The Home Village board is a 44×44 construction grid inside a three-tile outer
deployment border (50×50 tiles total). Defender footprints belong entirely to
the construction grid. Buildings and walls reserve their footprint plus one
deployment tile; concealed traps do not create an invisible deployment margin.
Pathfinding uses the same Core geometry as collision and deployment.

## Data, scenarios, and replays

`tools/importer/reference.py` carries the frozen source inventory and its
supplements. `tools/import_data.py` normalizes those inputs without containing
combat rules. The normalized catalogue preserves the available progression
fields—hitpoints, damage, cadence, range, footprint, requirements, limits, and
source provenance—per `(kind, level)`.

The CLI validates and simulates JSON scenarios. Replays preserve the initial
scenario and the exact Core command sequence, including each effective tick and
sequence number. They can therefore reproduce a battle through the same engine
used by the Viewer and RL adapter.

## Adapters

- `src/viewer/main.cpp` contains only the SDL/ImGui loop and command submission;
- `src/viewer/render.hpp`, `assets.hpp`, and `demo_scenario.cpp` isolate
  rendering, graphical resources, and the practice scenario;
- `simulation_bridge.hpp` is the sole `BattleState` owner on the Viewer side;
- `bindings/module.cpp` exposes the Core to Python without duplicating rules.

The Viewer records terminal replays under `replays/` and can load a replay
from its JSON path or `--replay` at startup. It receives result values such as
destruction, stars, troops remaining, and time remaining from `BattleResult`;
it does not calculate them itself.

Focused domain tests are indexed in [`tests/README.md`](../../tests/README.md).

## Splitting rule

A file represents a domain, not a catalogue item. Troop or defence specifics
build on shared systems and receive a focused behavior test when necessary.
