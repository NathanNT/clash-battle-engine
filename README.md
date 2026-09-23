# CoCSim empty engine

CoCSim is a deterministic C++20 Home Village simulation foundation. The active `main` branch contains an intentionally empty ruleset: no buildings, units, spells, traps, equipment, or combat outcomes. The populated pre-rebuild project is preserved at branch `archive/pre-empty-engine-2026-09-23`, commit `db13f52f2c2c59867cd1685770db03d57d101266`.

The logical clock advances only in fixed 16 ms ticks. The 50×50 board retains a 44×44 build area and a 3-tile border. Commands are future tick aligned and ordered by sequence. Core owns the board, time, events, snapshots, replays, and state hashes. The CLI, SDL3/ImGui Viewer, and optional Python adapter call the same Core.

On Windows with Visual Studio 2022:

```powershell
.\tools\dev.ps1 build
.\tools\dev.ps1 test
.\tools\dev.ps1 viewer
.\tools\dev.ps1 headless
```

Create and run an empty scenario:

```powershell
.\build\empty\Debug\cocsim.exe init-scenario empty.json
.\build\empty\Debug\cocsim.exe simulate empty.json --until 1600 --save-replay empty-replay.json --save-snapshot empty-snapshot.json
.\build\empty\Debug\cocsim.exe replay empty-replay.json
.\build\empty\Debug\cocsim.exe resume empty.json empty-snapshot.json
```

The empty format uses `ruleset: empty-16ms-v1`. Old populated scenarios, replays, snapshots, and content commands are rejected. See [rebuild notes](docs/empty-engine-rebuild.md).
