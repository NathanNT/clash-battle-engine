# Witch projectile audit delta

The regular Witch has a published four-tile range and 0.7-second parent cadence
in the pinned public table. No consulted source provides a current projectile
speed, launch delay, travel model, splash radius, collision contract or impact
timing. Core therefore uses the shared value-only logical projectile route and
resolves a direct primary-target impact on its normal T+10 ms scheduling
boundary. This is not a calibrated projectile claim.

`cocsim_witch_tests` verifies the L8 parent damage and snapshot/replay future
hash. Witch Skeleton summoning is outside this projectile contract because its
child table and lifecycle remain unmaterialized and explicitly documented.
