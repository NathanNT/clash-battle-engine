# Remaining TH18 work

The persistent Hero/Pet/Equipment goal is paused for the user's new-source
handoff. Read the [handoff](hero-pet-equipment-handoff.md) before resuming this
execution order; it adds no combat claim or new source fact.

The separate [16 ms fixed-step migration](../docs/tick-migration-2026-09-23.md)
is complete (165/165 Viewer-build tests and RL runtime passed). It changes
engine scheduling only; the item order and partial coverage below remain.

## Active persistent goal: Heroes, Pets and Equipment

The [2026-09-23 scope delta](../data/reference/th18-2026-09-23-hero-pet-equipment-scope.json)
expands the earlier partial Hero phase to all six permanent Heroes, 12
selectable Home Village Pets and an initial 41 permanent Hero Equipment items.
The [Revenge Deck correction](../data/reference/th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json)
raises the active roster to 42. The
[per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json)
is the execution ledger. Inventory verification passed in the complete 157 of
157 suite.

1. L.A.S.S.I, Electro Owl, Mighty Yak, Unicorn, Frosty, Diggy and Poison Lizard L1-15,
   plus Phoenix, Spirit Fox, Angry Jelly, Sneezy and Greedy Raven L1-10,
   were marked
   partial after their
   [L.A.S.S.I audit](../data/reference/th18-2026-09-23-lassi-combat-contract-audit.json)
   [Electro Owl audit](../data/reference/th18-2026-09-23-electro-owl-level-and-behavior-audit.json)
   [Mighty Yak audit](../data/reference/th18-2026-09-23-mighty-yak-level-and-wall-contract-audit.json)
   [Unicorn audit](../data/reference/th18-2026-09-23-unicorn-healing-and-lifecycle-audit.json)
   [Frosty/Frostmite audit](../data/reference/th18-2026-09-23-frosty-frostmite-contract-audit.json)
   [Diggy audit](../data/reference/th18-2026-09-23-diggy-current-level-and-behavior-audit.json)
   [Poison Lizard audit](../data/reference/th18-2026-09-23-poison-lizard-level-and-effect-audit.json)
   [Phoenix audit](../data/reference/th18-2026-09-23-phoenix-revival-and-interaction-audit.json)
   [Spirit Fox audit](../data/reference/th18-2026-09-23-spirit-fox-invisibility-contract-audit.json)
   [Angry Jelly audit](../data/reference/th18-2026-09-23-angry-jelly-brainwash-contract-audit.json)
   [Sneezy/Booger audit](../data/reference/th18-2026-09-23-sneezy-booger-contract-audit.json)
   and [Greedy Raven audit](../data/reference/th18-2026-09-23-greedy-raven-resource-target-audit.json).
   Frostmite and Booger are separate unresolved spawned-unit rows in the matrix.
   Diggy L11-15 now has a dated data-only supplement. Poison Lizard retains
   its per-level poison-effect fields and dated HP/DPS. Phoenix retains pinned
   revival durations and official cross-Hero interactions. Spirit Fox retains
   official per-level invisibility durations and the six-second visible time.
   Angry Jelly retains official Brainwash durations and a dated range
   correction. Sneezy retains pinned cap and rage fields; Booger no longer
   triggers traps. Greedy Raven retains official five-times resource damage
   and pinned per-level resource DPS. The complete post-Greedy-Raven suite
   passed 157 of 157 tests. All 12 Pets have now been audited and remain
   partial; revisit their shared lifecycle and documented child units when
   new combat evidence is available.
2. Revisit Pet spawned subunits and Hero interactions as evidence permits.
3. The Equipment inventory correction is **partial, data-only** after the
   [Revenge Deck audit](../data/reference/th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json).
   Its 27 levels are pinned, but counter damage, healing and tick order are
   unsourced. Portal Pendant remains outside the active roster pending official
   release evidence. The complete suite passed 157 of 157 tests.
