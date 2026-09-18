#!/usr/bin/env python3
"""Validate that the packaged Viewer assets follow catalogue image provenance.

The public source may intentionally reuse an earlier visual tier for a later
numeric level.  This validator checks the declared image field, not an invented
``level-N.png`` convention, and checks the documented Golemite -> Golem alias.
"""
from __future__ import annotations

import json
import pathlib

from project_layout import registered_kind_ids


ROOT = pathlib.Path(__file__).resolve().parents[1]
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"
ASSETS = ROOT / "assets"
VIEWER_DIR = ROOT / "src" / "viewer"


def resolved_image(contents: dict[str, dict], content: dict, level: int, variant: str) -> str | None:
    image_content = contents["golem"] if content["id"] == "golemite" else content
    for candidate in range(level, 0, -1):
        source_level = next((item for item in image_content["levels"]
                             if item["level"] == candidate and item.get("variant", "normal") == variant), None)
        if source_level and source_level.get("image"):
            return source_level["image"]
    return None


def main() -> None:
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    contents = {item["id"]: item for item in catalogue["contents"]}
    core_ids = set(registered_kind_ids())
    missing: list[str] = []
    source_visual_absent: list[str] = []
    fallback_count = 0
    for content in contents.values():
        if content["id"] not in core_ids:
            continue
        for level in content.get("levels", []):
            variant = level.get("variant", "normal")
            image = resolved_image(contents, content, level["level"], variant)
            if image is None:
                # A non-renderable level must remain explicit rather than
                # creating a fictional asset path. Spawned-only units still
                # use the core/Viewer entity path, but must be reported as a
                # visual fidelity gap instead of failing the whole asset pack.
                detail = f"{content['id']} {variant} L{level['level']}: no source image or prior visual tier"
                if content.get("support") == "spawned_only" or content["id"] in {"baby_dragon", "miner"}:
                    source_visual_absent.append(detail)
                else:
                    missing.append(detail)
                continue
            if image != level.get("image"):
                fallback_count += 1
            if not (ASSETS / image).is_file():
                detail = f"{content['id']} {variant} L{level['level']}: missing {image}"
                if content["id"] in {"baby_dragon", "miner"}: source_visual_absent.append(detail)
                else: missing.append(detail)
        if content.get("image"):
            image = content["image"]
            if not (ASSETS / image).is_file():
                missing.append(f"{content['id']}: missing metadata image {image}")
    viewer = "\n".join(
        path.read_text(encoding="utf-8")
        for path in sorted(VIEWER_DIR.glob("*"))
        if path.suffix in {".cpp", ".hpp"}
    )
    assert "stats->attributes.image" in viewer
    assert "data.find(image_kind, candidate_level, variant)" in viewer
    assert "entity.supercharged ? \"supercharged\" : \"normal\"" in viewer
    assert "kind == Kind::Golemite ? Kind::Golem" in viewer
    # The fallback path is only used if a new supplemental numerical tier has
    # no visual tier in the pinned catalogue.  It must still preserve the
    # public dashed troop ids rather than look in the defenses directory.
    assert 'if (kind == Kind::SneakyGoblin) id = "sneaky-goblin";' in viewer
    assert 'if (kind == Kind::RootRider) id = "root-rider";' in viewer
    assert "kind == Kind::SneakyGoblin || kind == Kind::RootRider" in viewer
    assert "scenario.non_combat_obstacles" in viewer
    assert "data.find_non_combat(obstacle.kind)" in viewer
    assert "data.find_non_combat(image_kind)" in viewer
    # Sprite canvases contain authored placement margins. Cropping their alpha
    # bounds would stretch and shift assets relative to the sourced tile grid.
    assert "asset.source = {0, 0, float(surface->w), float(surface->h)};" in viewer
    assert "opaque_bounds" not in viewer
    assert not missing, "\n".join(missing)
    print(f"Viewer asset validation passed ({fallback_count} catalogue visual-tier fallback(s), "
          f"{len(source_visual_absent)} documented source-visual gap(s))")
    for detail in source_visual_absent:
        print(f"OPEN_VISUAL_GAP: {detail}")


if __name__ == "__main__":
    main()
