# Headhunter and Druid projectile audit delta

Headhunter's sourced parent range and cadence enter the shared serializable
logical projectile route. Its current secondary table has no version-pinned
launch, travel, collision or impact timing, so the Core's T+10 ms impact is a
documented lower-bound contract, not a calibrated projectile claim. Hero-only
damage and poison remain inactive because heroes and poison timing are out of
scope.

Druid's sourced human healing range and cadence enter the same serializable
healing-projectile route as Healer. The current secondary table does not
provide healing launch, travel, chain geometry, selection or impact timing.
Core therefore applies one direct target on its normal T+10 ms boundary; it
does not infer the unmeasured healing chain.

`cocsim_headhunter_tests` and `cocsim_druid_tests` cover the respective Core
paths plus snapshot/replay future-hash preservation.