4. Audit all 42 confirmed permanent Equipment items by eligible Hero, including
   active and passive effects, sourced per-level values and spawned subunits.
   Barbarian Puppet and its spawned Raged Barbarians are **partial, data-only**
   after the [summon contract audit](../data/reference/th18-2026-09-23-barbarian-puppet-summon-contract-audit.json):
   unit level, spawn geometry, Rage order and Hero recovery timing are open;
   the full suite passed 157 of 157 tests. The [official bonus and spawn-rate recheck](../data/reference/th18-2026-09-23-barbarian-puppet-official-bonus-and-spawn-rate.json)
   confirms all 18 HP/recovery pairs, 20-second Rage and a historical five-unit
   batch every 500 ms. Child total, initial batch, level, placement and tick
   order remain open; the full suite passed 164/164. Rage Vial is also **partial,
   data-only** after its [effect-order audit](../data/reference/th18-2026-09-23-rage-vial-effect-order-audit.json):
   modifier stacking, activation and recovery ordering remain unsourced;
   157/157 tests passed. Earthquake Boots is **partial, data-only** after its
   [ground-target audit](../data/reference/th18-2026-09-23-earthquake-boots-ground-target-audit.json):
   2026 ground-troop damage is sourced, but current area percentages, geometry,
   target exceptions and tick order remain open; 157/157 tests passed.
   The [official radius and damage table recheck](../data/reference/th18-2026-09-23-earthquake-boots-official-radius-and-damage-table.json)
   establishes an eight-tile radius at all levels and matches the pinned
   L9-18 building percentages after the 2025 nerf. L1-8 building damage,
   Wall and troop damage, exact geometry and tick order remain open; the full
   suite passed 164/164 after this additive delta.
   Vampstache is **partial, data-only** after its
   [heal-event audit](../data/reference/th18-2026-09-23-vampstache-heal-event-audit.json):
   current heal amount, hit eligibility, cadence and tick order remain open;
   157/157 tests passed. The [official DPS table recheck](../data/reference/th18-2026-09-23-vampstache-official-dps-table.json)
   confirms all 18 pinned bonuses; healing and attack-speed behavior remain
   open. The complete suite passed 164/164 after the additive delta. Giant
   Gauntlet is **partial, data-only** after its
   [current-effect audit](../data/reference/th18-2026-09-23-giant-gauntlet-current-effect-audit.json):
   the 2026 all-level buff lacks current per-level duration and reduction;
   157/157 tests passed. Spiky Ball is **partial, data-only** after its
   [bounce and count audit](../data/reference/th18-2026-09-23-spiky-ball-bounce-and-count-audit.json):
   all 27 official launch target counts conflict with pinned raw values and
   current projectile values and bounce timing remain open; 157/157 tests
   passed. Snake Bracelet and its spawned Snake are **partial, data-only**
   after the [summon audit](../data/reference/th18-2026-09-23-snake-bracelet-spawn-contract-audit.json):
   the current damage trigger, cap meaning, child behavior and spawn order
   remain open; 157/157 tests passed. Stick Horse is **partial, data-only**
   after its [deployment and Wall audit](../data/reference/th18-2026-09-23-stick-horse-deployment-and-wall-audit.json):
   current speed units, Wall pathing and expiry ordering remain open; 157/157
   tests passed. Archer Puppet and spawned Archers are **partial, data-only**
   after their [summon audit](../data/reference/th18-2026-09-23-archer-puppet-summon-contract-audit.json):
   current child level, spawn phase, invisibility and Queen recovery order
   remain open; 157/157 tests passed. The [official Queen-bonus and invisibility recheck](../data/reference/th18-2026-09-23-archer-puppet-official-bonus-and-invisibility-table.json)
   confirms all 18 DPS/recovery and child invisibility rows plus a historical
   five-unit-per-second batch rate. Child lifecycle and recovery phase remain
   open; the complete suite passed 164/164. Invisibility Vial is **partial,
   data-only** after its [damage and target audit](../data/reference/th18-2026-09-23-invisibility-vial-damage-and-target-audit.json):
   current duration, extra damage staging, target loss and expiry remain open;
   157/157 tests passed. Giant Arrow is **partial, data-only** after its
   [Air Defense and damage audit](../data/reference/th18-2026-09-23-giant-arrow-air-defense-damage-audit.json):
   the 2x Air Defense multiplier and four damage corrections are sourced, but
   14 current damage values and projectile geometry/timing remain open;
   157/157 tests passed. Healer Puppet and its spawned Healers are **partial,
   data-only** after their [summon and healing audit](../data/reference/th18-2026-09-23-healer-puppet-summon-and-healing-audit.json):
   current child continuity, placement, healing targeting and tick order
   remain open; 157/157 tests passed. Frozen Arrow is **partial, data-only**
   after its [slow and impact audit](../data/reference/th18-2026-09-23-frozen-arrow-slow-and-impact-audit.json):
   post-2024 percentage bands match pinned fields, but current effect
   continuity, target eligibility and impact/expiry order remain open;
   157/157 tests passed. Magic Mirror and its Queen clones are **partial,
   data-only** after their [clone and invisibility audit](../data/reference/th18-2026-09-23-magic-mirror-clone-and-invisibility-audit.json):
   Equipment damage-bonus exclusions are official, but current clone values,
   spawn/attack order and Queen invisibility timing remain open; 157/157 tests
   passed. Action Figure and its Giant Giant child are **partial, data-only**
   after their [spawn contract audit](../data/reference/th18-2026-09-23-action-figure-giant-giant-contract-audit.json):
   child stats, event-troop inheritance, spawn and Queen invisibility timing
   remain open; 157/157 tests passed. Monolith Arrow is **partial, data-only**
   after its [housing audit](../data/reference/th18-2026-09-23-monolith-arrow-housing-and-projectile-audit.json):
   official 14/10/5 percent tiers and exclusions are pinned, while trigger,
   damage reduction and projectile order remain open; 157/157 tests passed.
   Henchmen Puppet and its Henchmen child are **partial, data-only** after
   their [child contract audit](../data/reference/th18-2026-09-23-henchmen-puppet-and-child-contract-audit.json):
   current child stats, placement, invisibility and tick order remain open;
   157/157 tests passed. Dark Orb is **partial, data-only** after its
   [projectile and slow audit](../data/reference/th18-2026-09-23-dark-orb-projectile-and-slow-audit.json):
   dated L8 HP 690 corrects pinned raw 600, while collision, slow and tick
   order remain open; 157/157 tests passed. Metal Pants is **partial,
   data-only** after its
   [barrier and recovery audit](../data/reference/th18-2026-09-23-metal-pants-barrier-and-recovery-audit.json):
   current barrier values, damage origins, recovery and tick order remain open;
   157/157 tests passed. Noble Iron is **partial, data-only** after its
   [opening-shot audit](../data/reference/th18-2026-09-23-noble-iron-opening-shots-audit.json):
   current values, charge consumption, range and projectile order remain
   open; 157/157 tests passed. Dark Crown is **partial, data-only** after its
   [defeat-threshold audit](../data/reference/th18-2026-09-23-dark-crown-defeat-threshold-audit.json):
   official three boosts and historical 60/120/180 housing thresholds are
   recorded, while eligibility, HP growth and death order remain open;
   157/157 tests passed. Meteor Staff is **partial, data-only** after its
   [target/cadence audit](../data/reference/th18-2026-09-23-meteor-staff-target-and-cadence-audit.json):
   current damage, cadence, targeting and impact order remain open;
   157/157 tests passed. Eternal Tome is **partial, data-only** after its
   [aura-immunity audit](../data/reference/th18-2026-09-23-eternal-tome-aura-immunity-audit.json):
   aura geometry, eligibility and tick order remain open; 157/157 tests
   passed. Life Gem is **partial, data-only** after its
   [balance/aura audit](../data/reference/th18-2026-09-23-life-gem-balance-and-aura-audit.json):
   current values, geometry and HP conversion remain open; 157/157 tests
   passed. Rage Gem is **partial, data-only** after its
   [aura/stacking audit](../data/reference/th18-2026-09-23-rage-gem-aura-and-stacking-audit.json):
   current values, geometry, stacking and tick order remain open; 157/157
   tests passed. Healing Tome is **partial, data-only** after its
   [aura/pulse audit](../data/reference/th18-2026-09-23-healing-tome-aura-and-pulse-audit.json):
   current healing, radius, pulse and tick order remain open; 157/157 tests
   passed. The [official duration table recheck](../data/reference/th18-2026-09-23-healing-tome-official-duration-table.json)
   matches all 18 retained post-2024 duration values. Heal rate, aura geometry,
   pulse arithmetic and tick order remain open; the full suite passed 164/164.
   The [level provenance delta](../data/reference/th18-2026-09-23-healing-tome-primary-level-provenance.json)
   now carries those 18 official duration values into immutable GameData records
   shared by Headless, Viewer and RL. The 164/164 suite and RL runtime passed;
   Equipment combat, current-client continuity and effect overlays remain partial.
   The [six-Equipment level provenance delta](../data/reference/th18-2026-09-23-six-equipment-primary-level-provenance.json)
   carries 117 official per-level records for Royal Gem, Earthquake Boots,
   Vampstache, Barbarian Puppet, Archer Puppet and Electro Boots through
   immutable Core to Headless, Viewer and RL. The 164/164 suite and RL runtime
   passed. Their combat effects, interactions and current-client continuity
   remain partial; continue with independent sourced behavior contracts.
   Fireball is **partial, data-only** after its
   [target/splash audit](../data/reference/th18-2026-09-23-fireball-target-size-and-splash-audit.json):
   target geometry, impact timing and splash rules remain open; 157/157
   tests passed. Lavaloon Puppet and spawned Lavaloon are **partial, data-only**
   after their [child contract audit](../data/reference/th18-2026-09-23-lavaloon-puppet-and-child-contract-audit.json):
   L21-27 three-unit count is official; child combat and Pup relationship
   remain open; 157/157 tests passed. Heroic Torch is **partial, data-only**
   after its [balance and Wall audit](../data/reference/th18-2026-09-23-heroic-torch-balance-and-wall-audit.json):
   January 2026 changed values without a level table; aura, Wall and tick
   behavior remain open; 157/157 tests passed. Royal Gem is **partial,
   data-only** after its [recovery audit](../data/reference/th18-2026-09-23-royal-gem-recovery-and-bonus-audit.json):
   dated sources confirm HP/DPS but active heal continuity and tick order
   remain open; 157/157 tests passed.
   The [official localized L1-18 bonus table](../data/reference/th18-2026-09-23-royal-gem-official-level-bonus-table.json)
   now directly corroborates every retained Royal Gem HP/DPS pair; the
   164/164 suite passed. Deployment, two-Equipment stacking and active-heal
   ordering remain unsourced, so no combat modifier is enabled.
   The [active-heal source recheck](../data/reference/th18-2026-09-23-royal-gem-active-heal-source-recheck.json)
   found blank heal columns in an accessible current reference, an upstream
   mirror without heal values, a divergent unversioned historical table and
   only an uninspectable search snippet matching the frozen heal series.
   Current heal amounts, cap and event order remain unsourced; the full
   164/164 suite passed and no Core heal was enabled. Continue with the next
   independent Equipment contract.
   Seeking Shield is **partial,
   data-only** after its [target and bounce audit](../data/reference/th18-2026-09-23-seeking-shield-target-and-bounce-audit.json):
   current damage, footprint target metric, bounce and impact order remain
   open; 157/157 tests passed. Hog Rider Puppet and spawned Hog Riders are
   **partial, data-only** after their [child audit](../data/reference/th18-2026-09-23-hog-rider-puppet-and-child-audit.json):
   January 2026 L1-7 buffs lack exact current values; spawn, invisibility and
   child combat order remain open; 157/157 tests passed. Haste Vial is
   **partial, data-only** after its [cadence/speed audit](../data/reference/th18-2026-09-23-haste-vial-cadence-and-speed-audit.json):
   official passive L9-18 values match raw, but fixed-tick speed and cadence
   conversion remain open; 157/157 tests passed. Rocket Spear is **partial,
   data-only** after its [charged shots audit](../data/reference/th18-2026-09-23-rocket-spear-charged-shots-audit.json):
   pinned shots and damage lack a current projectile, splash and charge order
   contract; 157/157 tests passed. Electro Boots is **partial, data-only**
   after its [aura/balance audit](../data/reference/th18-2026-09-23-electro-boots-aura-and-balance-audit.json):
   official nerf lacks exact current values; aura geometry, fractional damage,
   healing and pulse order remain open; 157/157 tests passed. The [official aura and healing table recheck](../data/reference/th18-2026-09-23-electro-boots-official-aura-and-healing-table.json)
   confirms all 27 post-2025 aura DPS and self-heal rates. Current continuity,
   integer pulse arithmetic, target geometry and tick order remain open; the
   complete suite passed 164/164. Frost Flake is
   **partial, data-only** after its [freeze and target audit](../data/reference/th18-2026-09-23-frost-flake-freeze-and-target-audit.json):
   current projectile values, target fallback, impact and freeze ordering
   remain open; 157/157 tests passed. Fire Heart is **partial, data-only**
   after its [balance/death burst audit](../data/reference/th18-2026-09-23-fire-heart-balance-and-death-burst-audit.json):
   official May 2026 DPS replacements and L15/L18 regeneration are pinned;
   L16/L17 regeneration, burst geometry and tick order remain open; 157/157
   tests passed. Flame Blower is **partial, data-only** after its
   [direction/activation audit](../data/reference/th18-2026-09-23-flame-blower-direction-and-activation-audit.json):
   official order and facing direction are pinned, while damage anomaly,
   geometry and hit timing remain open; 157/157 tests passed. Stun Blaster is
   **partial, data-only** after its [shockwave/target audit](../data/reference/th18-2026-09-23-stun-blaster-shockwave-and-target-audit.json):
   damage pulse count, target exclusions, stun and recovery order remain open;
   157/157 tests passed. Electro Fangs is **partial, data-only** after its
   [chain/target audit](../data/reference/th18-2026-09-23-electro-fangs-chain-and-target-audit.json):
   official four-target chain and 20% decay are sourced; per-level damage is
   community-only, and link range, selection and rounding remain open;
   157/157 tests passed. Rocket Backpack is **partial, data-only** after its
   [dash/balance audit](../data/reference/th18-2026-09-23-rocket-backpack-dash-and-balance-audit.json):
   official L21/L27 penetrating damage and August dash/Phoenix fixes are
   pinned, while intermediate damage, path and lifecycle remain open;
   157/157 tests passed. All 42 Equipment items have item-level audits.
   Continue through the roster
   when an individual contract remains incomplete.
