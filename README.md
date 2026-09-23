# Clash Battle Engine

A deterministic C++20 Home Village simulation foundation. The active `main` branch currently contains no combat entities or populated catalogue. The pre-rebuild project is preserved at `archive/pre-empty-engine-2026-09-23` (`db13f52f2c2c59867cd1685770db03d57d101266`).

Core advances in fixed 16 ms ticks. Its 50×50 board has a 44×44 build area and a 3-tile border. Commands, observations, events, snapshots, replays, and state hashes are owned by Core. The Headless CLI, Viewer, and optional Python adapter all call that same implementation.

On Windows with Visual Studio 2022:

```powershell
.\tools\dev.ps1 build
.\tools\dev.ps1 test
.\tools\dev.ps1 viewer
.\tools\dev.ps1 headless
```

Create and run a scenario:

```powershell
.\build\engine\Debug\cocsim.exe init-scenario scenario.json
.\build\engine\Debug\cocsim.exe simulate scenario.json --until 1600 --save-replay replay.json --save-snapshot snapshot.json
.\build\engine\Debug\cocsim.exe replay replay.json
.\build\engine\Debug\cocsim.exe resume scenario.json snapshot.json
```

The saved ruleset identifier remains `empty-16ms-v1` for format compatibility. Populated legacy scenarios and commands are rejected. See [rebuild notes](docs/rebuild-baseline.md).
