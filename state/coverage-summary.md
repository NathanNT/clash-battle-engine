# TH18 coverage rule

The Core now advances in fixed 16 ms steps; see the
[migration audit](../docs/tick-migration-2026-09-23.md). The 165/165
Viewer-build suite and separate RL runtime validation passed. This scheduling
change adds no sourced Hero, Pet or Equipment behavior, so their coverage axes
remain unchanged.

The [2026-09-23 Hero/Pet/Equipment handoff](hero-pet-equipment-handoff.md)
summarizes verified paths, remaining combat and art work, and evidence needed
from the proposed new source. This documentation-only handoff changes no
coverage status.

The frozen Home Village TH18 reference is dated 2026-09-17. New information is
added only through dated, hash-pinned deltas in `data/reference/`. The frozen
reference is never overwritten.

## What counts as integrated

An item is complete only when its sourced levels and variants are materialized
by Core, usable through Headless, Viewer, and RL, rendered with a documented
asset, serializable in snapshots and replays, deterministic, and covered by
focused tests. The audit distinguishes those checks independently.

## When information is incomplete

Use the strongest available public source, preferring Supercell. Record each
URL, title, publication and consultation dates, version, level, variant,
confidence, and derivation in the additive delta.

If a combat-relevant value is still unknown, do not guess it. Record the
missing rule and sources checked, leave that behaviour disabled or explicitly
partial, and move to the next independent item. A catalogue row, generic
rendering fallback, or approximation is never marked complete.

## Where to look

- `data/reference/` contains the versioned source records and uncertainties.
- `docs/home-village-coverage.md` is the human-readable per-item matrix.
- `python tools/audit_th18_conformity.py` produces the current exhaustive
  adapter, data, test, and fidelity status.

## Hero phase

The active persistent goal includes the six Heroes plus 12 Pets and 42
Equipment items in the [per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json).
The [scope delta](../data/reference/th18-2026-09-23-hero-pet-equipment-scope.json)
records the source-backed assignment and two-equipment-slot boundary. Pets
and Equipment remain catalogue-only at this first inventory increment. The
complete suite passed 157 of 157 tests, including the new matrix validation.

L.A.S.S.I L1-15 remains partial and catalogue-only after its
[combat-contract audit](../data/reference/th18-2026-09-23-lassi-combat-contract-audit.json).
Its sourced high-level behavior cannot yet be resolved through an exact
paired-Pet lifecycle. The complete 157 of 157 suite passed after the audit;
Electro Owl was audited next.

Electro Owl L1-15 remains partial and catalogue-only after its
[level and behavior audit](../data/reference/th18-2026-09-23-electro-owl-level-and-behavior-audit.json).
The catalogue now preserves the pinned attack and healing fields for all 12
Pets, plus official Owl L11-15 hitpoints and DPS. The current-build and
paired-Pet combat contract gaps remain explicit in the per-item matrix. The
complete post-change suite passed 157 of 157 tests; Mighty Yak is the next
audited Pet.

Mighty Yak L1-15 remains partial and catalogue-only after its
[level and Wall-contract audit](../data/reference/th18-2026-09-23-mighty-yak-level-and-wall-contract-audit.json).
Pinned per-level Wall damage and official L11-15 HP and DPS are preserved.
The original splash claim is superseded, and current Wall selection after the
2026 AI fix, Hero lifecycle, speed and event order remain unresolved. The
complete post-change suite passed 157 of 157 tests; Unicorn is the next
audited Pet.

Unicorn L1-15 remains partial and catalogue-only after its
[healing and lifecycle audit](../data/reference/th18-2026-09-23-unicorn-healing-and-lifecycle-audit.json).
Pinned pulse values and official L4-10 heal values are retained; L11-15
availability is official but its numerical values and deterministic pulse or
post-Hero-loss rules remain unverified. The complete post-change suite passed
157 of 157 tests; Frosty and Frostmites were audited next.

Frosty L1-15 and Frostmite remain partial and catalogue-only after their
[contract audit](../data/reference/th18-2026-09-23-frosty-frostmite-contract-audit.json).
Pinned summon counts and caps are retained, but Frostmite child stats, spawn
timing and slow behavior are unsourced. The temporary Frostmite Spell is
excluded. The complete post-change suite passed 157 of 157 tests; Diggy is
the next audited Pet.

Diggy L1-15 remains partial and catalogue-only after its
[level and behavior audit](../data/reference/th18-2026-09-23-diggy-current-level-and-behavior-audit.json).
The [L11-15 observation](../data/reference/th18-2026-09-23-diggy-l11-l15-observation.json)
adds dated secondary game-file HP, DPS and upgrade values, pinned by SHA-256
and disabled for Core materialization. Exact current stun and underground
targeting after the 2026 rework, plus Hero reassignment timing, remain open.
The complete post-change suite passed 157 of 157 tests; Poison Lizard is the
next active Pet.

Poison Lizard L1-15 remains partial and catalogue-only after its
[level and effect audit](../data/reference/th18-2026-09-23-poison-lizard-level-and-effect-audit.json).
Dated game-file HP/DPS and pinned per-level poison DPS and slow percentages
are retained. Official sources support enemy-unit preference and the 4.5-tile
range; exact poison timing, stacking, search radius, projectile and paired-Hero
transitions remain open. No Core or adapter actor was enabled. The complete
post-change suite passed 157 of 157 tests; Phoenix is the next active Pet.

Phoenix L1-10 remains partial and catalogue-only after its
[revival and interaction audit](../data/reference/th18-2026-09-23-phoenix-revival-and-interaction-audit.json).
The dated HP/DPS table confirms the TH18 level cap and pinned per-level revival
durations are retained. Official sources describe its egg, temporary Hero
protection, Grand Warden passive retention and Dragon Duke dash interaction.
Fatal-hit interception, shield timing, pathing and splash remain unresolved.
No Core or adapter actor was enabled. The complete post-change suite passed
157 of 157 tests; Spirit Fox is the next active Pet.

Spirit Fox L1-10 remains partial and catalogue-only after its
[invisibility contract audit](../data/reference/th18-2026-09-23-spirit-fox-invisibility-contract-audit.json).
Official per-level Spirit Walk durations and a six-second visible interval are
documented, while the dated level table confirms TH18 L10 HP/DPS. First
activation, target loss during invisibility, paired-Hero transitions and tick
order remain unresolved. No Core or adapter actor was enabled. The complete
post-change suite passed 157 of 157 tests; Angry Jelly is the next active Pet.

