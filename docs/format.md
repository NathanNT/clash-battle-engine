# Scenario, replay, and snapshot format

The current ruleset identifier is `clash-battle-engine-16ms-v1`. All logical times are integer milliseconds aligned to the fixed 16 ms tick.

A scenario has exactly these fields: `format_version` (1), `ruleset`, `width` (50), `height` (50), `seed`, and `duration_ms` (a positive multiple of 16). Unknown fields and unsupported identifiers are rejected.

A replay has `format_version` (1), `tick_ms` (16), the scenario values, and accepted commands. Each command has a type (`wait` or `end_battle`), a request time, a future tick-aligned effective time, and a sequence number. Commands with the same effective time execute in sequence order.

A snapshot has `format_version` (1), `kind` (`clash-battle-engine-snapshot-v1`), the logical time, replay values, and event values. Restoring it reconstructs Core state from those values. Files with other identifiers or schemas fail validation and must be regenerated; no catalogue is loaded as a fallback.
