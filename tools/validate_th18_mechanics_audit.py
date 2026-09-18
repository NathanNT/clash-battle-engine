#!/usr/bin/env python3
"""Validate post-freeze evidence audits without mutating the frozen ledger."""
from __future__ import annotations

import hashlib
import json
import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[1]
REFERENCE = ROOT / "data" / "reference" / "th18-2026-09-17.json"
AUDIT = ROOT / "data" / "reference" / "th18-mechanics-audit-2026-09-18.json"
DATE = re.compile(r"^\d{4}-\d{2}-\d{2}$")
SHA256 = re.compile(r"^[0-9a-f]{64}$")


def main() -> None:
    reference = json.loads(REFERENCE.read_text(encoding="utf-8"))
    audit = json.loads(AUDIT.read_text(encoding="utf-8"))
    assert audit["schema_version"] == 1
    assert audit["audit_id"] == "home-village-th18-mechanics-audit-2026-09-18"
    assert audit["reference_id"] == reference["reference_id"]
    assert audit["target_reference"] == {
        "file": REFERENCE.name,
        "sha256": hashlib.sha256(REFERENCE.read_bytes()).hexdigest(),
        "action": "evidence_only_no_historical_overwrite",
    }
    assert SHA256.fullmatch(audit["target_reference"]["sha256"])
    assert "does not authorize" in audit["conclusion"]

    sources = audit["sources"]
    assert len(sources) == 3 and len({source["id"] for source in sources}) == len(sources)
    for source in sources:
        assert source["tier"] in {"primary", "secondary-dated-build", "secondary-version-unknown"}
        assert source["title"] and source["url"].startswith("https://")
        assert DATE.fullmatch(source["consulted_on"])
        assert source["game_version"]
        assert source["finding"]
        assert source["published_on"] is None or DATE.fullmatch(source["published_on"])
    dated = next(source for source in sources if source["id"] == "goblinsfarm-2026-08-26-air-sweeper")
    assert dated["tier"] == "secondary-dated-build" and dated["game_version"] == "18.400.21"
    unversioned = next(source for source in sources if source["id"] == "fandom-2026-09-18-air-sweeper")
    assert unversioned["tier"] == "secondary-version-unknown" and unversioned["published_on"] is None

    findings = audit["findings"]
    assert len(findings) == 1
    finding = findings[0]
    assert finding["content_id"] == "air_sweeper"
    assert finding["status"] == "implementation_withheld"
    assert finding["corroborated_secondary_values"]["push_strength_tiles_by_level"] == [1.6, 2.0, 2.4, 2.8, 3.2, 3.6, 4.0]
    assert finding["corroborated_secondary_values"]["attack_cadence_seconds"] == 5
    assert "No new value is imported" in finding["not_imported"]
    assert len(finding["blocking_unknowns"]) >= 5
    print("TH18 mechanics audit validation passed: Air Sweeper evidence remains non-importing")


if __name__ == "__main__":
    main()
