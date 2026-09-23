# Projectile audit — Super Bowler (`super_bowler`)

Supercell establishes three damaging impacts per attack. The shared Core path
therefore emits exactly three serialized, ground-only splash impacts on the
initial target ray at logical T+10 ms. Viewer and RL observe those Core
projectiles without owning a combat clock.

The 3-tile forward step and 0.6-tile splash are secondary evidence; fixed
centres, simultaneous timing, no retargeting, travel and collision are explicit
replaceable approximations. This is partial functionality, not projectile
fidelity verification.
