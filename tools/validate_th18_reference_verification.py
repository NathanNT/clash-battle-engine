#!/usr/bin/env python3
"""Protect the frozen TH18 ledger from silent modification by later audits."""
from __future__ import annotations

import hashlib
import json
import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[1]
REFERENCE = ROOT / "data" / "reference" / "th18-2026-09-17.json"
VERIFICATION = ROOT / "data" / "reference" / "th18-reference-verification-2026-09-18.json"
DATE = re.compile(r"^\d{4}-\d{2}-\d{2}$")
SHA256 = re.compile(r"^[0-9a-f]{64}$")


def main() -> None:
    reference = json.loads(REFERENCE.read_text(encoding="utf-8"))
    verification = json.loads(VERIFICATION.read_text(encoding="utf-8"))
    target = verification["target_reference"]

    assert verification["schema_version"] == 1
    assert verification["verification_id"] == "home-village-th18-reference-verification-2026-09-18"
    assert verification["verified_at"] == "2026-09-18T00:00:00+02:00"
    assert target == {
        "file": REFERENCE.name,
        "reference_id": reference["reference_id"],
        "frozen_at": reference["frozen_at"],
        "sha256": hashlib.sha256(REFERENCE.read_bytes()).hexdigest(),
        "action": "retained_unchanged",
    }
    assert "Permanent Home Village" in verification["scope"]
    assert "temporary event content" in verification["scope"]
    assert "No post-freeze permanent in-scope delta" in verification["conclusion"]
    assert "does not overwrite" in verification["conclusion"]
    assert isinstance(verification["remaining_uncertainty"], str) and verification["remaining_uncertainty"]

    sources = verification["sources"]
    assert sources and len({source["id"] for source in sources}) == len(sources)
    for source in sources:
        assert source["tier"] in {"primary", "primary-version-uncertain"}, source["id"]
        assert source["publisher"] == "Supercell", source["id"]
        assert isinstance(source["title"], str) and source["title"], source["id"]
        assert isinstance(source["url"], str) and source["url"].startswith("https://"), source["id"]
        assert DATE.fullmatch(source["consulted_on"]), source["id"]
        assert source["game_version"] == "not published", source["id"]
        assert isinstance(source["finding"], str) and source["finding"], source["id"]
        published_on = source.get("published_on")
        assert published_on is None or DATE.fullmatch(published_on), source["id"]

    crafted = next(source for source in sources
                   if source["id"] == "supercell-support-2026-09-18-crafted-defenses")
    assert crafted["tier"] == "primary-version-uncertain"
    assert crafted["published_on"] is None
    assert "not used to change" in crafted["finding"]
    assert SHA256.fullmatch(target["sha256"])
    print(f"TH18 reference verification passed: frozen {target['file']} sha256 is pinned")


if __name__ == "__main__":
    main()