Angry Jelly L1-10 remains partial and catalogue-only after its
[Brainwash contract audit](../data/reference/th18-2026-09-23-angry-jelly-brainwash-contract-audit.json).
Official attached immunity, defense-only Hero targeting, detachment and
per-level duration are documented. Two dated game-file sources support a
five-tile attack range over the pinned older 1.5-tile value. Attachment start,
Hero and Pet target transitions, and detach timing remain open. No Core or
adapter actor was enabled. The complete post-change suite passed 157 of 157
tests; Sneezy and its spawned air troops are the next active Pet increment.

Sneezy L1-10 and Booger remain partial and catalogue-only after their
[contract audit](../data/reference/th18-2026-09-23-sneezy-booger-contract-audit.json).
Sourced parent HP/DPS and pinned cap/rage fields are retained. Booger has a
separate per-item row; official sources say it attacks Buildings and no longer
triggers traps. Child values, spawn/cap and rage timing, Grand Warden follow
weight and range-mode meaning remain open. The complete post-change suite
passed 157 of 157 tests; Greedy Raven is the next active Pet.

Greedy Raven L1-10 remains partial and catalogue-only after its
[resource-target audit](../data/reference/th18-2026-09-23-greedy-raven-resource-target-audit.json).
Dated HP/DPS and eight-tile range are confirmed; pinned per-level resource
DPS equals the official five-times multiplier. Nearby search radius, eligible
resource buildings, hit rounding and paired-Hero transitions remain open.
No Core or adapter actor was enabled. The complete post-change suite passed
157 of 157 tests. All 12 Pets have been audited; Equipment inventory
correction for Revenge Deck is the next active increment.

Revenge Deck L1-27 is partial and catalogue-only after its
[inventory and counter audit](../data/reference/th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json).
Supercell confirms its August 2026 Dragon Duke release. The dated level
observation preserves HP boosts, ore costs and Blacksmith requirements, raising
the active Equipment inventory from 41 to 42 without editing the frozen index.
Counter damage, any healing, trigger eligibility and tick ordering remain
unsourced, so no Core actor or adapter behavior was enabled. Portal Pendant is
excluded pending release evidence. The full suite passed 157 of 157 tests;
individual Equipment audits are next.

Barbarian Puppet L1-18 and its spawned Raged Barbarians remain partial and
catalogue-only after the [summon contract audit](../data/reference/th18-2026-09-23-barbarian-puppet-summon-contract-audit.json).
Official sources establish the active King summon, two Equipment slots and HP
bonuses; dated tables corroborate the pinned HP and ore fields. Spawned-unit
level, placement and tick order, Rage duration boundaries and modifier order,
and Hero recovery timing are unresolved. The child has a separate coverage
row. No Core actor or adapter behavior was enabled. The full suite passed
157 of 157 tests; Rage Vial is next.

The [official Barbarian Puppet bonus and spawn-rate recheck](../data/reference/th18-2026-09-23-barbarian-puppet-official-bonus-and-spawn-rate.json)
pins all 18 King HP/recovery pairs, a 20-second summoned Rage duration and the
announced five-unit batch every 500 ms. It supplies neither the current child
total nor first-batch phase, unit level, formation, collision or recovery tick
order. Parent Equipment and spawned child remain partial and catalogue-only;
the complete 164/164 suite passed after the additive delta.

Rage Vial L1-18 remains partial and catalogue-only after its
[effect-order audit](../data/reference/th18-2026-09-23-rage-vial-effect-order-audit.json).
Official sources establish the active King Rage, damage and movement bonuses,
and HP recovery. Dated tables corroborate the pinned DPS and recovery values.
Passive versus active damage application, movement rounding, activation and
expiry ticks, recovery phase and modifier stacking remain open. The full suite
passed 157 of 157 tests; Earthquake Boots is next.

Earthquake Boots L1-18 remains partial and catalogue-only after its
[ground-target audit](../data/reference/th18-2026-09-23-earthquake-boots-ground-target-audit.json).
Official 2025 notes reduced Building damage and excluded Skeleton Trap
skeletons; official 2026 notes added ground-troop damage. Dated tables confirm
pinned Hero HP and DPS. Current area percentages, radius geometry, Wall
formula, target exceptions and effect tick order remain open. No Core or
adapter actor was enabled. The full suite passed 157 of 157 tests;
Vampstache is next.

The [official Earthquake Boots radius and damage recheck](../data/reference/th18-2026-09-23-earthquake-boots-official-radius-and-damage-table.json)
pins Supercell's eight-tile radius at every level and its post-2025 L9-18
building damage percentages. All ten percentages match the retained catalogue
rows. The radius center, footprint and edge test, L1-8 current building damage,
Wall and ground-troop formulas, Skeleton Trap exception after 2026, activation
and tick order remain unknown. No Equipment effect was enabled; the complete
164/164 suite passed, including the active reference and per-item matrix checks.

Vampstache L1-18 remains partial and catalogue-only after its
[heal-event audit](../data/reference/th18-2026-09-23-vampstache-heal-event-audit.json).
Supercell confirms passive healing on each King attack; dated tables verify
its DPS progression. Current heal-per-hit and attack-speed values, attack
versus impact trigger, target eligibility, heal cap, death order and cadence
stacking remain open. No Core or adapter actor was enabled. The full suite
passed 157 of 157 tests; Giant Gauntlet is next.

The [official Vampstache DPS recheck](../data/reference/th18-2026-09-23-vampstache-official-dps-table.json)
pins all 18 post-buff DPS bonuses from Supercell's localized table; a dated
September 2026 reported build shows the same sequence. Healing amounts,
attack-speed values, target eligibility, event phase, damage-per-hit conversion
and stacking remain open. No combat effect was enabled; the complete 164/164
suite passed, including active reference and per-item matrix checks.

Giant Gauntlet L1-27 remains partial and catalogue-only after its
[current-effect audit](../data/reference/th18-2026-09-23-giant-gauntlet-current-effect-audit.json).
The official launch table establishes the 2.5-tile area and historical
per-level effect values; Supercell increased duration and damage reduction
at every level in January 2026. Dated public tables confirm DPS and costs but
omit the current duration and reduction. Area hitboxes, regeneration and
modifier tick order also remain open. No Core or adapter actor was enabled.
The full suite passed 157 of 157 tests; Spiky Ball was audited next.

