# Current TH18 work

## Done

- The fixed Core step is 16 ms; the [audit](../docs/tick-migration-2026-09-23.md) records rounding and old/new outcomes. Snapshot V22 and replay V5 bind the grid. The 165/165 Viewer suite and RL runtime pass; sources, formulas, projectiles and coverage statuses are unchanged.
- Six Heroes have applicable TH18 levels and sourced base attacks in shared
  Core across Headless, Viewer and RL; see the [Hero matrix](../data/reference/th18-hero-coverage-2026-09-20.json). All six Viewer Hero Banner assignments, Apply, snapshot, Viewer bridge replay playback and six distinct Banner icon renders pass 164/164; defensive combat remains partial.
- The 2026-09-17 reference is frozen; later evidence uses dated, hash-pinned deltas.
- The Hero phase and battlefield art rights remain **partial** after the
  [defense recheck](../data/reference/th18-2026-09-23-hero-defense-evidence-closure.json) and [art rights audit](../data/reference/th18-2026-09-23-hero-battlefield-art-rights-audit.json);
  the full suite passed 162 of 162 tests.
- Meteor Golem L1-3 and Meteormite are **partial, data-only** after their
  [evidence recheck](../data/reference/th18-2026-09-23-meteor-golem-meteormite-evidence-recheck.json):
  child values and split, throw, landing, merge and collision remain unknown.
  No Core proxy was enabled; the complete suite passed 156 of 156 tests.
- Ruin Witch and Ruin Knight remain **partial, data-only** in prior deltas;
  the user-directed Hero goal supersedes their active position.
- The permanent Hero goal began with six Heroes, all 12 selectable Home
  Village Pets and 41 Hero Equipment items. The
  [scope delta](../data/reference/th18-2026-09-23-hero-pet-equipment-scope.json)
  and [per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json)
  preserve the earlier Hero gaps and mark catalogue-only Pet and Equipment
  records as unimplemented. The complete suite passed 157 of 157 tests after
  the inventory increment.
- L.A.S.S.I, Electro Owl, Mighty Yak, Unicorn, Frosty/Frostmite and Diggy
  remain **partial, catalogue-only**. Their dated audits, retained data,
  unresolved Core contracts and 157/157 suite results are summarized in the
  [coverage summary](coverage-summary.md) and [per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json).
- Poison Lizard, Phoenix, Spirit Fox, Angry Jelly, Sneezy/Booger and Greedy
  Raven are **partial, catalogue-only** after dated audits. Retained level
  fields, sourced behavior, gaps and 157/157 suite results are in the
  [coverage summary](coverage-summary.md) and [per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json).
- Revenge Deck L1-27 is **partial, catalogue-only** after the
  [2026-09-23 inventory and counter audit](../data/reference/th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json).
  Its official August release makes it the 42nd permanent Equipment item.
  Dated HP boosts, ore costs and Blacksmith requirements are hash-pinned.
  Counter damage, healing, trigger and tick order remain unknown; no Equipment
  Core actor or adapter was enabled. Portal Pendant remains unconfirmed and is
  excluded from the 42-item roster. The complete suite passed 157 of 157 tests.
- All eight Barbarian King Equipment items are **partial, catalogue-only**
  after dated per-item audits. Raged Barbarians and Bracelet Snakes have
  separate spawned-unit rows. The [coverage summary](coverage-summary.md) and
  [per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json)
  retain each sourced level, effect and unresolved trigger, geometry, modifier
  and tick-order contract. No King Equipment Core actor was enabled. Each
  increment passed its complete suite; the official Barbarian Puppet HP/recovery and historical spawn-rate recheck passed 164/164, while child combat remains partial.
- Archer Puppet L1-18 and spawned invisible Archers are **partial,
  catalogue-only** after their [summon audit](../data/reference/th18-2026-09-23-archer-puppet-summon-contract-audit.json).
  Official sources confirm active Queen summons and HP recovery; a dated 2024
  table matches pinned invisibility bands. Current child level, spawn phase,
  formation, invisibility timing and recovery order remain open. No Equipment
  or child Core actor was enabled. The original audit passed 157/157; the official L1-18 Queen DPS/recovery and child invisibility recheck passed 164/164, with child lifecycle still partial.
- Invisibility Vial L1-18 is **partial, catalogue-only** after its
  [damage and target audit](../data/reference/th18-2026-09-23-invisibility-vial-damage-and-target-audit.json).
  Official notes exclude its extra damage from Rage boosts and Magic Mirror
  clones. Current duration, extra-shot staging, target loss and tick order
  remain open. No Equipment Core actor was enabled. The full suite passed
  157/157.
- Giant Arrow L1-18 is **partial, catalogue-only** after its
  [Air Defense and damage audit](../data/reference/th18-2026-09-23-giant-arrow-air-defense-damage-audit.json).
  Supercell published 2x Air Defense damage and new L9/12/15/18 projectile
  values. The other 14 current values, direction, collision and impact tick
  order remain open. No Equipment Core actor was enabled. The full suite
  passed 157/157.
- Healer Puppet L1-18 and spawned Healers L4-8 are **partial,
  catalogue-only** after their [summon and healing audit](../data/reference/th18-2026-09-23-healer-puppet-summon-and-healing-audit.json).
  Official sources confirm Queen summoning, self healing and HP bonus.
  Current child continuity, placement, healing targets and tick order remain
  open. No Equipment or child Core actor was enabled. The full suite passed
  157/157.