5. Revisit the six Hero gaps and verify all Hero, Pet and Equipment interactions
   through Core, Headless, Viewer, RL, snapshots, replays and fixed-tick hashes.
   The shared read-only Core catalogue now lists all 12 Pets and 42 Equipment
   items in Headless, Viewer and RL. It passed the complete 158/158 suite and
   separate RL runtime validation. Combat attachment, effects and spawned
   actors remain unimplemented; the matrix keeps those axes partial.
   The [shared attachment audit](../data/reference/th18-2026-09-23-hero-pet-equipment-attachment-contract-audit.json)
   pins one Pet and two Equipment slots per Hero, with Active/Passive types
   exposed from Core through all three adapters. Activation and Pet lifecycle ordering remain
   partial; the complete suite passed 158/158 and RL runtime validation passed.
   A later [Electro Fangs Passive override](../data/reference/th18-2026-09-23-electro-fangs-passive-type-override.json)
   carries Supercell's explicit classification into the normalized catalogue
   without changing its frozen levels. At that increment Monolith Arrow was
   the one remaining unverified activation type. The complete suite passed 158/158 again, and
   RL runtime validation passed; combat chains remain partial.
   Monolith Arrow now has a read-only deployed-housing and tier projection in
   Core, Headless, Viewer and RL. It counts deployed Heroes as 25 each and
   excludes spawned units, while leaving activation and damage unimplemented.
   Snapshot, replay and hash checks plus the complete 159/159 suite and RL
   runtime validation passed. Its combat axis remains partial.
   Core now projects the official 14/10/5 maximum-HP percentages from those
   tiers through Headless, Viewer and RL. Fixed-tick boundaries, snapshot,
   replay, hashes, the full 164/164 suite and RL runtime passed; projectile
   application, rounding and effect timing remain partial.
   The [dated Passive classification override](../data/reference/th18-2026-09-23-monolith-arrow-passive-type-override.json)
   now carries Clash Ninja's explicit June 2026 label into the immutable
   catalogue. Headless, Viewer and RL read the same Core field; the 164/164
   suite and RL runtime passed. The primary release note does not label the
   type, and per-attack projectile, damage reduction, timing and stacking
   remain partial.
   Validated `hero_loadouts` now encode one Pet and up to two Hero-specific
   Equipment choices per selected attacking Hero in scenario/replay values.
   Core checks source-backed IDs, levels and slots; Headless, Viewer and RL
   expose the same selection. Duplicate assignments are rejected while their
   current game behavior remains unsourced. Snapshot and replay checks, the
   complete 160/160 suite and RL runtime validation passed. Combat effects,
   activation controls and battlefield Pet actors remain partial.
   The Viewer now provides Pet and eligible Equipment pickers with catalogue
   levels for its selected Heroes. Applying a choice starts a fresh Core bridge
   at T+0; an invalid choice leaves the running battle intact. Its default air
   practice army includes Minion Prince, Grand Warden (air) and Dragon Duke.
   The ImGui render smoke, bridge restart checks and full 161/161 suite passed.
   Snapshot restoration still requires the exact loadout, while equal combat
   states with different currently inert selections now hash equally. The
   complete 161/161 suite and separate RL runtime validation passed after
   this hash correction.
   A real ImGui mouse press/release now verifies the Apply control fires once;
   the full 161/161 suite passed again. Pet and Equipment picker clicks and
   battlefield visuals still need direct verification; combat effects and
   activation controls remain partial.
   Core now retains the exact normalized content and all 1,073 pinned level
   rows with their provenance for the 12 Pets and 42 Equipment items. Headless
   `hero-support-level`, the Viewer selected-level record, and RL
   `hero_support_level` expose those read-only values. All Core levels were
   checked for a provenance record; four representative Headless and RL rows
   were compared with the normalized source. The full 161/161 suite and RL
   runtime validation passed. Pet lifecycle and Equipment effect contracts
   remain partial, so no combat actor was enabled.
   A rendered ImGui popup test now uses mouse input to pick L.A.S.S.I. and an
   eligible Queen Equipment item, then validates the draft in Core. The Viewer
   replay bridge loads a selected Pet, publishes it at T+0 and matches direct
   Core after two fixed ticks. The full 162/162 suite passed. Other item
   battlefield visuals and combat effects remain partial.
   All 12 Pet and 42 Equipment selections now render in ImGui at their highest
   catalogued level and are accepted by Core; the full 162/162 suite passed.
   Mouse clicks for all 54 picker entries now pass the full 162/162 suite;
   battlefield Pet art and Equipment effect overlays remain open.