Spiky Ball L1-27 remains partial and catalogue-only after its
[bounce and count audit](../data/reference/th18-2026-09-23-spiky-ball-bounce-and-count-audit.json).
Supercell specifies Building-only bounces and a 2024 size-aware initial-target
change. Its historical launch target counts differ from the pinned raw series
at all 27 levels. Dated 2026 tables corroborate Hero HP, DPS and costs but do
not supply current projectile damage or counts. Bounce path, retargeting and
impact order remain open. No Core or adapter actor was enabled. The full suite
passed 157 of 157 tests; Snake Bracelet was audited next.

Snake Bracelet L1-27 and spawned Snake L1-10 remain partial and catalogue-only
after their [summon audit](../data/reference/th18-2026-09-23-snake-bracelet-spawn-contract-audit.json).
Supercell confirms a permanent King Equipment that summons snakes when the King
takes damage, separately from the temporary Snake Barrel. A dated 2026 table
corroborates Hero HP, DPS and costs; the pinned raw table maps all levels to
Snake levels and summon caps. The current threshold, cap meaning, child combat,
spawn placement and tick order remain open. The child has a separate coverage
row. No Core or adapter actor was enabled. The full suite passed 157 of 157
tests; Stick Horse was audited next.

Stick Horse L1-27 remains partial and catalogue-only after its
[deployment and Wall audit](../data/reference/th18-2026-09-23-stick-horse-deployment-and-wall-audit.json).
Supercell confirms the permanent King Equipment gives a limited
post-deployment Wall hop, speed and damage effect. The pinned level table
retains durations, movement and attack-speed fields, while a dated 2026 table
corroborates Hero HP, DPS and upgrade costs. Movement units, route choice,
whether hopping shares buff expiry and fixed-tick ordering remain open. No
Core or adapter actor was enabled. The full suite passed 157 of 157 tests;
Archer Puppet was audited next.

Archer Puppet L1-18 and its spawned Archers remain partial and catalogue-only
after the [summon audit](../data/reference/th18-2026-09-23-archer-puppet-summon-contract-audit.json).
Supercell confirms an active Queen Archer summon and HP recovery; a dated 2024
balance table matches pinned 3.5-6.5 second invisibility bands and reports
five Archers per second. A dated 2026 table corroborates Queen DPS and costs
but omits summon values. Current child level, spawn phase, invisibility timing,
recovery order and interactions remain open. The child has a separate coverage
row. No Core or adapter actor was enabled. The full suite passed 157 of 157
tests; Invisibility Vial was audited next.

The [official Archer Puppet bonus and invisibility recheck](../data/reference/th18-2026-09-23-archer-puppet-official-bonus-and-invisibility-table.json)
pins all 18 Queen DPS/recovery pairs, all 18 spawned Archer invisibility bands
and the historical five-unit batch every 1000 ms. Child level, first spawn
phase, formation, invisibility boundaries and recovery order remain unknown.
Parent and child combat stay partial; the complete 164/164 suite passed.

Invisibility Vial L1-18 remains partial and catalogue-only after its
[damage and target audit](../data/reference/th18-2026-09-23-invisibility-vial-damage-and-target-audit.json).
Supercell establishes active Queen invisibility and stronger attacks, excludes
the Vial's extra damage from Rage boosts, and excludes Equipment damage boosts
from Magic Mirror clones. A dated 2026 table confirms Queen HP and upgrade
costs but omits effect values. Current duration, shot staging, target loss,
projectile interactions and expiry order remain open. No Core or adapter actor
was enabled. The full suite passed 157 of 157 tests; Giant Arrow was audited
next.

Giant Arrow L1-18 remains partial and catalogue-only after its
[Air Defense and damage audit](../data/reference/th18-2026-09-23-giant-arrow-air-defense-damage-audit.json).
Supercell's May 2026 table establishes 2x Air Defense damage at every level
and new base damage at L9/12/15/18 of 1100/1200/1350/1500. Those sparse values
are exposed in catalogue metadata while the raw source fields retain their
historical values. The other 14 post-May values, arrow direction, collision
geometry, travel and impact tick order remain unknown. No Core or adapter
actor was enabled. The full suite passed 157 of 157 tests; Healer Puppet was
audited next.

Healer Puppet L1-18 and spawned Healers L4-8 remain partial and catalogue-only
after their [summon and healing audit](../data/reference/th18-2026-09-23-healer-puppet-summon-and-healing-audit.json).
Supercell confirms the Queen's active Healer summon, self healing and HP
increase. Pinned count, child-level and self-heal fields match an independent
table without a stated client version; a dated 2026 table confirms HP and
costs, and ordinary Healer L4-8 data is separately recorded. Child stat
inheritance, placement, healing target choice, first pulse and passive
regeneration tick order remain unknown. No Core or adapter actor was enabled.
The full suite passed 157 of 157 tests; Frozen Arrow was audited next.

Frozen Arrow L1-27 remains partial and catalogue-only after its
[slow and impact audit](../data/reference/th18-2026-09-23-frozen-arrow-slow-and-impact-audit.json).
Supercell confirms passive Queen arrows that slow on hit. A dated September
2024 balance table lowered slow values at all levels; its 25-65 percent bands
match the pinned payload. A dated 2026 public table confirms Queen DPS but
omits the slow fields. Current slow continuity, target eligibility,
movement/cadence effect, projectile impact, stacking and expiry phase remain
open. No Core or adapter actor was enabled. The full suite passed 157 of 157
tests; Magic Mirror was audited next.

Magic Mirror L1-27 and its Queen clones remain partial and catalogue-only
after their [clone and invisibility audit](../data/reference/th18-2026-09-23-magic-mirror-clone-and-invisibility-audit.json).
Supercell confirms clone spawning, one-second original-Queen invisibility,
and exclusion of Equipment damage and passive DPS boosts from clones. Pinned
clone count, duration, HP and DPS match an independent table without a stated
client version; a dated 2026 table confirms Queen HP but omits clone effects.
Current clone continuity, spawn position, attack and expiry tick order,
Queen invisibility target loss and other modifier interactions remain open.
No Core or adapter actor was enabled. The full suite passed 157 of 157 tests;
Action Figure was audited next.

