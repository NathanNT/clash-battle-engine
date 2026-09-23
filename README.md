# Clash Battle Engine

A deterministic C++20 Home Village simulation foundation. Core advances in fixed 16 ms ticks on a 50×50 board with a 44×44 build area and 3-tile border. Commands, observations, events, snapshots, replays, and state hashes all use the same Core implementation.

The current ruleset supports board geometry, scheduling, and simulation state. Combat entities and statistics will be integrated only after the version-pinned behavior and conversion catalogue is complete.

On Windows with Visual Studio 2022:

```powershell
.\tools\dev.ps1 build
.\tools\dev.ps1 test
.\tools\dev.ps1 viewer
.\tools\dev.ps1 headless
```

Create and run a scenario:

```powershell
.\build\engine\Debug\clash_battle_engine.exe init-scenario scenario.json
.\build\engine\Debug\clash_battle_engine.exe simulate scenario.json --until 1600 --save-replay replay.json --save-snapshot snapshot.json
.\build\engine\Debug\clash_battle_engine.exe replay replay.json
.\build\engine\Debug\clash_battle_engine.exe resume scenario.json snapshot.json
```

See the [format specification](docs/format.md) and [architecture](docs/architecture/overview.md).
