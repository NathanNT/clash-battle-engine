#!/usr/bin/env python3
"""Ensure area-damage semantics follow the pinned catalogue field.

`damage_type` is the source classification.  The core must not grow an
independent Kind table that can drift when imported content changes.
"""
from __future__ import annotations

import json
import pathlib

from project_layout import core_source_text


ROOT = pathlib.Path(__file__).resolve().parents[1]
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"


def main() -> None:
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    core = core_source_text()
    expected = [
        content["id"] for content in catalogue["contents"]
        if content.get("damage_type") == "splash"
    ]
    assert expected, "catalogue contains no area-damage content"
    assert 'const bool splash=string_field(content,"damage_type").value_or("")=="splash";' in core, (
        "core area-damage classification no longer follows catalogue damage_type"
    )
    print(f"Damage classification validation passed ({len(expected)} catalogue area-damage entries)")


if __name__ == "__main__":
    main()