Action Figure L1-27 and its Giant Giant child L1-10 remain partial and
catalogue-only after their [spawn contract audit](../data/reference/th18-2026-09-23-action-figure-giant-giant-contract-audit.json).
Supercell confirms the permanent Queen Equipment summon. The pinned child
level mapping, Queen self healing, HP and DPS bonuses are data-only; a dated
2026 public table corroborates Queen HP/DPS. A separate September event troop
called Giant Giant does not establish the Equipment child's stats or AI.
Current child values, placement, target order, Queen invisibility and
self-heal tick order remain open. No Core or adapter actor was enabled. The
full suite passed 157 of 157 tests; Monolith Arrow was audited next.

Monolith Arrow L1-27 remains partial and catalogue-only after its
[housing and projectile audit](../data/reference/th18-2026-09-23-monolith-arrow-housing-and-projectile-audit.json).
Official Supercell text pins the Queen's 14/10/5 percent target-max-HP tiers
by deployed housing and the excluded housing classes. The current public
table corroborates the frozen 27 Queen HP bonuses. The supplemental Hero
assignment now resolves to Archer Queen in the per-item matrix. At that audit,
activation classification and attack-link timing,
the 10 percent damage-reduction recipient, housing sample time, projectile
impact and integer rounding remain unsourced. No Core or adapter actor was
enabled. The full suite passed 157 of 157 tests; Henchmen Puppet is next.

Henchmen Puppet L1-18 and its spawned Henchmen L1-7 remain partial and
catalogue-only after their
[child contract audit](../data/reference/th18-2026-09-23-henchmen-puppet-and-child-contract-audit.json).
Its active Minion Prince summon and two flying children are supported by
dated release evidence and the frozen payload. Official TH18 notes confirm
the +500 HP Equipment adjustment, and a dated game-file table corroborates
the 18 DPS bonuses. Current child combat values, summon position, recovery,
invisibility and tick order remain open; the undated community child table is
a research lead only. No Equipment or child Core actor was enabled. The full
suite passed 157 of 157 tests; Dark Orb is next.

Dark Orb L1-18 remains partial and catalogue-only after its
[projectile and slow audit](../data/reference/th18-2026-09-23-dark-orb-projectile-and-slow-audit.json).
The dated table corroborates Prince DPS bonuses and identifies a current
L8 HP bonus of 690; the pinned raw 600 is preserved separately. Historical
projectile damage 45-200 and slow 20-50 percent for 8-18 seconds remain
unverified for current combat. Launch, collision, targets, slow stacking,
expiry and tick order are open. No Equipment Core actor was enabled. The
full suite passed 157 of 157 tests; Metal Pants is next.

Metal Pants L1-18 remains partial and catalogue-only after its
[barrier and recovery audit](../data/reference/th18-2026-09-23-metal-pants-barrier-and-recovery-audit.json).
Supercell confirms permanent Prince Equipment and temporary protection;
the dated table confirms all 18 Prince HP bonuses. The pinned duration,
reduction and recovery fields are historical only. Eligible damage origins,
mitigation arithmetic, activation, expiry, recovery and interaction ordering
remain open. No Equipment Core actor was enabled. The full suite passed 157
of 157 tests; Noble Iron is next.

Noble Iron L1-18 remains partial and catalogue-only after its
[opening-shot audit](../data/reference/th18-2026-09-23-noble-iron-opening-shots-audit.json).
Supercell confirms passive stronger, faster, longer-ranged opening Prince
projectiles and January 2026 all-level range, count and speed buffs. The
dated client table confirms +500 Prince HP; the pinned post-buff candidate
effect series lacks current version confirmation. Charge use on missed or
cancelled shots, target acquisition, cooldown staging, impact and defensive
use remain open. No Equipment Core actor was enabled. The full suite passed
157 of 157 tests; Dark Crown is next.

Dark Crown L1-27 remains partial and catalogue-only after its
[defeat-threshold audit](../data/reference/th18-2026-09-23-dark-crown-defeat-threshold-audit.json).
Supercell confirms three HP/power boosts after friendly Troop defeats.
The dated launch report gives historical cumulative 60/120/180 defeated
housing thresholds and exclusions; the 2026 table confirms Prince HP
bonuses only. Current eligibility, per-stage HP growth, current-HP fill,
damage and attack-speed stacking, simultaneous deaths and tick order remain
open. No Equipment Core actor was enabled. The full suite passed 157 of 157
tests; Meteor Staff is next.

Meteor Staff L1-27 remains partial and catalogue-only after its
[target/cadence audit](../data/reference/th18-2026-09-23-meteor-staff-target-and-cadence-audit.json).
Supercell confirms repeated strikes against the nearest Defense. The dated
2026 table confirms Prince HP/DPS bonuses; pinned effect cooldown and damage
are historical candidates. Current values, first strike, target geometry,
projectile travel, impact and retargeting remain open. No Equipment Core actor
was enabled. The full suite passed 157 of 157 tests; Eternal Tome is next.

Eternal Tome L1-18 remains partial and catalogue-only after its
[aura-immunity audit](../data/reference/th18-2026-09-23-eternal-tome-aura-immunity-audit.json).
Official notes confirm Warden and nearby-unit damage immunity, immunity for
Troops entering after activation and one second of protection after exit.
The pinned 3.2-8.2 second series matches a dated post-change table; the June
2025 +0.2 second change is already included. Current aura geometry, eligible
classes, activation and damage tick order remain open. No Equipment Core
actor was enabled. The full suite passed 157 of 157 tests; Life Gem is next.

Life Gem L1-18 remains partial and catalogue-only after its
[balance/aura audit](../data/reference/th18-2026-09-23-life-gem-balance-and-aura-audit.json).
Official January 2026 changes increased L1-7 extra HP/caps and L1-18 Warden
bonuses, superseding the pinned raw series without full per-level replacements.
Official February and April notes exclude Totems and siege machines. Aura
geometry, eligible in-scope classes, current-HP conversion, stacking and tick
order remain open. No Equipment Core actor was enabled. The full suite passed
157 of 157 tests; Rage Gem is next.

Rage Gem L1-18 remains partial and catalogue-only after its
[aura/stacking audit](../data/reference/th18-2026-09-23-rage-gem-aura-and-stacking-audit.json).
Official material confirms a passive nearby-unit damage aura and separate
Warden DPS/attack-speed bonuses. A dated 2026 table corroborates Warden DPS;
pinned aura percentages lack current confirmation. Aura geometry, eligible
classes, damage staging, stacking, self-inclusion and tick order remain open.
No Equipment Core actor was enabled. The full suite passed 157 of 157 tests;
Healing Tome is next.

