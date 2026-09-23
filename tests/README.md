# Engine tests

`engine_tests.cpp` checks board geometry and routing, 16 ms ticks, future-aligned commands, stable sequence, event restoration, snapshot/replay determinism, timeout, and strict scenario validation. `viewer_bridge_tests.cpp` checks presentation pacing and board hit testing. CTest also checks CLI round trips, unsupported schema rejection, and SDL dummy-video Viewer startup.
