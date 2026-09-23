# Projectile audit — Super Minion (`super_minion`)

The active TH18 delta gives the first eight Super Minion Long Shots a
primary-sourced 10.25-tile range. Core captures each launch in the shared
logical T+10 ms projectile path used by Viewer and RL. The official source says
those shots deal extra damage but does not quantify it: immutable GameData uses
ordinary damage as a documented lower-bound fallback, not as a fidelity claim.

The counter is consumed on Core launch and is serialized in Snapshot V17.
Target loss, cancellation, flight, collision and the actual damage multiplier
remain explicit uncertainties.