Healing Tome L1-18 remains partial and catalogue-only after its
[aura/pulse audit](../data/reference/th18-2026-09-23-healing-tome-aura-and-pulse-audit.json).
Official material confirms active healing for Warden and nearby friendly
units. A dated 2024 balance table matches pinned 15-20 second durations;
the raw 60-150 healing-per-second rates lack current confirmation. Aura
geometry, eligible classes, first pulse, integer rounding, Warden recovery,
stacking and damage/healing tick order remain open. No Equipment Core actor
was enabled. The full suite passed 157 of 157 tests; Fireball is next.

The [official Healing Tome duration recheck](../data/reference/th18-2026-09-23-healing-tome-official-duration-table.json)
pins all 18 post-September-2024 durations at 15-20 seconds, matching the
retained rows and fixed 10 ms grid. The source does not provide heal cadence,
integer accumulation, aura geometry or activation/death tick order. No combat
effect was enabled; the complete 164/164 suite passed after this delta.

The [Healing Tome level provenance delta](../data/reference/th18-2026-09-23-healing-tome-primary-level-provenance.json)
places the 18 verified duration values and a hash-pinned primary source link in
immutable Equipment level records. Headless and RL return the same Core row;
the Viewer selected-level panel reads that row. The full 164/164 suite and RL
runtime passed. This is read-only evidence. Current-client continuity, heal
rate, pulse arithmetic, aura geometry, activation state and effect visuals
remain partial.

The [six-Equipment primary level provenance delta](../data/reference/th18-2026-09-23-six-equipment-primary-level-provenance.json)
adds 117 read-only official level records for Royal Gem, Earthquake Boots,
Vampstache, Barbarian Puppet, Archer Puppet and Electro Boots. Values retain
their primary source IDs and source-delta hashes in immutable Core records;
Headless, Viewer and RL read that same data. The full 164/164 suite and RL
runtime passed. Combat activation, auras, summons, healing, stacking,
current-client continuity and future-influencing state remain partial.

Fireball L1-27 remains partial and catalogue-only after its
[target/splash audit](../data/reference/th18-2026-09-23-fireball-target-size-and-splash-audit.json).
Official notes establish nearest-Defense area damage, building-size-aware
initial selection and ground/air splash. The 2024 launch L18 radius is
obsolete after the 2025 change; pinned data puts 6 tiles at L24. Current
damage/radius continuity, exact target metric, impact and splash geometry,
eligibility and tick order remain open. No Equipment Core actor was enabled.
The full suite passed 157 of 157 tests; Lavaloon Puppet followed.

Lavaloon Puppet L1-27 remains partial and catalogue-only after its
[summon and child audit](../data/reference/th18-2026-09-23-lavaloon-puppet-and-child-contract-audit.json).
Supercell confirms the L21-27 count increased from two to three and reduced
spawned Lavaloon housing by 8.7%; a dated secondary report says 23 to 21.
Pinned data gives counts 1/2/3 and child level 1-11. The permanent spawned
Lavaloon lacks current HP, damage, movement, target, death and placement
rules. Official Lavaloon Pup housing is one, but its relationship to the
permanent Equipment variant is not verified. No Equipment or child Core actor
was enabled. The full suite passed 157 of 157 tests; Heroic Torch followed.

Heroic Torch L1-27 remains partial and catalogue-only after its
[balance and Wall audit](../data/reference/th18-2026-09-23-heroic-torch-balance-and-wall-audit.json).
Official notes confirm nearby Troop Wall pass-through, speed and damage
reduction, Warden speed inclusion and Totem exclusion. January 2026 buffs
supersede pinned HP, DPS, recovery, duration and reduction without publishing
replacement values by level. Aura eligibility, radius, damage stage, Wall
exit and activation order remain open. No Equipment Core actor was enabled.
The full suite passed 157 of 157 tests; Royal Gem followed.

Royal Gem L1-18 remains partial and catalogue-only after its
[recovery and bonus audit](../data/reference/th18-2026-09-23-royal-gem-recovery-and-bonus-audit.json).
Official material confirms Champion self-healing and HP/DPS bonuses. A dated
2026 table and historical buff report match pinned HP 60-570 and DPS 35-120.
The pinned 1200-2400 active heal has no current confirmation; cap, trigger,
same-tick order, max/current HP conversion and Equipment interactions remain
open. No Equipment Core actor was enabled. The full suite passed 157 of 157
tests; Seeking Shield followed.

The [official Traditional Chinese Royal Gem level table](../data/reference/th18-2026-09-23-royal-gem-official-level-bonus-table.json)
prints every post-buff HP and DPS bonus at L1-18. All 18 pairs match the
retained catalogue and a dated September 2026 secondary table; the active
reference and full 164/164 suite passed. This closes the numeric provenance
axis for those bonuses. The table does not establish HP application at
deployment, DPS-to-hit rounding, two-Equipment stacking or active-heal
timing, so Royal Gem combat coverage remains partial.

The [Royal Gem active-heal recheck](../data/reference/th18-2026-09-23-royal-gem-active-heal-source-recheck.json)
records five checked sources. An accessible 2026 reference displays HP/DPS but
no heal amount; another page derives from the same upstream raw payload and
also omits the heal series. An unversioned historical table disagrees with the
frozen heal values, while a matching search snippet could not be inspected
because the page returned HTTP 403. Current L1-18 healing, activation phase,
cap and same-tick order remain unverified. The hash-pinned delta and complete
164/164 suite passed; Royal Gem combat remains partial.

Seeking Shield L1-18 remains partial and catalogue-only after its
[target and bounce audit](../data/reference/th18-2026-09-23-seeking-shield-target-and-bounce-audit.json).
Official sources establish four Defense targets regardless of range,
defensive Builder exclusion, and a later building-size-aware initial target
selection. Dated 2026 data confirms the HP bonus; pinned projectile damage
1000-2500 has no current confirmation. First-target metric, bounce search,
repeat/target-loss policy, projectile timing and damage order remain open.
No Equipment Core actor was enabled. The full suite passed 157 of 157 tests;
Hog Rider Puppet followed.

