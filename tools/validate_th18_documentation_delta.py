#!/usr/bin/env python3
"""Validate the explicit TH18 public-documentation gap register."""
from __future__ import annotations

import json
import pathlib

from importer.reference import FROZEN_REFERENCE


ROOT = pathlib.Path(__file__).resolve().parents[1]
REFERENCE = ROOT / "data" / "reference" / "th18-2026-09-17.json"
REFERENCE_VERIFICATION = ROOT / "data" / "reference" / "th18-reference-verification-2026-09-18.json"
MECHANICS_AUDIT = ROOT / "data" / "reference" / "th18-mechanics-audit-2026-09-18.json"
DELTA = ROOT / "data" / "reference" / "th18-documentation-delta-2026-09-17.json"
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"
COVERAGE = ROOT / "docs" / "home-village-coverage.md"
REQUIRED_FIELDS = {"id", "scope", "sources_consulted", "missing", "impact",
                   "implementation_status", "validation_needed"}


def main() -> None:
    reference = json.loads(REFERENCE.read_text(encoding="utf-8"))
    verification = json.loads(REFERENCE_VERIFICATION.read_text(encoding="utf-8"))
    mechanics_audit = json.loads(MECHANICS_AUDIT.read_text(encoding="utf-8"))
    delta = json.loads(DELTA.read_text(encoding="utf-8"))
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    assert delta["schema_version"] == 1
    assert delta["reference_id"] == reference["reference_id"]
    assert delta["frozen_at"] == reference["frozen_at"]
    # The frozen reference is never rewritten.  A dated verification may add
    # consulted sources for an explicitly open gap; accepting those ids here
    # preserves provenance without turning an unversioned current page into a
    # historical numerical overwrite.
    sources = {source["id"] for source in reference["sources"]}
    sources.update(source["id"] for source in FROZEN_REFERENCE["sources"])
    sources.update(source["id"] for source in verification["sources"])
    sources.update(source["id"] for source in mechanics_audit["sources"])
    expected: dict[str, set[str]] = {}
    for index in reference["secondary_inventory_indices"]:
        expected.setdefault(index["family"], set()).update(index["ids"])
    for item in reference["official_deltas"]:
        if "id" in item:
            expected.setdefault(item["family"], set()).add(item["id"])
        for content_id in item.get("ids", []):
            expected.setdefault(item["family"], set()).add(content_id)
    expected_ids = set().union(*expected.values())
    loaded_ids = {content["id"] for content in catalogue["contents"]}
    loaded_ids.update(spell["id"] for spell in catalogue["spells"])
    missing_ids = expected_ids - loaded_ids

    entries = delta["entries"]
    assert entries and len({entry["id"] for entry in entries}) == len(entries)
    covered_missing_families: set[str] = set()
    known_ids = expected_ids | loaded_ids
    for entry in entries:
        assert REQUIRED_FIELDS <= set(entry), entry["id"]
        assert entry["sources_consulted"] and set(entry["sources_consulted"]) <= sources, entry["id"]
        assert all(isinstance(entry[field], str) and entry[field].strip()
                   for field in ("id", "missing", "impact", "implementation_status", "validation_needed")), entry["id"]
        scope = entry["scope"]
        assert scope["kind"] in {"all_expected_inventory", "implemented_combat_entities", "missing_catalogue_inventory", "item_ids"}, entry["id"]
        if scope["kind"] == "item_ids":
            assert scope["ids"] and set(scope["ids"]) <= known_ids, entry["id"]
        if scope["kind"] == "missing_catalogue_inventory":
            families = set(scope["families"])
            assert families <= set(expected), entry["id"]
            covered_missing_families |= families

    missing_families = {family for family, ids in expected.items() if ids - loaded_ids}
    assert missing_families <= covered_missing_families, (missing_families, covered_missing_families)
    assert {"tornado-displacement", "ground-wall-route-selection",
            "skeleton-trap-spawn-lifecycle-and-assets", "spell-element-specific-rules",
            "dated-cannon-and-archer-tower-tables", "mortar-minimum-range",
            "air-defense-supercharge", "wizard-tower-splash-boundary",
            "hidden-tesla-reveal-contract"} <= {entry["id"] for entry in entries}
    assert "air-bomb-launch-range-and-delay" in {entry["id"] for entry in entries}
    assert "baby-dragon-tantrum-transition-order" in {entry["id"] for entry in entries}
    # These tiers are now physically present and independently checked by the
    # Viewer asset validator.  Do not let the living delta/coverage document
    # regress to the obsolete claim that their absence blocks GUI rendering.
    baby_dragon = next(entry for entry in entries if entry["id"] == "baby-dragon-tantrum-transition-order")
    miner = next(entry for entry in entries if entry["id"] == "miner-underground-transition-and-visibility")
    assert "present in the local asset pack" in baby_dragon["validation_needed"]
    assert "present in the local asset pack" in miner["validation_needed"]
    coverage = COVERAGE.read_text(encoding="utf-8")
    assert "no Baby Dragon sprite" not in coverage
    assert "sprite source du\nMineur est encore absent" not in coverage
    assert "coefficient de dégâts du Gobelin sur les ressources n'est pas exposé" not in coverage
    assert "sprites WebP\npublics de Gobelin et Serviteur ne sont pas encore décodés" not in coverage
    clan_castle = next(entry for entry in entries if entry["id"] == "home-clan-castle-defending-lifecycle")
    assert clan_castle["scope"] == {"kind": "item_ids", "ids": ["clan_castle"]}
    assert "supercell-support-2026-09-17-clan-castle" in clan_castle["sources_consulted"]
    sneaky = next(entry for entry in entries if entry["id"] == "home-sneaky-goblin-cloak-interactions")
    assert sneaky["scope"] == {"kind": "item_ids", "ids": ["sneaky_goblin"]}
    assert "supercell-2020-03-30-spring-super-troops" in sneaky["sources_consulted"]
    assert "Snapshot V11" in sneaky["impact"]
    inferno = next(entry for entry in entries if entry["id"] == "inferno-single-target-ramp-and-resource")
    assert inferno["scope"] == {"kind": "item_ids", "ids": ["inferno_tower"]}
    assert "fandom-2026-09-17-inferno-tower" in inferno["sources_consulted"]
    assert inferno["implementation_status"] == "secondary_three_stage_single_target_ramp_and_lock_reset_pending_tick_freeze_and_ammunition_contract"
    tesla = next(entry for entry in entries if entry["id"] == "hidden-tesla-reveal-contract")
    assert "fandom-2026-09-17-hidden-tesla" in tesla["sources_consulted"]
    assert tesla["implementation_status"] == "secondary_51_percent_destruction_and_proximity_reveal_shared_core_gui_rl_snapshot_replay_weighting_open"
    bomb_tower = next(entry for entry in entries if entry["id"] == "bomb-tower-death-radius")
    assert {"cocmap-2026-09-17-bomb-tower", "fandom-2026-09-17-bomb-tower"} <= set(bomb_tower["sources_consulted"])
    assert bomb_tower["implementation_status"] == "secondary_l1_to_l13_delayed_ground_only_death_explosion_shared_core_gui_rl_snapshot_replay_pending_tick_order"
    assert "RL catalogue rows expose the imported delay" in bomb_tower["impact"]
    spells = next(entry for entry in entries if entry["id"] == "spell-element-specific-rules")
    assert "reserved spell level" in spells["impact"]
    assert "every executable spell level" in spells["impact"]
    assert "zone orange" in coverage and "death_explosions" in coverage
    root_rider = next(entry for entry in entries if entry["id"] == "home-root-rider-wall-contract-and-level-4")
    assert root_rider["scope"] == {"kind": "item_ids", "ids": ["root_rider"]}
    assert {"supercell-2023-12-09-root-rider", "supercell-2024-06-03-root-rider-balance", "goblinsfarm-2026-09-16-root-rider", "clashrecord-2026-09-17-root-rider"} <= set(root_rider["sources_consulted"])
    assert "L4 is imported from a dated secondary table" in root_rider["impact"]
    defense_batch = next(entry for entry in entries if entry["id"] == "home-defense-special-mechanics-batch-1")
    assert {"supercell-2024-11-25-th17-update", "goblinsfarm-2026-08-26-air-sweeper", "fandom-2026-09-18-air-sweeper", "goblinsfarm-2026-09-16-monolith", "fandom-2026-09-18-monolith"} <= set(defense_batch["sources_consulted"])
    assert "conflicting secondary cone descriptions" in defense_batch["missing"]
    # The frozen inventory is now wholly represented in the catalogue.  This
    # does not imply fidelity: the independent behavior/statistic records above
    # remain mandatory and deliberately keep the delta open.
    assert not missing_ids, missing_ids
    print(f"TH18 documentation delta validation passed: {len(entries)} open records, {len(missing_ids)} expected elements not yet loaded")


if __name__ == "__main__":
    main()
