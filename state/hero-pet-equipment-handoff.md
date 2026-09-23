# Hero, Pet and Equipment handoff — 2026-09-23

This is a status handoff for the persistent Home Village TH18 goal. It adds no
gameplay data or combat rule. The frozen 2026-09-17 reference remains unchanged;
the [per-item matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json)
and dated files in [`data/reference/`](../data/reference/) are the detailed
evidence ledger. Preserve all existing workspace changes when resuming.

## Verified work

- All six permanent Heroes have their included TH18 levels and sourced base
  attacks through shared fixed-10-ms Core paths. Grand Warden ground/air mode
  and six Hero Banner assignments are selectable in the Viewer and carried by
  scenarios and replays. Defensive Hero combat remains partial.
- The inventory contains 12 permanent Pets and 42 permanent Equipment items.
  Immutable Core `GameData` retains 1,073 level records with provenance.
  Headless, Viewer and RL can read those records and select legal Hero loadouts;
  a selected loadout survives scenario and replay serialization. These
  selections currently have no Pet or Equipment combat effect.
- The Viewer prebattle editor, Pet/Equipment picker clicks, all six Banner
  icon renders, Grand Warden mode selection, and selected-loadout replay
  playback have automated checks. Battlefield Pet and Equipment effect visuals
  are still open.
- Monolith Arrow has a sourced read-only deployed-housing and 14/10/5 percent
  maximum-HP band projection through Core, Headless, Viewer and RL. Its
  projectile and damage application are not implemented. Its Passive label has
  dated secondary provenance, not primary or current-build confirmation.
- The last complete `.\tools\dev.ps1 test` run passed **164/164** tests. The
  separately rebuilt Python binding passed `validate_rl_runtime.py`, including
  1,042 GameData scenarios. These results prove the tested paths, not complete
  Hero, Pet or Equipment fidelity.

## Work still required

1. Implement source-supported Hero Banner defender patrol, acquisition,
   pursuit, return, respawn, collision and tick ordering. Resolve Hero-specific
   hitboxes, path clearance, anchors and battlefield scale.
2. Materialize each included Pet as a Hero-linked Core actor with sourced
   deployment, follow, targeting, attack/heal, death and interaction rules.
3. Apply permanent Equipment bonuses and active/passive effects through Core,
   including activation commands, targets, geometry, durations, stacking,
   damage/healing arithmetic and same-tick order where evidence supports them.
4. Materialize the 11 tracked spawned-unit types from Pets and Equipment with
   sourced level mapping, count, placement, lifecycle and targeting. The matrix
   names each parent and child.
5. Carry every future-influencing selection and new logical timer/counter in
   value snapshots, replays and deterministic hashes. Exercise the same Core
   behavior through Headless, Viewer and RL, including Viewer controls,
   battlefield rendering, overlays and replay playback.
6. Establish redistribution rights for battlefield Hero/Pet art or use
   compatible assets. The current Banner icons do not resolve sprite rights.
7. Add focused behavior, interaction, serialization, replay, determinism and
   regression tests for each supported increment; run the full suite and update
   `state/current.md`, `state/backlog.md`, `state/coverage-summary.md` and the
   per-item matrix after each tested increment.

## Missing evidence and current knowledge limits

The existing audits often establish an item's identity, level values and broad
effect, but do not establish the complete deterministic contract. Recurrent
missing fields are current-client values, Pet/Hero deployment coordinates,
target eligibility and priority, projectile or heal launch/impact timing,
collision geometry, integer rounding, effect stacking, expiry and death order,
and interactions between both Equipment slots, Pets, Heroes and spawned units.
Royal Gem's current per-level active heal is specifically unresolved. Hero
Banner defense and Hero collision geometry remain unresolved. No missing value
or ordering rule should be inferred from a generic troop implementation.

The proposed new data source has not yet been provided or assessed. On resume,
check its URL or origin, title, publisher, publication and consultation dates,
game version/build, redistribution terms, content IDs, level/mode coverage,
raw hash, and whether it actually defines the missing combat phases. Add any
new facts only in a dated hash-pinned additive delta, record derivations and
rejected inferences, and retain every unresolved coverage axis as partial.

## GUI commands (PowerShell, from the repository root)

```powershell
.\tools\dev.ps1 build
.\tools\dev.ps1 test
.\tools\dev.ps1 viewer
```

`build` compiles the Viewer and configured tests. `test` builds and runs the
complete suite, including Viewer tests. `viewer` builds and launches the GUI.
