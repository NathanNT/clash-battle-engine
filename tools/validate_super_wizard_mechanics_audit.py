#!/usr/bin/env python3
"""Keep the Super Wizard evidence stop condition explicit and non-enabling."""
from __future__ import annotations

import json
import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[1]
AUDIT = ROOT / "data" / "reference" / "super-wizard-mechanics-audit-2026-09-18.json"
IDS = ROOT / "include" / "cocsim" / "detail" / "content_ids.inc"
DATE = re.compile(r"^\d{4}-\d{2}-\d{2}$")


def main() -> None:
    audit = json.loads(AUDIT.read_text(encoding="utf-8"))
    assert audit["schema_version"] == 1
    assert audit["audit_id"] == "home-village-super-wizard-mechanics-2026-09-18"
    assert audit["content_id"] == "super_wizard"
    assert DATE.fullmatch(audit["consulted_on"])
    sources = audit["sources"]
    assert {source["tier"] for source in sources} == {"primary", "secondary"}
    assert len({source["id"] for source in sources}) == len(sources)
    for source in sources:
        assert source["title"] and source["url"].startswith("https://")
        assert DATE.fullmatch(source["published_on"])
        assert source["game_version"] and source["finding"]
    assert any("40%" in finding for finding in audit["established"])
    assert len(audit["unresolved"]) >= 4
    assert audit["decision"].startswith("Enabled as core_supported_secondary_partial")
    assert 'COCSIM_KIND(SuperWizard, "super_wizard")' in IDS.read_text(encoding="utf-8")
    print("Super Wizard mechanics audit validation passed: Chain Magic is enabled with explicit approximations")


if __name__ == "__main__":
    main()
