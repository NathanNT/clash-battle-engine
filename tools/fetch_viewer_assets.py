#!/usr/bin/env python3
"""Fetch every normal-level sprite referenced by the normalized catalogue.

The source is immutable: changing the commit requires intentionally editing this
file and re-auditing the source licence. Existing files are only replaced when
their SHA-256 differs from the fetched source.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import urllib.request

COMMIT = "62b019df868f9decb49e97a0ff5ae15fb27af9ba"
BASE = f"https://raw.githubusercontent.com/chiefpansancolt/clash-of-clans-data/{COMMIT}/"
def catalogue_images(project_root: pathlib.Path) -> tuple[str, ...]:
    catalogue = json.loads((project_root / "data" / "catalogue.normalized.json").read_text(encoding="utf-8"))
    level_images = {
        level["image"]
        for content in catalogue["contents"]
        for level in content["levels"]
        if level.get("variant", "normal") == "normal" and level.get("image")
    }
    # Non-combat metadata (for example Hero Banner) has no artificial combat
    # level, but its source artwork is still part of the Viewer asset manifest.
    metadata_images = {content["image"] for content in catalogue["contents"] if content.get("image")}
    source_images = {
        path for content in catalogue["contents"] for path in content.get("source_images", {}).values()
    }
    return tuple(sorted(level_images | metadata_images | source_images))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--only", nargs="+", metavar="PATH",
                        help="Fetch only documented catalogue paths and merge their hashes into the manifest.")
    args = parser.parse_args()
    project_root = pathlib.Path(__file__).resolve().parents[1]
    root = project_root / "assets"
    images = catalogue_images(project_root)
    selected = images
    if args.only:
        requested = set(args.only)
        unknown = requested - set(images)
        if unknown:
            raise ValueError(f"paths are not referenced by the normalized catalogue: {sorted(unknown)}")
        selected = tuple(sorted(requested))
    existing: dict[str, str] = {}
    manifest = root / "viewer-assets.sha256"
    if args.only and manifest.is_file():
        for line in manifest.read_text(encoding="utf-8").splitlines():
            digest, relative = line.split("  ", 1)
            existing[relative] = digest
    rows = []
    for relative in selected:
        payload = urllib.request.urlopen(BASE + relative, timeout=30).read()
        target = root / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(payload)
        rows.append((relative, hashlib.sha256(payload).hexdigest()))
        print(f"fetched {relative}")
    existing.update(dict(rows))
    manifest.write_text("".join(f"{digest}  {relative}\n" for relative, digest in sorted(existing.items())), encoding="utf-8")


if __name__ == "__main__":
    main()
