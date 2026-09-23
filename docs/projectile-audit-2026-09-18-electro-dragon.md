# electro_dragon projectile audit delta

The Electro Dragon now creates one serialized logical projectile for each selected chain victim. The source-supported chain has at most five distinct ground or air victims and decreasing damage. The Core branch order is deterministic: distance, initial maximum hitpoints, then entity ID.

The primary Supercell launch post establishes the five-victim decreasing chain. A later official fix establishes that no target may occur twice in one chain. The 20 percent decay and one-empty-tile geometry use dated secondary documentation and therefore remain partial fidelity. Projectile flight, jump timing, collision rules, target loss and death-lightning behavior remain explicit open uncertainties.

The Viewer renders the existing Core projectile observations and RL observes the same projectiles. No adapter contains chain combat logic.
