# yetimite projectile audit delta

The yetimite is a spawned-only, self-destructive ranged splash unit. Its public
table gives a two-tile range but no versioned attack-entry delay, projectile
flight, jump geometry, collision rule or impact timing. Core therefore uses the
existing value-only `BattleState::Projectile` path and schedules the single
impact at T+10 ms after acquisition. This is a deterministic scheduling boundary
and not a calibrated combat timing claim.

`cocsim_yeti_tests` verifies the sourced fourfold defense multiplier, the
spawned-unit lifecycle and snapshot/replay determinism. Viewer and RL receive
the same Core projectile read model.
