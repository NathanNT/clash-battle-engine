# Empty-engine tests

`empty_engine_tests.cpp` checks board geometry and routing, 16 ms ticks, future-aligned commands, stable sequence, event restoration, snapshot/replay determinism, timeout, and strict scenario validation. `viewer_bridge_tests.cpp` checks presentation pacing without changing the rule tick. CTest also checks CLI round trips, rejection of old populated content, and SDL dummy-video Viewer startup. Entity-specific regressions remain on the archive branch.
