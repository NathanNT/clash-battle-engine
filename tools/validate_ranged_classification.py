#!/usr/bin/env python3
"""Keep catalogue long-range combatants on the core projectile path."""
from __future__ import annotations

import json
import pathlib

from project_layout import core_source_text


ROOT = pathlib.Path(__file__).resolve().parents[1]
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"


def main() -> None:
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    core = core_source_text()
    ranged_line = next(line for line in core.splitlines() if "const bool level_ranged=" in line)
    assert ranged_line.strip() == "const bool level_ranged=level_range>1.0&&!heals;", ranged_line
    expected = {
        content["id"]
        for content in catalogue["contents"]
        if content["id"] != "healer"
        and (content.get("range_tiles") or 0.0) > 1.0
    }
    assert expected, "catalogue contains no long-range combatant"
    print(f"Ranged classification validation passed ({len(expected)} catalogue entries)")


if __name__ == "__main__":
    main()
