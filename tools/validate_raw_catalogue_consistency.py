#!/usr/bin/env python3
"""Detect unrecorded drift from the immutable raw catalogue cache.

The frozen reference can intentionally override a sourced field, but every
other imported field must still equal the normalized value derived from the
cached full-commit payload.  This keeps sizes, levels and combat statistics
from silently changing between the raw evidence, the catalogue and GameData.
"""
from __future__ import annotations

import hashlib
import importlib.util
import json
import pathlib


ROOT = pathlib.Path(__file__).resolve().parents[1]
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"
IMPORTER = ROOT / "tools" / "import_data.py"


def importer_module():
    spec = importlib.util.spec_from_file_location("cocsim_import_data", IMPORTER)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def assert_fields(actual: dict, expected: dict, excluded: set[str], label: str) -> int:
    checked = 0
    for name, value in expected.items():
        if name in excluded:
            continue
        assert actual.get(name) == value, (label, name, actual.get(name), value)
        checked += 1
    return checked


def main() -> None:
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    importer = importer_module()
    commit = catalogue["source"]["commit"]
    cache = ROOT / "data" / "raw" / commit
    contents = {item["id"]: item for item in catalogue["contents"]}
    spells = {item["id"]: item for item in catalogue["spells"]}
    root_fields = {
        "footprint_tiles", "housing_space", "activation_housing_space",
        "range_tiles", "trigger_range_tiles", "min_range_tiles",
        "shots_per_burst", "time_between_bursts_seconds",
        "attack_speed_seconds", "movement_speed", "projectile_speed_tiles_per_second", "target_type", "damage_type",
    }
    checked_payloads = checked_levels = checked_fields = checked_super_variants = 0

    for content_id, (source_id, relative_path) in importer.FILES.items():
        path = cache / pathlib.Path(relative_path).name
        assert path.is_file(), f"missing immutable raw payload: {path.name}"
        payload = path.read_bytes()
        source = json.loads(payload)
        assert source["id"] == source_id, (content_id, source["id"], source_id)
        content = contents[content_id]
        assert content["source_id"] == source_id
        if source.get("category") == "hero-equipment":
            assert content["levels"], (content_id, "equipment has no levels")
            recorded_source = content["levels"][0]["provenance"]["source"]
            expected = importer.apply_content_overrides(
                importer.normalized_hero_equipment(content_id, source_id, source,
                                                   recorded_source), content_id)
            assert content == expected, (content_id, "hero equipment drift")
            assert all(level["provenance"]["source"]["raw_sha256"] == hashlib.sha256(payload).hexdigest()
                       for level in content["levels"]), (content_id, "provenance")
            checked_levels += len(content["levels"])
            checked_fields += len(content) + sum(len(level) for level in content["levels"])
            checked_payloads += 1
            continue
        expected_root = {
            "footprint_tiles": source.get("size"),
            "housing_space": source.get("housingSpace", 0),
            "activation_housing_space": source.get("modes", {}).get("normal", {}).get(
                "activationHousingSpace", source.get("triggerHousingSpace", 0)),
            "range_tiles": source.get("range", source.get("modes", {}).get("normal", {}).get("range")),
            "trigger_range_tiles": source.get("triggerRadius", source.get("modes", {}).get("normal", {}).get("triggerRange", 0)),
            "min_range_tiles": source.get("modes", {}).get("normal", {}).get("minRange", 0),
            "shots_per_burst": source.get("modes", {}).get("normal", {}).get("shotsPerBurst", 1),
            "time_between_bursts_seconds": source.get("modes", {}).get("normal", {}).get("timeBetweenBursts", 0),
            "attack_speed_seconds": source.get("attackSpeed", source.get("modes", {}).get("normal", {}).get("attackSpeed")),
            "movement_speed": source.get("movementSpeed"),
            "projectile_speed_tiles_per_second": source.get("projectileSpeed"),
            "target_type": source.get("targetType"),
            "damage_type": source.get("damageType", source.get("modes", {}).get("normal", {}).get("damageType")),
        }
        expected_overrides = importer.CONTENT_OVERRIDES.get(content_id, {})
        if content_id == "hero_banner":
            # This non-combat record has no normal combat level. Its distinct
            # field provenance comes from the separately pinned observation,
            # rather than the combat content-override table.
            assert content.get("field_provenance", {}) == {
                "targetable": {"source": "fandom-2026-09-17-hero-banner", "tier": "secondary"},
                "counts_toward_destruction": {"source": "fandom-2026-09-17-hero-banner", "tier": "secondary"},
                "deployment_margin_tiles": {"source": "fandom-2026-09-17-hero-banner", "tier": "secondary"},
                "image": {"source": "chiefpansancolt-62b019df", "tier": "secondary"},
                "source_images": {"source": "chiefpansancolt-62b019df-hero-banner-variants", "note": "Scenario variant is visual assignment data only. It cannot instantiate or modify a defending Hero."},
                "max_placements": {"source": "supercell-support-2026-09-20-hero-banner-quantity", "note": "TH18 Home Village constraint, corroborated by the pinned availability table."},
            }, (content_id, "non-combat field provenance drift")
            assert content["source_images"] == {
                "normal": "images/home/army-buildings/hero-banner/empty.png",
                "barbarian_king": "images/home/army-buildings/hero-banner/barbarian-king.png",
                "archer_queen": "images/home/army-buildings/hero-banner/archer-queen.png",
                "grand_warden": "images/home/army-buildings/hero-banner/grand-warden.png",
                "royal_champion": "images/home/army-buildings/hero-banner/royal-champion.png",
                "minion_prince": "images/home/army-buildings/hero-banner/minion-prince.png",
                "dragon_duke": "images/home/army-buildings/hero-banner/dragon-duke.png",
            }, (content_id, "source visual variants drift")
            assert content["max_placements"] == 4, (content_id, "TH18 placement limit drift")
        else:
            assert content.get("field_provenance", {}) == {
                name: {"source": evidence["source"], "note": evidence.get("note", "")}
                for name, evidence in expected_overrides.items()
            }, (content_id, "field provenance is not the frozen-reference override")
        # A non-combat placement record deliberately omits combat-root fields
        # such as housing, weapon range and target type.  Its separate
        # provenance assertion above is its complete contract.
        root_overrides = set(expected_overrides) | (root_fields if content_id == "hero_banner" else set())
        checked_fields += assert_fields(content, expected_root, root_overrides, content_id + " root")
        digest = hashlib.sha256(payload).hexdigest()
        actual_levels = {(item["level"], item.get("variant", "normal")): item
                         for item in content.get("levels", [])}
        hero_icon = source.get("images", {}).get("icon") if source.get("category") == "hero" else None
        for raw_level in source.get("levels", []):
            expected = importer.normalized_level(raw_level, hero_icon,
                                                 pet=source.get("category") == "pet")
            key = (expected["level"], expected["variant"])
            actual = actual_levels[key]
            provenance = actual["provenance"]
            if provenance["source"]["source"] != "chiefpansancolt-62b019df":
                continue
            assert provenance["source"]["raw_sha256"] == digest, (content_id, key)
            checked_fields += assert_fields(actual, expected, set(provenance["overridden_fields"]) | {"level"},
                                            f"{content_id} L{expected['level']} {expected['variant']}")
            checked_levels += 1
        # Super Troops are nested in their immutable parent payloads, rather
        # than fetched from mutable guessed URLs. Reconstruct their normalized
        # entry from that same payload and compare the entire public contract,
        # including the parent-field provenance marker.
        if raw_super := source.get("superTroop"):
            super_id = raw_super["id"].replace("-", "_")
            actual_super = contents[super_id]
            assert actual_super["levels"], (content_id, "super troop has no levels")
            recorded_source = actual_super["levels"][0]["provenance"]["source"]
            parent_source = {key: value for key, value in recorded_source.items()
                             if key != "source_path"}
            expected_super = importer.normalized_super_troop(content_id, source, raw_super,
                                                              parent_source)
            expected_super = importer.apply_content_overrides(expected_super, super_id)
            assert actual_super == expected_super, (content_id, super_id, "nested super troop drift")
            # A current Super Troop can receive a hash-pinned extra level
            # absent from the nested raw payload (for example Super Dragon
            # L13). Its complete equality with `expected_super` above is
            # already checked; retain the raw-payload provenance assertion for
            # all other rows and separately require the supplemental source.
            supplements = {level["level"]: level for level in importer.EXTRA_LEVELS.get(super_id, [])}
            for level in actual_super["levels"]:
                provenance = level["provenance"]["source"]
                if level["level"] in supplements:
                    expected_provenance = supplements[level["level"]]["provenance"]["source"]
                    assert provenance == expected_provenance, (content_id, super_id, level["level"], "supplement provenance")
                else:
                    assert (provenance["raw_sha256"] == digest
                            and provenance["source_path"] == "superTroop.levels"), (content_id, super_id, level["level"], "raw provenance")
            checked_super_variants += 1
            checked_levels += len(actual_super["levels"])
            checked_fields += sum(len(level) for level in actual_super["levels"]) + len(actual_super)
        checked_payloads += 1

    for spell_id, (source_id, relative_path) in importer.SPELLS.items():
        path = cache / pathlib.Path(relative_path).name
        assert path.is_file(), f"missing immutable raw payload: {path.name}"
        payload = path.read_bytes()
        source = json.loads(payload)
        assert source["id"] == source_id, (spell_id, source["id"], source_id)
        spell = spells[spell_id]
        assert spell["source_id"] == source_id
        actual_levels = {item["level"]: item for item in spell["levels"]}
        digest = hashlib.sha256(payload).hexdigest()
        for raw_level in source.get("levels", []):
            expected = importer.normalized_spell_level(raw_level)
            actual = actual_levels[expected["level"]]
            provenance = actual["provenance"]
            if provenance["source"]["source"] != "chiefpansancolt-62b019df":
                continue
            assert provenance["source"]["raw_sha256"] == digest, (spell_id, expected["level"])
            checked_fields += assert_fields(actual, expected, set(provenance["overridden_fields"]) | {"level"},
                                            f"{spell_id} L{expected['level']}")
            checked_levels += 1
        checked_payloads += 1

    # Current spells absent from the pinned upstream tree use a separately
    # hashed observation.  Compare the entire normalized record, including the
    # per-level provenance synthesized from that artifact, so a catalogue
    # refresh cannot silently erase or alter a secondary-table field.
    for expected_spell in importer.SUPPLEMENTAL_SPELLS:
        actual_spell = spells[expected_spell["id"]]
        assert actual_spell == expected_spell, (expected_spell["id"], "supplement spell drift")
        source = actual_spell["levels"][0]["provenance"]["source"]
        artifact = ROOT / "data" / "reference" / source["pinned_observation"]
        assert hashlib.sha256(artifact.read_bytes()).hexdigest() == source["raw_sha256"], (
            expected_spell["id"], "supplement spell provenance")
        checked_levels += len(actual_spell["levels"])
        checked_fields += len(actual_spell) + sum(len(level) for level in actual_spell["levels"])

    for expected_content in importer.SUPPLEMENTAL_CONTENTS:
        # Supplemental records can be superseded by a later official delta in
        # the frozen reference, just like raw upstream records.  Reapply that
        # ledger before comparison so the validator protects the generated
        # result instead of demanding an obsolete secondary observation.
        expected_content = importer.apply_content_overrides(expected_content,
                                                             expected_content["id"])
        actual_content = contents[expected_content["id"]]
        assert actual_content == expected_content, (expected_content["id"], "supplement content drift")
        source = actual_content["levels"][0]["provenance"]["source"]
        artifact = ROOT / "data" / "reference" / source["pinned_observation"]
        assert hashlib.sha256(artifact.read_bytes()).hexdigest() == source["raw_sha256"], (
            expected_content["id"], "supplement content provenance")
        checked_levels += len(actual_content["levels"])
        checked_fields += len(actual_content) + sum(len(level) for level in actual_content["levels"])

    assert checked_payloads == len(importer.FILES) + len(importer.SPELLS)
    assert checked_super_variants == 17, checked_super_variants
    print(f"Raw catalogue consistency passed ({checked_payloads} payloads, {checked_super_variants} nested Super Troops, {checked_levels} levels, {checked_fields} fields)")


if __name__ == "__main__":
    main()