6. Run the final item-by-item goal audit and retain every unsourced axis as
   explicit partial coverage.

Temporary event content, Builder Base, Clan Capital, siege machines, Clan
Castle reinforcements and unrelated spell-roster completion are outside this
goal. The wider TH18 backlog below is deferred while this goal is active.

## Immediate order

1. Resume remaining ordinary troops and required spawned units.
2. Resume remaining defensive buildings and permanent variants.
3. Resume remaining resource, army, and other targetable buildings.
4. Verify walls, traps, geometry, assets, snapshots, replays, and adapters.
5. Run a final item-by-item TH18 audit.

## Hero phase boundary

The Barbarian King, Archer Queen, Grand Warden, Royal Champion, Minion Prince
and Dragon Duke have their applicable TH18 base attack slices materialized by
the shared Core and verified through Headless, Viewer, RL, snapshots and
replays. Royal Champion wall crossing and Dragon Duke Royal Rampage are the
source-supported non-equipment special behaviors integrated in this phase.
The Hero evidence audit closed partial on 2026-09-23 after the
[final source recheck](../data/reference/th18-2026-09-23-hero-defense-evidence-closure.json).
Meteor Golem and Meteormite were marked partial after the
[2026-09-23 evidence recheck](../data/reference/th18-2026-09-23-meteor-golem-meteormite-evidence-recheck.json).
Ruin Witch and Ruin Knight are the active ordinary-troop group in
`state/current.md`.

