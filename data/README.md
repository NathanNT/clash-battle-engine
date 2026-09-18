# Data cache

This directory contains a versioned TH18 evidence ledger in
`reference/th18-2026-09-17.json`. It records official deltas, exact source URLs,
publication/consultation dates and field-level catalogue overrides. It is
append-only: a new reference date requires a new ledger, never an in-place
rewrite of historical raw payloads.

Populate the secondary structured snapshot via
`python tools/import_data.py --commit <40-char SHA>`. The importer records raw
payloads under `data/raw/<SHA>/`, their SHA-256 hashes, and a normalized catalogue.
No branch name is accepted, so an update is explicit and diffable. The upstream
license and game version must be reviewed and entered by the operator. They are not
inferred from a README. Every normalized level receives its pinned URL, commit and
payload SHA-256. A ledger override adds its field names to that level's provenance.
the source and derivation remain in the ledger.

## Catalogue representation

Every materialized `Stats` record is keyed by `(kind, level)`. The Core reads
that level's hitpoints, damage per impact, cooldown, range, footprint, target
data, and documented special parameters. `EntityView.level` exposes the chosen
level unchanged to the Viewer and headless callers.

The normalized schema retains the available progression fields: build or
research cost and resource, time, Town Hall or Laboratory requirements, XP,
resource capacity and production, building/trap/storage limits, wall rings,
images, and Town Hall weapon records. `Stats::attributes` exposes scalar
fields. The complete normalized source record remains in
`data/catalogue.normalized.json`.

## Viewer assets

`python tools/fetch_viewer_assets.py` downloads each normal-level sprite
referenced by the normalized catalogue from the same pinned upstream commit.
Hashes and local provenance are retained under `assets/`. The assets are local
Viewer material: review upstream licensing and the original rights holder's
fan-content policy before redistribution.
