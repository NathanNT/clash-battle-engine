# GUI / simulation contract

Date: 2026-09-17. The Viewer has two isolated responsibilities: the simulation
thread owns the sole `BattleState`; the GUI thread owns SDL, ImGui, textures,
and rendering caches. No `BattleState` pointer or reference crosses that
boundary.

```text
SDL / ImGui (GUI thread) -- value QueuedCommand --> SimulationBridge
SimulationBridge (SIM thread) -- immutable PresentationSnapshot --> GUI
                           --> cocsim_core / BattleState
```

`SimulationBridge` assigns every received command to `battle.now() + kTickMs`.
Wall time only accumulates presentation debt and chooses how many fixed ticks
the simulation processes. Rules, commands, snapshots, replays, and hashes are
therefore defined solely by integer `kTickMs` milliseconds.

Each publication copies the Core public projections—entities, spells,
explosions, projectiles, reserves, result, command log, and a bounded event
queue—into one of three `shared_ptr<const PresentationSnapshot>` instances. The
GUI atomically loads the latest complete buffer. It may drop intermediate frames
or cosmetic traces, but can never modify, slow, or observe a half-written
simulation state. During continuous playback the copy is capped at 30 Hz; a
reset, single step, or battle end is published immediately.

Reset, pause, single-step, and playback multiplier are atomic messages to the
SIM owner. The presentation snapshot also carries the serializable Core
`state_hash` at the published tick. It is a verification probe, never a rules
input.

## Viewer interaction

The Viewer loads the bundled TH18 practice scene unless `--scenario` provides a
Core-validated JSON scenario. Troop and spell panels come from that scenario.
A map click submits the same Core deployment command used by replay and RL; the
GUI does not decide whether it is valid.

Buildings and walls display a red one-tile exclusion frame. Any cell outside
those Core-owned rectangles is deployable, including a genuinely free cell
inside a wall enclosure. Concealed traps never create an invisible deployment
margin. Shift-click is a read-only footprint inspector.

`Space` pauses, `N` advances one tick, `R` resets, `V` toggles defensive
ranges, and the **Finish battle** control enqueues `EndBattle` for the next
fixed tick. Destruction, stars, troops remaining, and time remaining are read
from `BattleResult`.

On its first terminal publication, the Viewer saves the initial scenario and
the Core command log by value under `replays/viewer-replay-*.json`. The system
clock serves only to create a unique filename: the stored commands retain their
Core `effective_ms` and `sequence` values and can be loaded by `cocsim replay`.

## Verification

`cocsim_viewer_thread_tests` runs without SDL. It verifies a paused reset at
T+0, FIFO delivery of two value deployment commands at one tick, their fixed
tick application, and reserve consistency in the same snapshot. It also
compares the resulting hash with a direct `BattleState`, verifies that a GUI-held
frame remains unchanged after buffer rotation, and runs a concurrent GUI reader
through twenty publications before comparing the final hash with a direct Core
run. `headless_gui_isolation_validation` additionally verifies that Core/RL do
not link SDL/ImGui and that the Viewer contains no `BattleState`.

A reset places a marker in the queue. Commands serialized before that marker
are discarded when the new `BattleState` is created; commands arriving after it
belong to the new battle. The same concurrent test covers this case.

Headless mode and the RL interface never create `SimulationBridge`: they remain
synchronous callers of `cocsim_core`, without a GUI thread, assets, or a
presentation clock.