Hog Rider Puppet L1-18 and its spawned Hog Riders remain partial and
catalogue-only after their
[summon and child audit](../data/reference/th18-2026-09-23-hog-rider-puppet-and-child-audit.json).
Official notes establish the summon and January 2026 increases to spawned
count and level at Equipment L1-7, but give only percentage ranges. Pinned
count is 8-10 and child level 5-12; current L1-7 exact values, child stat
inheritance, formation, spawn tick, Champion invisibility/recovery order and
interactions remain open. No Equipment or child Core actor was enabled. The
full suite passed 157 of 157 tests; Haste Vial followed.

Haste Vial L1-18 remains partial and catalogue-only after its
[cadence and speed audit](../data/reference/th18-2026-09-23-haste-vial-cadence-and-speed-audit.json).
Official release notes establish Champion movement and attack-speed boosts;
the April 2024 passive L9-18 adjustment matches pinned percentages. Dated
data confirms the DPS bonus. Current active duration/speed continuity,
movement units, fixed-tick cadence conversion, stacking and expiry order
remain open. No Equipment Core actor was enabled. The full suite passed
157 of 157 tests; Rocket Spear followed.

Rocket Spear L1-27 remains partial and catalogue-only after its
[charged shots audit](../data/reference/th18-2026-09-23-rocket-spear-charged-shots-audit.json).
The official launch establishes level-dependent boosted Champion spear throws
with greater damage and range. Pinned data records 7-10 shots, 350-980 extra
damage per shot, 10 tile range and 0.8 tile area radius; dated data confirms
the HP/DPS bonuses. Current charged-shot values, projectile and splash
geometry, charge consumption, activation and impact order remain open. No
Equipment Core actor was enabled. The full suite passed 157 of 157 tests;
Electro Boots followed.

Electro Boots L1-27 remains partial and catalogue-only after its
[aura and balance audit](../data/reference/th18-2026-09-23-electro-boots-aura-and-balance-audit.json).
Official release establishes a periodic Royal Champion damage aura, and the
January 2025 announcement says it was subsequently toned down. Pinned data
records a 400 ms pulse, 5 tile radius, 132-200 DPS, fractional damage per hit
and 6-45 self-healing per second; dated data confirms HP bonuses. Current
post-nerf values, target geometry, fractional damage/healing arithmetic and
tick order remain open. No Equipment Core actor was enabled. The full suite
passed 157 of 157 tests; Frost Flake followed.

The [official Electro Boots aura and healing recheck](../data/reference/th18-2026-09-23-electro-boots-official-aura-and-healing-table.json)
confirms all 27 retained aura DPS and self-healing rates against Supercell's
post-2025 nerf table. It does not establish the 400 ms pulse, five-tile radius,
fractional per-hit damage, integer healing, target selection or same-tick order
as current executable rules. No aura actor was enabled; the full 164/164 suite
passed after the additive delta.

Frost Flake L1-27 remains partial and catalogue-only after its
[freeze and target audit](../data/reference/th18-2026-09-23-frost-flake-freeze-and-target-audit.json).
Official support lists it as permanent Royal Champion Epic Equipment; dated
community coverage describes simultaneous freeze projectiles with defense
priority and fallback targets. Pinned levels record 4-7 targets, 105-294
projectile damage, 5.2-7 second duration and a 100% slowdown label. Current
values, target geometry, projectile travel and freeze tick order remain open.
No Equipment Core actor was enabled. The corrected full suite passed 157 of
157 tests; Fire Heart followed.

Fire Heart L1-18 remains partial and catalogue-only after its
[balance and death burst audit](../data/reference/th18-2026-09-23-fire-heart-balance-and-death-burst-audit.json).
Supercell's May 2026 table replaces the pinned DPS bonus at L2-18 and lowers
regeneration at L15 and L18. The dated September 2026 table confirms the new
DPS series. The frozen values remain intact for provenance. Current L16/L17
regeneration, death burst values, target geometry, integer healing accrual and
death tick order remain open. No Equipment Core actor was enabled. The full
suite passed 157 of 157 tests; Flame Blower followed.

Flame Blower L1-18 remains partial and catalogue-only after its
[direction and activation audit](../data/reference/th18-2026-09-23-flame-blower-direction-and-activation-audit.json).
Official June 2026 notes fix Flame Blower before Rocket Backpack regardless of
slot; the August fix makes Flame Blower fire in the Duke's facing direction.
The July Backpack direction was temporary. Pinned damage has an anomalous L15
value above L16/L17. Current damage, flame geometry, facing sample and hit
tick order remain open. No Equipment Core actor was enabled. The full suite
passed 157 of 157 tests; Stun Blaster followed.

Stun Blaster L1-18 remains partial and catalogue-only after its
[shockwave and target audit](../data/reference/th18-2026-09-23-stun-blaster-shockwave-and-target-audit.json).
Dated data corroborates Duke HP/DPS bonuses, and pinned data records 150-400
total shockwave damage, 4.5-7.5 second stun and 8 tile radius. Community
sources report multiple pulses but disagree or lack detail on target classes
and recovery. Pulse timing, damage division, geometry and stun order remain
open. No Equipment Core actor was enabled. The full suite passed 157 of 157
tests; Electro Fangs followed.

Electro Fangs L1-18 remains partial and catalogue-only after its
[chain and target audit](../data/reference/th18-2026-09-23-electro-fangs-chain-and-target-audit.json).
The frozen observation and dated September 2026 table cover HP bonuses and
ore costs. Supercell confirms a passive chain of up to four targets, 20% loss
per jump and 400 maximum damage per strike. A community table gives per-level
damage and counts; primary level values, link radius, target selection,
rounding and damage order remain open. No Equipment Core actor was enabled.
The full suite passed 157 of 157 tests; Rocket Backpack followed.

Rocket Backpack L1-27 remains partial and catalogue-only after its
[dash and balance audit](../data/reference/th18-2026-09-23-rocket-backpack-dash-and-balance-audit.json).
Supercell's May 2026 table lowers L21/L27 penetrating damage to 1875/2150.
The August fix makes the dash complete before Duke defeat when Phoenix
revival expires, fixes missing flame and early stop, and restores Flame
Blower's facing direction. Intermediate damage, path, area, collision and
fixed-tick lifecycle remain open. No Equipment Core actor was enabled. The
full suite passed 157 of 157 tests; all 42 Equipment items now have audits.

