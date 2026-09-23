# Architecture

`clash_battle_engine_core` is the only logical rules owner. It has a fixed 16 ms integer clock, a 50×50 Home Village board with deterministic path search, immutable `GameData`, a value-only scenario codec, a future tick command queue with sequence ordering, events, observations, state hashes, snapshots, and replay reconstruction.

`clash_battle_engine` is the Headless CLI. `clash_battle_engine_viewer` renders the board on the main canvas and exposes cell selection, play, pause, step, speed, reset, commands, and replay controls. The optional `_clash_battle_engine` binding and Gymnasium adapter expose Core observations and commands. Viewer wall time only determines how many 16 ms ticks Core executes before a frame; it never affects a combat decision.

There are no active entity types, entity instances, combat statistics, or content catalogue in this ruleset. Any future data and behavior integration must use a version-pinned, fully researched catalogue and a separate explicit integration pass.
