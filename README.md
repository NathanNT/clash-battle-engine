# Clash Battle Engine

Clash Battle Engine is a deterministic C++20 combat-simulator foundation
inspired by the Home Village combat model of Clash of Clans. It is built for
repeatable experiments, replayable battles, and headless or interactive use. It
is not an exact reproduction of the original game.

## Highlights

- A fixed 16 ms logical tick and deterministic command ordering.
- One shared `cocsim_core` for the command-line simulator and SDL3/ImGui Viewer.
- Versioned scenarios, snapshots, state hashes, and battle replays.
- A playable TH18 practice scene with buildings, troops, walls, traps, and
  documented source provenance.

## Fixed-step timing

The simulator uses a fixed 16 ms step. Existing catalogue durations and combat
formulas are retained; durations are rounded to the nearest representable tick
(for example, 1000 ms becomes 1008 ms). Commands require future aligned times,
and replay V5 records the tick duration so older 10 ms schedules cannot be
silently reinterpreted. This does not assert the original game's internal tick
duration or change the logical projectile algorithm.

## Quick start

On Windows, the development helper configures a consistent MSVC build:

```powershell
.\tools\dev.ps1 test
.\tools\dev.ps1 viewer
.\tools\dev.ps1 headless
```

The Viewer starts with the bundled TH18 practice scene. Choose a troop in the
side panel and click a valid map cell to deploy it. `Space` pauses, `N` advances
one tick, and `R` resets the battle. At the end of a battle, the Viewer saves a
replay under `replays/`.

For a headless CMake build, configure with `COCSIM_BUILD_VIEWER=OFF`. The CLI
can validate, simulate, and replay the JSON scenarios in `scenarios/`.

## Project layout

| Path | Purpose |
| --- | --- |
| `src/core/` and `include/cocsim/` | Deterministic simulation engine and public API. |
| `src/viewer/` | SDL3/ImGui presentation adapter. |
| `scenarios/` | Versioned, runnable battle scenarios. |
| `data/` | Normalized catalogue and its public manifest. |
| `tests/` | Focused behavior, serialization, determinism, and adapter tests. |
| `tools/` | Development, import, and validation utilities. |

## Fidelity and assets

Combat behavior is an explicitly versioned approximation. Missing public game
data is not silently invented, so some mechanics remain intentionally open.
The catalogue is normalized from pinned public input. Review the original
rights holder’s policy before redistributing downloaded Viewer assets.

## Status

The project is under active TH18-conformity development. It is suitable for
deterministic simulation and experimentation, but its results should not be
treated as authoritative results from the original game.

## License

Clash Battle Engine is released under the [MIT License](LICENSE).

## Unofficial fan content notice

This material is unofficial and is not endorsed by Supercell. For more
information, see [Supercell's Fan Content Policy](https://supercell.com/en/fan-content-policy/).