The shared `GameData` catalogue now exposes all 12 Pets and 42 Equipment items
as read-only identity, eligible Hero where sourced, maximum catalogue level and
support status. Headless `hero-support-catalogue`, the Viewer catalogue panel
and the RL binding read the same Core records. The catalogue path passed the
complete 158 of 158 suite and separate RL runtime validation. It creates no
Pet or Equipment combat actor and changes no battle state, snapshot, replay or
fixed-tick rule. The per-item matrix records catalogue access separately from
the still-unimplemented combat adapter axes.

The [shared attachment audit](../data/reference/th18-2026-09-23-hero-pet-equipment-attachment-contract-audit.json)
pins Supercell's one-Pet and two-Equipment slot descriptions and the manual
Active versus permanent Passive distinction. Core's read-only catalogue now
exposes sourced Equipment ability types in Headless, Viewer and RL: 28 Active,
13 Passive and one explicitly unverified supplemental record. The 12 Pet
records have no Equipment ability type. Slot and type documentation does not
establish a deployment, activation, modifier or death tick order; no combat
actor or command was enabled. The full 158/158 suite and RL runtime check pass.

The [Electro Fangs Passive override](../data/reference/th18-2026-09-23-electro-fangs-passive-type-override.json)
uses Supercell's explicit April 2026 label and leaves the frozen supplemental
level record intact. At that increment, Monolith Arrow was the single
unverified activation type because its primary release note did not classify
the trigger. The complete 158/158 suite and RL runtime check passed.

The [Monolith Arrow Passive override](../data/reference/th18-2026-09-23-monolith-arrow-passive-type-override.json)
uses an explicit dated June 2026 secondary label, repeated in the publisher's
Equipment guide. The official release confirms the item and housing tiers but
does not itself label Active or Passive. The immutable Core catalogue now
reports Passive through Headless, Viewer and RL; focused provenance checks,
the complete 164/164 suite and RL runtime passed. Normal-attack link,
projectile phase, damage arithmetic and reduction remain partial.

The 2026-09-23 [evidence closure](../data/reference/th18-2026-09-23-hero-defense-evidence-closure.json)
marks this phase partial. The primary-source recheck found no complete Hero
Banner defensive contract or Hero-specific geometry, and no asset rights or
battlefield sprite evidence. The complete post-change suite passed 156 of 156
tests on 2026-09-23.

## Ordinary troop phase

Meteor Golem L1-3 and Meteormite remain partial and data-only after the
[2026-09-23 recheck](../data/reference/th18-2026-09-23-meteor-golem-meteormite-evidence-recheck.json).
The parent values are sourced, while current child values, split and merge
timing, landing and collision remain unknown. No combat actor is claimed. The
complete suite passed 156 of 156 tests after this reference update. Ruin Witch
and Ruin Knight are next in execution order.

`data/reference/th18-hero-coverage-2026-09-20.json` is the per-hero matrix.
The Barbarian King, Archer Queen, Grand Warden, Royal Champion and Minion
Prince and Dragon Duke base attacks are integrated partially. Their remaining
intrinsic, defensive and visual gaps are explicit. Royal Champion wall crossing
is implemented through the shared deterministic wall-bypass movement. Each of
the six Heroes has focused snapshot and value-serialized replay verification.
The Hero audit is closed partial. Hero Equipment is excluded. Source icons are
verified and rendered for every Hero, while Hero Banner defense and battlefield
sprite geometry or animation remain documented evidence gaps.

The RL scenario parser now preserves the value-serialized Grand Warden ground
or air ArmySlot mode and all six Hero Banner variants. Its observation exposes
Core flying state and Banner variant; the RL runtime check confirms distinct
ground/air logical hashes and equal hashes across image-only Banner variants.
The complete 164/164 suite passed. Banner patrol, collision and sprite rights
remain partial.

The Viewer prebattle editor now exposes the Grand Warden's source-backed
ground and air scenario modes. Mouse selection is accepted by Core. Both
value-serialized replays restore their selected mode, and Viewer playback
matches direct Core after two 10 ms ticks; the flying flag and distinct
future-state hashes are checked. The full 164/164 suite passed. This does not
resolve the documented in-game transition, collision or Banner behavior gaps.

The 20 percent defense Hero DPS and hitpoint modifiers published by Supercell
are Tournament Mode modifiers, not standard Home Village values. They are
explicitly rejected for the current Core contract in the dated Hero Banner
recheck.

The Minion Prince and Dragon Duke now have their respective pinned-source
defensive targetability restrictions in shared Core targeting and damage
resolution. Hero Banner patrol and timing remain open.

Their allowed defender-target cases now also have focused snapshot and replay
regressions. The serialised command path therefore preserves the exact same
targetability decision after restore and replay, without a Viewer or RL rule.

All six Heroes now also have a focused explicit static defender-placement
regression. It exercises their shared base attacks plus snapshot and replay
determinism without pretending that a scenario placement defines the
undocumented Hero Banner patrol contract. The complete 156 of 156 suite passed
on 2026-09-21.

The dated 2026-09-21 Friendly Challenge modifier audit records primary
Supercell evidence for optional challenge-only Hero modifiers. It introduces no
standard Home Village value or Core rule because it does not specify Hero
Banner patrol, acquisition, return, respawn, collision, or tick ordering.

The companion Hero Banner behavior audit retains secondary claims about target
locking and pursuit, including an unverified 9 or 10 tile radius claim, solely
as research leads. They have no reproducible current measurement or complete
deterministic contract, so they do not enable an approximation in Core.

The Hero collision geometry audit records that neither the primary support page
nor the pinned Hero records publishes a footprint, hitbox, path clearance, or
world anchor. The shared Core radius remains a fallback and is not presented as
verified Hero geometry.

Royal Champion Wall crossing is also now explicitly verified from the defender
side through the same deterministic movement route, snapshot and replay path.
This does not infer any Banner patrol or defensive activation timing.

The dated Hero projectile audit records the checked primary and secondary
sources for Archer Queen, Grand Warden, Royal Champion and Minion Prince. None
publishes the complete current speed, launch, collision and target-loss
contract. The shared T+10 ms Core route remains a documented fallback only.

Dragon Duke's current Royal Rampage trap mitigation is now source-traced as a
20 percent reduction after the July balance change. Core applies its 0.8
incoming multiplier only to trap-origin damage while the shared isolated-flying
predicate holds. Focused snapshot and replay regressions passed in the complete
156 of 156 suite on 2026-09-21.

