# CoCSim invariants

- Rules use only `kTickMs` integer simulation time. Never use SDL, Python, wall
  clock or a floating `dt` for a combat decision.
- Commands must have an explicit, future tick-aligned effective time and stable
  sequence. Keep deterministic tie-breaks by entity id.
- `GameData` is immutable during a battle. Do not add combat statistics in entity
  classes; catalogue provenance is mandatory.
- A graphical speed multiplier may run more ticks per rendered second, but must
  never change the fixed rule tick.
- Snapshots/replays serialize values, never pointers or raw STL memory. Hash only
  future-influencing logical state.
- Keep GUI/Python adapters as callers of `cocsim_core`, never duplicate rules.

## TH18 integration procedure

- Treat `state/current.md` as the single active increment and
  `state/backlog.md` as the execution order. Do not begin a later integration
  before closing or explicitly marking the active item partial.
- Preserve the frozen 2026-09-17 Home Village reference. New evidence belongs
  in a dated, hash-pinned additive delta with complete provenance.
- Implement only source-supported Core behaviour, then exercise that exact path
  through Headless, Viewer, and RL. Core owns geometry and combat rules.
- Add focused behaviour, snapshot, replay, determinism, and regression tests.
  Run the complete suite before updating state.
- When research leaves a combat-relevant gap, record the sources checked and
  the missing value in the delta and coverage matrix. Do not guess the rule.
  Mark the item partial and continue with the next independent item.
