#!/usr/bin/env python3
"""Validate non-enabling evidence records for unresolved Super Troop mechanics."""
from __future__ import annotations

import json
import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[1]
REFERENCE = ROOT / "data" / "reference"
DATE = re.compile(r"^\d{4}-\d{2}-\d{2}$")
AUDITS = {
    "super-valkyrie-mechanics-audit-2026-09-18.json": "super_valkyrie",
    "super-hog-rider-mechanics-audit-2026-09-18.json": "super_hog_rider",
}


def main() -> None:
    for filename, content_id in AUDITS.items():
        audit = json.loads((REFERENCE / filename).read_text(encoding="utf-8"))
        assert audit["schema_version"] == 1
        assert audit["content_id"] == content_id
        assert audit["audit_id"] == f"home-village-{content_id.replace('_', '-')}-mechanics-2026-09-18"
        assert DATE.fullmatch(audit["consulted_on"])
        assert audit["established"] and audit["unresolved"]
        assert audit["decision"].startswith(f"Do not enable {content_id}")
        ids: set[str] = set()
        for source in audit["sources"]:
            assert source["id"] not in ids
            ids.add(source["id"])
            assert source["tier"] in {"primary", "secondary-dated-build", "secondary-version-unknown"}
            assert source["title"] and source["url"].startswith("https://")
            assert source["game_version"] and source["finding"]
            assert source["published_on"] is None or DATE.fullmatch(source["published_on"])
    print(f"Super Troop mechanics audits passed ({len(AUDITS)} non-enabling records; active partial baselines are covered by the active-reference validator)")


if __name__ == "__main__":
    main()