The pinned 2026-09-21 ClashKingAssets audit is a second public source that
still exposes only Hero icons. It provides no battlefield sprite, world anchor,
scale or animation evidence, so no substitute asset is presented as complete.

The official Supercell Fan Content Policy is now recorded as a separate rights
constraint. Source-pinned Hero icon bytes establish provenance only, not a
compatible redistribution license for this project. No new extracted or fan-kit
character art may be added without written authorization or compatible rights.

The [2026-09-23 battlefield art rights audit](../data/reference/th18-2026-09-23-hero-battlefield-art-rights-audit.json)
pins the current official policy and the exact upstream repository license and
README. The upstream MIT license identifies its repository author but supplies
no separate Supercell grant for battlefield Hero sprites. The checked documents
do not establish compatible redistribution rights for this simulator, so all
six Hero art coverage rows remain partial. Sprite scale, anchors and animation
bounds are also unsourced. No new game-derived sprite was added. The complete
162/162 suite passed after the audit was added to the active reference.

The official Hero Banner assignment page now confirms that the player selects
or swaps the defending Hero at each Banner. The six serialised Banner variants
therefore represent source-backed assignment values and render their pinned
artwork through the shared Viewer path. Patrol and combat remain open because
the source gives no deterministic radius, targeting, return, respawn or timing
contract. The Viewer prebattle editor now lets the player click and apply all
six assignments. A focused test verifies exact scenario and replay values,
snapshot binding, and equal logical hashes for image-only variants; the
complete 163/163 suite passed. Viewer bridge playback of a selected Dragon
Duke Banner replay matches direct Core after two 10 ms ticks. A software
renderer decodes all six pinned local Banner icons and observes distinct scene
pixels for each assignment. The complete 164/164 suite passed. These are
Banner images; battlefield Hero sprite placement and redistribution rights
remain partial.

The TH18 quantity limit is four Hero Banners. It is source-backed and enforced
by Core while preserving the Banner's non-targetable, non-destructible state.

The attacking Home Village army has four Hero slots. Core rejects a larger
in-scope Hero reserve deterministically before simulation begins. Duplicate
selection remains an explicit unknown rather than an assumed prohibition.

Monolith Arrow now has a read-only Core projection of deployed housing and the
official 0-180, 181-250 and 251+ tiers, exposed through Headless, Viewer and RL.
The same Core path projects the official 14/10/5 target maximum-HP percentages;
fixed-tick band boundaries, snapshot, replay and hashes passed 164/164 tests
and RL runtime validation.
It counts deployed attacker Heroes as 25 each, including defeated Heroes, and
excludes spawned units. The [pinned audit](../data/reference/th18-2026-09-23-monolith-arrow-housing-and-projectile-audit.json)
supplies the rule. Snapshot restoration, replay, deterministic hashes and
fixed-tick transitions passed with the complete 159/159 suite; RL runtime
validation also passed. Equipment activation, damage, projectile behavior,
housing sample phase at activation and visual color remain partial.

The [shared attachment audit](../data/reference/th18-2026-09-23-hero-pet-equipment-attachment-contract-audit.json)
now drives value-only `hero_loadouts` in scenario and replay JSON. Core accepts
one selected Pet and at most two eligible Equipment items with validated
catalogue levels for each unambiguous attacking Hero slot; duplicate selections
are rejected pending stronger evidence. Headless prints the selections, Viewer
shows them from its immutable Core frame, and RL returns the same values.
Snapshot restoration binds to the selected loadout; the complete 160/160 suite
and separate RL runtime check passed. Viewer editing and activation controls,
Pet actors, Equipment stat bonuses and effects remain unimplemented.

The Viewer now renders prebattle Pet and Equipment selection controls for its
selected Heroes. It reads eligible items and level caps from Core GameData and
applies a draft by constructing a fresh Core simulation bridge at T+0. Invalid
selections preserve the current battle. The default air practice army includes
three Heroes so the controls are available without a custom scenario. ImGui
rendering, bridge replacement and the full 161/161 suite passed. An additional
ImGui mouse press/release verifies the Apply button fires once; the complete
161/161 suite passed again. Pet and Equipment picker clicks, battlefield Pet
art and Equipment effect overlays remain unverified; the selection itself still
has no combat effect.

Snapshot restore still checks the exact selected loadout. The logical state
hash now omits that selection while it has no combat effect, so otherwise equal
battles hash equally across loadouts. A focused deployment comparison, the
complete 161/161 suite and separate RL runtime validation passed after this
correction. When a selected Pet or Equipment starts affecting combat, its
future-influencing selection must enter the logical hash.

Core GameData now retains the exact normalized content and each of the 1,073
level rows for all 12 permanent Pets and 42 permanent Equipment items, with
the existing pinned provenance embedded in every row. Headless
`hero-support-level`, Viewer selected-level source records and RL
`hero_support_level` read from that same immutable Core data. Every configured
level was checked for a provenance-bearing Core record; representative
Headless and RL values matched the normalized catalogue exactly. The full
161/161 suite and separate RL runtime validation passed. This is read-only
data access; Pet deployment, Equipment stat application, activation, timing
and interactions remain partial.

The Viewer now has automated mouse selection checks for a representative Pet
and eligible Queen Equipment item, followed by Core loadout validation. A
selected-Pet replay is loaded through the Viewer and produces the same state
hash as direct Core after two 10 ms ticks. The complete 162/162 suite passed.
These checks cover the selection and playback path. The broader picker check
is recorded below; battlefield Pet rendering and Equipment effect overlays
remain open.

The same ImGui test now renders all 12 Pet and 42 Equipment selections at each
item's highest catalogued level, then checks Core accepts the displayed
loadout. Mouse clicks for all 12 Pet and 42 Equipment picker entries also
select the expected Core loadout. The complete 162/162 suite passed. Scrolling
itself was positioned by the test harness; battlefield Pet and Equipment
effect visuals remain open.

Headless and RL now check maximum-level Core provenance and selection for each
of the 12 Pets and 42 Equipment items. Headless groups these into 22 legal
one-Hero loadouts, saves each as a value replay and confirms identical output
and state hashes after two 10 ms ticks. RL verifies every selection remains
observable after deployment through the Python-to-Core scenario path. The
complete 164/164 suite and separate RL runtime passed. This closes per-item
selection access checks, while all Pet and Equipment combat axes remain partial.
