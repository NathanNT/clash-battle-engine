# Hero projectile audit

Archer Queen (`archer_queen`) and Grand Warden (`grand_warden`) use the shared serialized Core projectile path.
Their current public references establish range and cadence, but not launch,
travel speed, collision or impact timing. Their logical next-tick path remains
explicitly unmeasured.

Royal Champion (`royal_champion`) uses the same shared serialized Core path.
Her sourced range and cadence do not establish a reproducible projectile speed,
launch, collision or impact contract. The logical next-tick path remains open.

Minion Prince (`minion_prince`) uses the same shared serialized Core path.
His sourced range and cadence do not establish projectile speed, launch,
collision or impact timing. The logical next-tick path remains open.

Dragon Duke (`dragon_duke`) uses the same shared serialized Core path. His
base range and cadence do not establish projectile or contact launch, collision
or impact timing. The logical next-tick path remains open.

Lava Pup (`lava_pup`) is also an executable ranged child. Its split is deterministic, but
its projectile travel contract is not published. No visual or wall-clock rule
is used to fill this gap.