Hero Equipment and pets were excluded from the earlier Hero-only phase; they
are included in the active persistent goal above. Official Hero Banner documentation
confirms patrol near the banner but does not define a deterministic defense
contract. Each Hero has a source-verified icon, while battlefield sprites,
anchors and animations remain unverified. The dated
[battlefield art rights audit](../data/reference/th18-2026-09-23-hero-battlefield-art-rights-audit.json)
compares Supercell's policy with the pinned upstream license and README; those
checked documents establish no compatible redistribution permission for
game-derived Hero sprites. All six Hero art axes remain partial, with no
new sprite packaged. The full 162/162 suite passed. Both defense and art gaps
remain partial work, not completed integration.

The Viewer prebattle editor now selects each of the six sourced Hero Banner
assignments by mouse and applies the chosen value through the existing Core
scenario. Focused checks cover snapshot binding, replay serialization and
identical combat hashes across image-only Banner variants. Selected Banner
replay playback through the Viewer bridge matches direct Core after two fixed
ticks. All six pinned local Banner icons decode and render distinct pixels
through the actual Viewer scene path. The full 164/164 suite passed;
defensive patrol and combat remain partial.

The Python RL adapter now preserves the same value-serialized Grand Warden
ground/air ArmySlot mode and all six Hero Banner variants as Headless and
Viewer. Its observation reports the Core flying state and selected Banner
variant. RL runtime checks both modes, distinct combat hashes and all six
visual-only Banner values; the complete 164/164 suite passed. Patrol,
collision, battlefield sprites and Equipment/Pet effects remain partial.

