#!/usr/bin/env python3
"""Keep the persistent TH18 goal's permanent-content boundary explicit."""
from __future__ import annotations

import json
import pathlib


ROOT = pathlib.Path(__file__).resolve().parents[1]
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"
SCOPE = ROOT / "data" / "reference" / "th18-permanent-scope-2026-09-18.json"


def ids(value: object) -> set[str]:
    assert isinstance(value, list)
    assert all(isinstance(item, str) for item in value)
    return set(value)


def main() -> None:
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    scope = json.loads(SCOPE.read_text(encoding="utf-8"))
    assert scope["reference_id"] == "home-village-th18-2026-09-17"
    assert scope["active_reference_id"] == "home-village-th18-2026-09-18-meteor-golem-meteormite-baseline"
    assert scope["reference_chain"] == [scope["reference_id"], "home-village-th18-2026-09-18-monolith-l5", "home-village-th18-2026-09-18-super-wall-breaker", "home-village-th18-2026-09-18-super-barbarian", "home-village-th18-2026-09-18-super-giant", "home-village-th18-2026-09-18-super-archer", "home-village-th18-2026-09-18-rocket-balloon", "home-village-th18-2026-09-18-inferno-dragon", "home-village-th18-2026-09-18-super-wizard", "home-village-th18-2026-09-18-super-minion", "home-village-th18-2026-09-18-super-bowler", "home-village-th18-2026-09-18-super-dragon", "home-village-th18-2026-09-18-x-bow-footprint", "home-village-th18-2026-09-18-electro-dragon-chain", "home-village-th18-2026-09-18-bowler-bounce", "home-village-th18-2026-09-18-ice-golem-death-freeze", "home-village-th18-2026-09-18-apprentice-warden-life-aura", "home-village-th18-2026-09-18-super-hog-rider-split", "home-village-th18-2026-09-18-super-miner", "home-village-th18-2026-09-18-super-valkyrie", "home-village-th18-2026-09-18-super-yeti-baseline", "home-village-th18-2026-09-18-super-witch-baseline", "home-village-th18-2026-09-18-ice-hound-ice-pup", "home-village-th18-2026-09-18-yeti-yetimite", "home-village-th18-2026-09-18-witch-skeleton-baseline", "home-village-th18-2026-09-18-lava-hound-lava-pup-baseline", "home-village-th18-2026-09-18-headhunter-baseline", "home-village-th18-2026-09-18-druid-bear-baseline", "home-village-th18-2026-09-18-furnace-firemite-baseline", scope["active_reference_id"]]
    assert scope["level_and_variant_inventory"]["source"] == "data/catalogue.normalized.json"

    included_groups = scope["included_groups"]
    included = set().union(*(ids(group) for group in included_groups.values()))
    assert sum(len(ids(group)) for group in included_groups.values()) == len(included), "scope groups overlap"
    assert len(included) == 111, "update the reviewed permanent inventory deliberately"

    contents = {item["id"]: item for item in catalogue["contents"]}
    assert included <= contents.keys(), f"scope ids absent from catalogue: {sorted(included - contents.keys())}"
    assert not included & {spell["id"] for spell in catalogue["spells"]}

    category_by_group = {
        "defensive_buildings": {"defense"},
        "resource_and_town_hall_buildings": {"resource", "town-hall"},
        "army_and_other_buildings": {"army", "research", "other"},
        "walls": {"wall"},
        "traps": {"trap"},
        "troops_and_required_spawned_subunits": {"troop", "spawned-unit"},
    }
    for group, allowed_categories in category_by_group.items():
        for content_id in ids(included_groups[group]):
            assert contents[content_id]["category"] in allowed_categories, content_id

    # Combat and targetable buildings must expose at least one level. Hero
    # Banner is the explicitly sourced placement-only exception and remains in
    # scope precisely because its Core geometry and rendering are observable.
    for content_id in included:
        content = contents[content_id]
        levels = content.get("levels", [])
        if content_id == "hero_banner":
            assert not levels and content["targetable"] is False
            assert content.get("deployment_margin_tiles") is not None
        else:
            assert levels, f"included permanent content without levels: {content_id}"
            assert all(level.get("provenance", {}).get("source", {}).get("raw_sha256") for level in levels), content_id

    required_spawned = scope["required_spawned_subunits"]
    for parent, children in required_spawned.items():
        assert parent in included
        for child in ids(children):
            if child in scope["unmaterializable_required_spawned_subunits"]:
                unresolved = scope["unmaterializable_required_spawned_subunits"][child]
                assert unresolved["parent"] == parent
                assert unresolved["evidence_audit"].startswith("data/reference/")
                # The immutable scope records the state known when it was
                # frozen. A later hash-pinned delta may legitimately make a
                # child materializable, as happened for Lava Pup. Keep the
                # historical uncertainty while requiring the live catalogue
                # to classify any newly present child normally below.
                if child in contents:
                    assert (contents[child].get("support") == "spawned_only"
                            or contents[child]["category"] == "spawned-unit"), child
                continue
            assert child in included
            # Existing Core-spawned children are marked `spawned_only`; Ruin
            # Knight is deliberately still data-only but its source category
            # declares it as the required spawned unit. The inventory must
            # retain both states without claiming either behavior complete.
            assert (contents[child].get("support") == "spawned_only"
                    or contents[child]["category"] == "spawned-unit"), child

    exclusions = scope["explicit_exclusions"]
    temporary = ids(exclusions["temporary_crafted_defenses"]["ids"])
    assert temporary.isdisjoint(included)
    assert all(contents[content_id]["category"] == "crafted-defense" for content_id in temporary)
    guardians = ids(exclusions["non_building_troop_trap_combatants"]["ids"])
    assert guardians.isdisjoint(included)
    assert all(contents[content_id]["category"] == "guardian" for content_id in guardians)
    excluded_categories = set(exclusions["goal_exclusions"]["categories"])
    assert all(contents[content_id]["category"] not in excluded_categories for content_id in included)

    unresolved_children = scope["unmaterializable_required_spawned_subunits"]
    assert {child for child in unresolved_children if child not in contents} == {
        child for children in required_spawned.values()
        for child in children if child not in contents
    }
    print(f"TH18 permanent scope validation passed ({len(included)} materializable ids, "
          f"{len(unresolved_children)} required spawned subunits not yet materializable, "
          f"{len(temporary)} temporary crafted defenses excluded)")


if __name__ == "__main__":
    main()