- Frozen Arrow L1-27 is **partial, catalogue-only** after its
  [slow and impact audit](../data/reference/th18-2026-09-23-frozen-arrow-slow-and-impact-audit.json).
  Its post-2024 slow bands match pinned fields, but current continuity,
  target eligibility, cadence effects and impact/expiry order remain open.
  No Equipment Core actor was enabled. The full suite passed 157/157.
- Magic Mirror L1-27 and Queen clones are **partial, catalogue-only** after
  their [clone and invisibility audit](../data/reference/th18-2026-09-23-magic-mirror-clone-and-invisibility-audit.json).
  Official notes exclude Equipment damage bonuses from clones. Current
  clone values, spawn/attack order and Queen invisibility timing remain open.
  No Equipment or child Core actor was enabled. The original audit passed 157/157; the official L1-18 Queen DPS/recovery and child invisibility recheck passed 164/164, with child lifecycle still partial.
- Action Figure/Giant Giant, Monolith Arrow and Henchmen Puppet/Henchmen are
  **partial, catalogue-only** after dated, hash-pinned audits. Their child,
  projectile, timing and interaction gaps are listed in the
  [coverage summary](coverage-summary.md) and [per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json);
  each post-audit full suite passed 157/157.
- Dark Orb L1-18 is **partial, catalogue-only** after its
  [audit](../data/reference/th18-2026-09-23-dark-orb-projectile-and-slow-audit.json): dated L8 HP is
  separate from raw 600; collision, slow and tick order are open; 157/157 passed.
- Metal Pants and Noble Iron L1-18 are **partial, catalogue-only**; barrier,
  recovery, opening-shot and tick gaps are in the [summary](coverage-summary.md); 157/157 passed.
- Dark Crown L1-27 is **partial, catalogue-only** after its
  [defeat-threshold audit](../data/reference/th18-2026-09-23-dark-crown-defeat-threshold-audit.json).
  Three boosts are official; current eligibility, HP semantics and death
  event order remain open. No Core actor was enabled; 157/157 passed.
- Meteor Staff L1-27 is **partial, catalogue-only** after its
  [target/cadence audit](../data/reference/th18-2026-09-23-meteor-staff-target-and-cadence-audit.json).
  Nearest-Defense strikes are official; current damage, cadence, target geometry
  and impact order remain open. No Core actor was enabled; 157/157 passed.
- Eternal Tome, Life Gem, Rage Gem, Healing Tome, Fireball, Lavaloon Puppet
  and Heroic Torch are
  **partial, catalogue-only** after dated audits. The [coverage summary](coverage-summary.md)
  and [matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json)
  retain sourced aura, heal, projectile, summon and Wall effects with open geometry,
  current values, child combat and tick order. No Equipment Core actor was enabled; each full
  suite passed 157/157. The official Healing Tome L1-18 duration recheck passed 164/164; heal pulses remain partial.
- Royal Gem, Seeking Shield, Hog Rider Puppet/children, Haste Vial, Rocket Spear, Electro Boots, Frost Flake, Fire Heart, Flame Blower, Stun Blaster, Electro Fangs and Rocket Backpack are **partial, catalogue-only**; see the [coverage summary](coverage-summary.md). Each item audit passed 157/157. Shared read-only Core, Headless, Viewer and RL catalogue access and the attachment audit pass 158/158 plus RL runtime validation. One Pet/two Equipment slots per Hero are sourced. Electro Fangs is source-backed Passive; Monolith Arrow now has secondary-sourced Passive classification. Core exposes a read-only Monolith Arrow deployed-housing tier projection through all adapters; 159/159 passed. Hero loadouts carry validated Pet and Equipment selections as scenario/replay values through Core, Headless, Viewer and RL; 160/160 and RL runtime passed. The Viewer now renders an editable prebattle selection and safely restarts Core on apply; its default practice army includes three air Heroes. ImGui render, Apply mouse click and bridge restart passed. Snapshot restoration binds to the chosen loadout while the logical state hash excludes its currently inert selection; the full 161/161 suite and RL runtime validation passed. Representative Pet and Equipment picker mouse selection and selected-loadout Viewer replay playback passed 162/162. All 12 Pet and 42 Equipment selections render at their highest catalogued level and Core accepts them; the full suite passed 162/162. All 12 Pet and 42 Equipment picker mouse clicks pass the 162/162 suite; battlefield visuals remain unverified. No Pet or Equipment combat actor is enabled. Core now retains all 1,073 pinned Pet/Equipment level records and provenance as immutable read-only data; Headless, Viewer and RL expose selected records. The full 161/161 suite and RL runtime validation passed. Royal Gem L1-18 HP/DPS, Earthquake Boots' eight-tile radius and L9-18 post-2025 building damage, Vampstache L1-18 DPS, and Electro Boots L1-27 aura DPS/self-healing match official tables; the full suite passed 164/164 after each delta. Combat timing and effects remain partial.

## Doing now

Hero/Pet/Equipment integration remains paused for the new-source
[handoff](hero-pet-equipment-handoff.md). The separate 16 ms migration is complete; 165/165 Viewer tests and RL runtime pass.

## Next

Resolve sourced attachment, active/passive effects and spawned subunits; verify adapters, snapshots and replays. Continue independent items when a rule remains unsourced.

## Rule

Import only sourced values. Record every missing combat rule in its dated delta
and coverage entry. Never replace it with a guessed behavior.