The Viewer prebattle editor now selects Grand Warden ground or air mode by
mouse, retaining the choice in the Core scenario. Saved replays restore each
mode; Viewer playback matches direct Core after two fixed ticks, presents the
Core flying flag and yields distinct future-state hashes. The full 164/164
suite passed. In-game mode transition timing remains unsourced.

All 12 Pet and 42 Equipment maximum-level records now pass direct Headless
and RL lookups. Headless tests group all 54 choices into 22 legal one-Hero
loadouts, each simulated for two fixed ticks, saved as a value replay and
replayed with identical output and state hash. RL verifies each choice reaches
Core and remains observable after deployment. The complete 164/164 suite and
separate RL runtime passed. These are selection paths; Pet actors, Equipment
effects and their future-state interactions remain partial.

The official Tournament Mode Hero defense bonus is documented separately and
must not be applied to standard Home Village battles.

## Clearly missing Core content

The next audit-driven groups include Ruin Knight, Ruin Witch, Air Sweeper, Spell
Tower, Builder's Hut weapon behaviour, Firespitter, Ricochet Cannon, Super
Wizard Tower, Revenge Tower, and Crafting Station behaviour.

Some existing entities are materializable but still lack verified special
behaviour, assets, geometry, or source values. They remain work items even
though they can already appear in scenarios.

The authoritative exhaustive list is generated by:

```powershell
python tools/audit_th18_conformity.py
```
