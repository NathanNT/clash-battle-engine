"""Shared repository discovery for validation tools.

Validators must follow modules as they are extracted from the former
``core.cpp``. Keeping this logic here prevents architectural cleanup from
silently weakening an audit or forcing every validator to know file names.
"""
from __future__ import annotations

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def core_source_text() -> str:
    paths = sorted((ROOT / "src" / "core").rglob("*.cpp"))
    paths += sorted((ROOT / "src" / "core").rglob("*.hpp"))
    paths += sorted((ROOT / "include" / "cocsim").rglob("*.hpp"))
    return "\n".join(path.read_text(encoding="utf-8") for path in paths)


def viewer_source_text() -> str:
    """Return every Viewer implementation and header after modularization.

    Conformity checks inspect Viewer behaviour (not merely its entry point), so
    they must follow helpers extracted from ``main.cpp`` just as Core checks
    follow its domain modules.
    """
    paths = sorted((ROOT / "src" / "viewer").rglob("*.cpp"))
    paths += sorted((ROOT / "src" / "viewer").rglob("*.hpp"))
    return "\n".join(path.read_text(encoding="utf-8") for path in paths)


def _registered_names(macro: str, relative: str) -> list[str]:
    definitions = (ROOT / relative).read_text(encoding="utf-8")
    values = re.findall(rf'{macro}\([^,]+,\s*"([a-z0-9_]+)"\)', definitions)
    if not values:
        raise RuntimeError(f"cannot locate identifiers in {relative}")
    return values


def registered_kind_ids() -> list[str]:
    return _registered_names("COCSIM_KIND", "include/cocsim/detail/content_ids.inc")


def registered_spell_ids() -> list[str]:
    return _registered_names("COCSIM_SPELL", "include/cocsim/detail/spell_ids.inc")
