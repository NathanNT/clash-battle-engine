#!/usr/bin/env python3
"""Fail fast if the frozen TH18 ledger and normalized data drift apart."""
from __future__ import annotations

import json
import hashlib
import pathlib
import re

from importer.reference import FROZEN_REFERENCE


ROOT = pathlib.Path(__file__).resolve().parents[1]
REFERENCE = ROOT / "data" / "reference" / "th18-2026-09-17.json"
REVISIONS = ROOT / "data" / "reference" / "th18-2026-09-17-revisions.json"
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"


def main() -> None:
    reference = json.loads(REFERENCE.read_text(encoding="utf-8"))
    revisions = json.loads(REVISIONS.read_text(encoding="utf-8"))
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    assert reference["reference_id"] == "home-village-th18-2026-09-17"
    assert revisions["parent_reference"] == REFERENCE.name
    correction = next(item for item in revisions["revisions"]
                      if item["scope"] == {"content_id": "trap_skeleton_ground", "levels": [1, 2]})
    assert correction["scope"] == {"content_id": "trap_skeleton_ground", "levels": [1, 2]}
    assert correction["previous"]["attack_speed_seconds"] == 1
    assert correction["replacement"]["attack_speed_seconds"] == 0.7
    assert correction["replacement"]["observation_sha256"] == "18734427fbacbc4b04213e7394906fe27948685935a3094fb5cc849c5b87e00d"
    wall_breaker_correction = next(item for item in revisions["revisions"]
                                   if item["scope"] == {"content_id": "wall_breaker", "fields": ["attack_speed_seconds"]})
    assert wall_breaker_correction["scope"] == {"content_id": "wall_breaker", "fields": ["attack_speed_seconds"]}
    assert wall_breaker_correction["previous"]["attack_speed_seconds"] == 0.7
    assert wall_breaker_correction["replacement"]["attack_speed_seconds"] == 1
    assert wall_breaker_correction["replacement"]["raw_sha256"] == "f8bbbe2d5e0eb43ff5bbd9a3734bf490b52d09a1eddc92fd45b7de6bbbafbf81"
    giant_correction = next(item for item in revisions["revisions"]
                            if item["scope"] == {"content_id": "giant", "level": 1, "fields": ["town_hall_required"]})
    assert giant_correction["previous"]["town_hall_required"] == 1
    assert giant_correction["replacement"]["town_hall_required"] == 2
    cannon_correction = next(item for item in revisions["revisions"]
                             if item["scope"] == {"content_id": "cannon", "level": 2, "fields": ["town_hall_required"]})
    assert cannon_correction["previous"]["town_hall_required"] == 1
    assert cannon_correction["replacement"]["town_hall_required"] == 2
    archer_correction = next(item for item in revisions["revisions"]
                             if item["scope"] == {"content_id": "archer", "level": 1, "fields": ["town_hall_required"]})
    assert archer_correction["previous"]["town_hall_required"] == 1
    assert archer_correction["replacement"]["town_hall_required"] == 2
    dragon_rider_correction = next(item for item in revisions["revisions"]
                                   if item["scope"] == {"content_id": "dragon_rider", "fields": ["target_type"]})
    assert dragon_rider_correction["previous"] == {"target_type": "ground", "source": "chiefpansancolt-62b019df"}
    assert dragon_rider_correction["replacement"] == {"target_type": "both", "source": "supercell-2021-06-13-dragon-rider"}
    electro_dragon_correction = next(item for item in revisions["revisions"]
                                     if item["scope"] == {"content_id": "electro_dragon", "levels": [8, 9], "fields": ["hitpoints", "dps", "damage_per_shot"]})
    assert electro_dragon_correction["source"] == "goblinsfarm-2026-09-16-electro-dragon"
    assert electro_dragon_correction["previous"]["levels"][1] == {"level": 9, "hitpoints": 6700, "dps": 500, "damage_per_shot": 1750}
    assert electro_dragon_correction["replacement"]["levels"][1] == {"level": 9, "hitpoints": 6900, "dps": 510, "damage_per_shot": 1785}
    electro_dragon_range_correction = next(item for item in revisions["revisions"]
                                           if item["scope"] == {"content_id": "electro_dragon", "fields": ["range_tiles"]})
    assert electro_dragon_range_correction["previous"] == {"range_tiles": 3, "source": "chiefpansancolt-62b019df"}
    assert electro_dragon_range_correction["replacement"]["range_tiles"] == 2.5
    ruin_witch_knight_correction = next(item for item in revisions["revisions"]
                                        if item["scope"] == {"content_ids": ["ruin_witch", "ruin_knight"], "fields": ["max_summoned_ruin_knights", "hitpoint_decay_per_second"]})
    assert ruin_witch_knight_correction["source"] == "supercell-2026-08-30-update"
    assert ruin_witch_knight_correction["previous"]["max_summoned_ruin_knights"] == 8
    assert ruin_witch_knight_correction["replacement"] == {"max_summoned_ruin_knights": 10, "hitpoint_decay_per_second": 0, "source": "supercell-2026-08-30-update"}
    assert reference["status"] == "provisional_official_delta_inventory"
    # Historical source assertions below intentionally continue to inspect the
    # immutable parent.  Catalogue provenance may additionally name a source
    # admitted by the independently validated active-reference delta.
    sources = {source["id"]: source for source in reference["sources"]}
    sources.update({source["id"]: source for source in FROZEN_REFERENCE["sources"]})
    # The source register is the indirection used by every per-level field
    # provenance.  Make its minimum audit trail machine-checkable: a source
    # id alone is not enough to reproduce, date or classify a TH18 value.
    iso_date = re.compile(r"^\d{4}-\d{2}-\d{2}$")
    for source_id, source in sources.items():
        assert source["id"] == source_id
        assert source.get("tier") in {
            "primary", "primary-undated", "secondary", "secondary-frozen-payload", "secondary-dated-build", "secondary-build-pinned", "secondary-dated-game-files",
            "secondary-version-unknown", "secondary-derived-from-pinned-upstream",
            "secondary-version-unknown-historical", "secondary-search-index-only", "experimental",
        }, source_id
        assert isinstance(source.get("publisher"), str) and source["publisher"], source_id
        assert isinstance(source.get("url"), str) and source["url"].startswith("https://"), source_id
        assert iso_date.fullmatch(str(source.get("consulted_on", ""))), source_id
        dated = [source.get(key) for key in ("published_on", "updated_on", "consulted_on")]
        assert any(iso_date.fullmatch(str(value)) for value in dated), source_id
    for source_id in ("supercell-2020-03-30-spring-super-troops", "supercell-2021-06-13-dragon-rider", "supercell-2023-12-09-root-rider", "supercell-2024-06-03-root-rider-balance", "supercell-2025-03-24-clash-anytime", "supercell-2025-11-17-th18-launch", "supercell-2026-02-23-th18", "supercell-2026-04-27-sound-of-clash", "supercell-2026-05-22-balance", "supercell-2026-06-15-anime-fury", "supercell-2026-07-09-balance", "supercell-2026-08-30-update", "supercell-support-2026-09-17-clan-castle"):
        assert sources[source_id]["tier"] == "primary"
        assert sources[source_id]["url"].startswith(("https://supercell.com/", "https://support.supercell.com/"))
    assert sources["fandom-2026-09-17-inferno-tower"]["tier"] == "secondary"
    assert sources["fandom-2026-09-17-bomb-tower"]["tier"] == "secondary"
    assert sources["cocmap-2026-09-17-bomb-tower"]["tier"] == "secondary"
    assert sources["fandom-2026-09-17-hidden-tesla"]["tier"] == "secondary"
    assert "replacement numerical values" in sources["supercell-2026-07-09-balance"]["limitations"]
    tesla_pekka_audit = reference["resolved_behavior_audits"][0]
    assert tesla_pekka_audit["id"] == "hidden-tesla-pekka-legacy-damage"
    assert tesla_pekka_audit["sources"][0] == "fandom-2026-09-17-pekka-history"

    # Every imported value must retain a route back to the frozen source
    # register.  Checking a few exemplar levels is not enough: a new level or
    # supplement must never smuggle an unregistered provenance identifier into
    # the immutable catalogue.
    def assert_registered_provenance(entry: dict, label: str) -> None:
        provenance = entry.get("provenance", {})
        source = provenance.get("source", {})
        if source:
            source_id = source["source"]
            assert source_id in sources, (label, source_id)
            assert source.get("tier") == sources[source_id]["tier"], (label, source_id)
            raw_sha256 = source.get("raw_sha256")
            assert isinstance(raw_sha256, str) and re.fullmatch(r"[0-9a-f]{64}", raw_sha256), label

    for content in catalogue["contents"]:
        for level in content.get("levels", []):
            assert_registered_provenance(level, f'{content["id"]} L{level.get("level")}')
        for field, provenance in content.get("field_provenance", {}).items():
            source_id = provenance.get("source")
            if source_id:
                if isinstance(source_id, dict):
                    source_id = source_id["source"]
                # This is intentionally not a public source identifier: it
                # records the explicit conservative fallback used when the
                # official Long Shot statement omits a numeric multiplier.
                # Keep it narrow so a future field cannot bypass source
                # registration under the same label.
                if source_id == "documented-deterministic-lower-bound":
                    assert content["id"] == "super_minion"
                    assert field == "opening_long_shot_damage_multiplier"
                    assert "ordinary imported damage" in provenance["note"]
                    continue
                assert source_id in sources, (content["id"], field, source_id)
    for spell in catalogue["spells"]:
        for level in spell.get("levels", []):
            assert_registered_provenance(level, f'{spell["id"]} L{level.get("level")}')
    assert "removed on 2016-10-12" in tesla_pekka_audit["finding"]
    assert tesla_pekka_audit["core_contract"].startswith("No P.E.K.K.A-specific")
    skeleton_air_delta = next(item for item in reference["official_deltas"] if item["id"] == "trap_skeleton_air")
    assert skeleton_air_delta == {"source": "supercell-2025-03-24-clash-anytime", "family": "spawned_unit", "id": "trap_skeleton_air", "values": {"housing_space": 1}, "behavior": "Air Skeleton housing space was reduced from 2 to 1."}
    skeleton_trap_delta = next(item for item in reference["official_deltas"] if item["id"] == "skeleton_trap")
    assert skeleton_trap_delta["source"] == "1337wiki-2026-09-17-skeleton-trap"
    assert "exclusively" in skeleton_trap_delta["behavior"]
    inventory_indices = {(item["family"], item["source"]): item for item in reference["secondary_inventory_indices"]}
    troop_index = inventory_indices[("troop", "fandom-2026-09-17-home-troops-index")]
    assert len(troop_index["ids"]) == 32
    assert {"meteor_golem", "furnace", "ruin_witch"} <= set(troop_index["ids"])
    defense_index = inventory_indices[("defense", "fandom-2026-09-17-home-defenses-index")]
    assert {"air_sweeper", "monolith", "revenge_tower", "super_wizard_tower"} <= set(defense_index["ids"])
    assert inventory_indices[("resource", "fandom-2026-09-17-home-resource-index")]["ids"][-1] == "clan_castle"
    assert len(inventory_indices[("trap", "fandom-2026-09-17-home-traps-index")]["ids"]) == 8
    assert len(inventory_indices[("siege", "fandom-2026-09-17-home-siege-index")]["ids"]) == 9
    assert len(inventory_indices[("hero", "fandom-2026-09-17-home-heroes-index")]["ids"]) == 6
    assert len(inventory_indices[("pet", "fandom-2026-09-17-home-pets-index")]["ids"]) == 12
    spell_index = inventory_indices[("spell", "fandom-2026-09-17-home-spell-equipment-index")]
    assert len(spell_index["ids"]) == 18
    assert {"totem", "angry"} <= set(spell_index["ids"])
    assert len(inventory_indices[("super_troop", "fandom-2026-09-17-home-spell-equipment-index")]["ids"]) == 17
    equipment_index = inventory_indices[("equipment", "fandom-2026-09-17-home-spell-equipment-index")]
    assert len(equipment_index["ids"]) == 41
    assert {"electro_fangs", "rocket_backpack", "monolith_arrow"} <= set(equipment_index["ids"])
    deltas = [item for item in reference["official_deltas"] if "id" in item]
    def has_delta(content_id, **fields):
        return any(item["id"] == content_id and all(item.get(key) == value for key, value in fields.items()) for item in deltas)
    assert has_delta("town_hall", behavior="Town Hall is no longer a defensive building")
    assert has_delta("ruin_knight", parent="ruin_witch")
    assert has_delta("smasher", town_hall=18)
    assert has_delta("super_wizard_tower", behavior="merges two Wizard Towers; attack chains to up to 15 nearby enemies")
    assert has_delta("dragon_rider", behavior="single-target damage; targets Ground & Air; crashes with damage when destroyed")
    assert has_delta("baby_dragon", level=12)
    assert any(item["id"] == "electro_dragon" and item.get("levels") == [8, 9]
               for item in deltas)
    assert reference["catalogue_content_overrides"][0]["fields"]["resource_damage_multiplier"]["value"] == 2
    baby_override = next(item for item in reference["catalogue_content_overrides"] if item["content_id"] == "baby_dragon")["fields"]
    assert (baby_override["isolation_radius_tiles"]["value"], baby_override["rage_damage_multiplier"]["value"], baby_override["rage_attack_speed_multiplier"]["value"]) == (4.5, 2, 1.5)
    miner_override = next(item for item in reference["catalogue_content_overrides"] if item["content_id"] == "miner")["fields"]
    assert miner_override["burrows"]["value"] is True
    sneaky_override = next(item for item in reference["catalogue_content_overrides"] if item["content_id"] == "sneaky_goblin")["fields"]
    assert sneaky_override["defense_invisibility_duration_seconds"] == {
        "value": 5,
        "source": "supercell-2020-03-30-spring-super-troops",
        "note": "Supercell specifies that the Sneaky Goblin is invisible to defenses for the first five seconds after deployment.",
    }
    sneaky = next(content for content in catalogue["contents"] if content["id"] == "sneaky_goblin")
    assert sneaky["defense_invisibility_duration_seconds"] == 5
    assert sneaky["field_provenance"]["defense_invisibility_duration_seconds"]["source"] == "supercell-2020-03-30-spring-super-troops"
    for content_id, level in (("barbarian", 13), ("goblin", 10), ("valkyrie", 12),
                              ("golem", 15), ("dragon", 13), ("balloon", 13),
                              ("x_bow", 13), ("giant_bomb", 12), ("army_camp", 14),
                              ("workshop", 9), ("blacksmith", 10)):
        assert has_delta(content_id, level=level)
    logger = next(item for item in deltas if item["id"] == "logger")
    assert logger["values"]["dps"] == 350
    assert logger["values"]["range_tiles_by_level"] == [[1, 4, 7], [5, 5, 8]]
    assert any(item.get("id") == "ruin_witch" and item.get("values", {}).get("max_ruin_knights") == 10
               for item in reference["official_deltas"])
    observations = {item["id"]: item for item in reference["secondary_observations"]}
    assert observations["dark_barracks"]["values"] == {"hitpoints": 1100, "footprint_tiles": "3x3", "town_hall_required": 16}
    assert observations["dark_spell_factory"]["values"] == {"hitpoints": 1070, "footprint_tiles": "3x3", "town_hall_required": 15}
    assert observations["dark_barracks"]["import_status"] == "catalogue_extra_level"
    assert observations["dark_spell_factory"]["import_status"] == "catalogue_extra_level"
    wall_breaker_observation = observations["wall_breaker"]
    assert wall_breaker_observation["import_status"] == "current_build_stats_corroboration"
    assert wall_breaker_observation["observation_sha256"] == "8eddc28ffeab93a03807a9125be51c00cce686c9adb098cea2c792dd89aeac50"
    giant_observation = observations["giant"]
    assert giant_observation["import_status"] == "current_build_stats_and_focus_corroboration"
    assert giant_observation["observation_sha256"] == "df094033d5b1484d8fab526ac324f9baa2e6c9cefa0f8f3707588bd126e1a28a"
    assert observations["balloon"]["import_status"] == "current_build_stats_and_focus_corroboration"
    assert observations["wizard"]["import_status"] == "current_build_stats_and_behavior_corroboration"
    assert observations["cannon"]["import_status"] == "dated_build_stats_and_footprint_corroboration"
    assert observations["archer_tower"]["import_status"] == "dated_build_stats_and_footprint_corroboration"
    assert observations["mortar"]["import_status"] == "dated_stats_corroboration"
    assert observations["air_defense"]["import_status"] == "dated_normal_stats_and_air_only_target_corroboration"
    assert observations["wizard_tower"]["import_status"] == "dated_normal_stats_and_splash_role_corroboration"
    assert observations["bomb_tower"]["import_status"] == "current_build_normal_stats_with_qualitative_death_behavior"
    mortar_minimum_range = observations["mortar_minimum_range"]
    assert mortar_minimum_range["import_status"] == "secondary_range_corroboration"
    minimum_range_path = REFERENCE.parent / mortar_minimum_range["observation_file"]
    assert hashlib.sha256(minimum_range_path.read_bytes()).hexdigest() == mortar_minimum_range["observation_sha256"]
    assert json.loads(minimum_range_path.read_text(encoding="utf-8"))["observed_root"]["min_range_tiles"] == 4
    hero_banner = observations["hero_banner"]
    assert hero_banner["values"] == {"footprint_tiles": "2x2", "targetable": False, "hitpoints": None, "counts_toward_destruction": False, "deployment_margin_tiles": 1, "image": "images/home/army-buildings/hero-banner/empty.png"}
    assert hero_banner["image_source"] == "chiefpansancolt-62b019df"
    ground_skeleton = observations["trap_skeleton_ground"]
    air_skeleton = observations["trap_skeleton_air"]
    assert ground_skeleton["levels"] == [{"level": 1, "hitpoints": 30, "damage_per_hit": 17.5, "dps": 25, "attack_speed_seconds": 0.7, "movement_speed": 24, "range_tiles": 0.4}, {"level": 2, "hitpoints": 45, "damage_per_hit": 21, "dps": 30, "attack_speed_seconds": 0.7, "movement_speed": 24, "range_tiles": 0.4}]
    assert air_skeleton["levels"][1]["damage_per_hit"] == 21
    assert air_skeleton["known_missing"] == ["lifetime", "retargeting and distraction rules"]
    extras = {(item["content_id"], item["level"]): item for item in reference["catalogue_extra_levels"]}
    assert extras[("dark_barracks", 13)]["observation_sha256"] == "17f38490c117b9a060017436d5ddcc1b5ee61eee3d28f594b7f3646a3675aa0b"
    assert extras[("dark_spell_factory", 8)]["observation_file"] == "secondary-observations-2026-09-17.json"
    supplemental = reference["catalogue_supplemental_contents"]
    assert supplemental == [
        {"content_id": "golemite", "source": "fandom-2026-09-17-golem", "observation_file": "golemite-observation-2026-09-17.json", "observation_sha256": "58a8bae19ef217d51703c6cc70d30d9b046fd2f93595d84563a57f4fbf320bae"},
        {"content_id": "trap_skeleton_ground", "source": "fandom-2026-09-17-skeleton-trap", "observation_file": "trap-skeleton-ground-observation-2026-09-17.json", "observation_sha256": "18734427fbacbc4b04213e7394906fe27948685935a3094fb5cc849c5b87e00d"},
        {"content_id": "trap_skeleton_air", "source": "fandom-2026-09-17-skeleton-trap", "observation_file": "trap-skeleton-air-observation-2026-09-17.json", "observation_sha256": "23f3ebc9232ff0efcd4ec5c20eb011ab15e76d0d8c1e49980c2cb31aad49bf62"},
        {"content_id": "sky_wagon", "source": "goblinsfarm-2026-09-16-sky-wagon", "observation_file": "sky-wagon-observation-2026-09-17.json", "observation_sha256": "071c86cf0a3e96cc852c2932504acd7c3509b30ae34c9b771439e67e1a24e025"},
        {"content_id": "logger", "source": "supercell-2026-04-27-sound-of-clash", "observation_file": "logger-observation-2026-09-17.json", "observation_sha256": "eec2be5d6004bd8fe5034f93a6b13c88e70e883e03f3e2e4456f852d1d5f1602"},
        {"content_id": "electro_fangs", "source": "goblinsfarm-2026-08-26-electro-fangs", "observation_file": "electro-fangs-observation-2026-09-17.json", "observation_sha256": "ca45650953ee51a6491622ba4d54f0fb03fb2e2046917c631945b4c003d04287"},
        {"content_id": "monolith_arrow", "source": "goblinsfarm-2026-08-26-monolith-arrow", "observation_file": "monolith-arrow-observation-2026-09-17.json", "observation_sha256": "8f0c293082947f88e04703b4834b94ada94dd014624a06a5f71fb879ff259383"},
        {"content_id": "ruin_witch", "source": "goblinsfarm-2026-08-26-ruin-witch", "observation_file": "ruin-witch-observation-2026-09-17.json", "observation_sha256": "425343f5e219e9efe6e199fd5f59fd1cb4e57448896f76f41b74dba5987ee2a0"},
        {"content_id": "ruin_knight", "source": "cocwarbase-2026-09-17-ruin-witch", "observation_file": "ruin-knight-observation-2026-09-17.json", "observation_sha256": "098d665ffdcc57d89e8679da2ed060ef8fe79a2ba2a30b7d9607d50ccc037a09"},
    ]
    for item in supplemental:
        observation_path = REFERENCE.parent / item["observation_file"]
        assert hashlib.sha256(observation_path.read_bytes()).hexdigest() == item["observation_sha256"]
    sky_observation_path = REFERENCE.parent / next(item for item in supplemental
                                                   if item["content_id"] == "sky_wagon")["observation_file"]
    sky_observation = json.loads(sky_observation_path.read_text(encoding="utf-8"))["content"]
    assert [(level["hitpoints"], level["attack_count"], level["units_to_spawn"]["minion"])
            for level in sky_observation["levels"]] == [
                (3600, 4, 2), (3800, 4, 3), (4000, 5, 4), (4200, 5, 4)]
    sky_wagon = next(content for content in catalogue["contents"] if content["id"] == "sky_wagon")
    assert sky_wagon["support"] == "catalogued_only"
    assert sky_wagon["siege_slot"] is True and sky_wagon["housing_space"] is None
    assert sky_wagon["behavior"]["immune_to_spells"] is True
    logger_observation_path = REFERENCE.parent / next(item for item in supplemental
                                                      if item["content_id"] == "logger")["observation_file"]
    logger_observation = json.loads(logger_observation_path.read_text(encoding="utf-8"))["content"]
    assert [(level["dps"], level["range_tiles"]) for level in logger_observation["levels"]] == [
        (350, 7), (350, 7), (350, 7), (350, 7), (350, 8)]
    logger = next(content for content in catalogue["contents"] if content["id"] == "logger")
    assert logger["support"] == "catalogued_only"
    assert logger["behavior"]["projectile_pierces"] is True
    electro_fangs_observation_path = REFERENCE.parent / next(item for item in supplemental
                                                              if item["content_id"] == "electro_fangs")["observation_file"]
    electro_fangs_observation = json.loads(electro_fangs_observation_path.read_text(encoding="utf-8"))["content"]
    assert [level["hero_hitpoints_bonus"] for level in electro_fangs_observation["levels"]] == [
        400, 650, 850, 1000, 1150, 1300, 1450, 1600, 1750, 1900, 2000, 2100,
        2200, 2275, 2325, 2375, 2400, 2400]
    electro_fangs = next(content for content in catalogue["contents"] if content["id"] == "electro_fangs")
    assert electro_fangs["support"] == "catalogued_only"
    assert electro_fangs["behavior"]["damage_decay_per_jump_percent"] == 20
    monolith_arrow_observation_path = REFERENCE.parent / next(item for item in supplemental
                                                               if item["content_id"] == "monolith_arrow")["observation_file"]
    monolith_arrow_observation = json.loads(monolith_arrow_observation_path.read_text(encoding="utf-8"))["content"]
    assert [level["hero_hitpoints_bonus"] for level in monolith_arrow_observation["levels"]] == [
        100, 140, 180, 220, 260, 300, 340, 380, 420, 460, 500, 540, 580,
        620, 680, 700, 740, 780, 820, 860, 900, 940, 980, 1020, 1080, 1120, 1160]
    monolith_arrow = next(content for content in catalogue["contents"] if content["id"] == "monolith_arrow")
    assert monolith_arrow["support"] == "catalogued_only"
    assert monolith_arrow["behavior"]["damage_percent_by_deployed_housing_space"][0] == {"maximum": 180, "percent": 14}
    ruin_witch_observation_path = REFERENCE.parent / next(item for item in supplemental
                                                           if item["content_id"] == "ruin_witch")["observation_file"]
    ruin_witch_observation = json.loads(ruin_witch_observation_path.read_text(encoding="utf-8"))["content"]
    assert [(level["hitpoints"], level["summoned_ruin_knight_level"])
            for level in ruin_witch_observation["levels"]] == [(2300, 1), (2550, 2), (2800, 3), (3050, 4)]
    ruin_witch = next(content for content in catalogue["contents"] if content["id"] == "ruin_witch")
    assert ruin_witch["behavior"]["max_summoned_ruin_knights"] == 10
    assert ruin_witch["field_provenance"]["behavior"]["source"] == "supercell-2026-08-30-update"
    ruin_knight_observation_path = REFERENCE.parent / next(item for item in supplemental
                                                            if item["content_id"] == "ruin_knight")["observation_file"]
    ruin_knight_observation = json.loads(ruin_knight_observation_path.read_text(encoding="utf-8"))["content"]
    assert [(level["hitpoints"], level["damage_to_walls"])
            for level in ruin_knight_observation["levels"]] == [(3100, 765), (3400, 855), (3700, 945), (4000, 1035)]
    ruin_knight = next(content for content in catalogue["contents"] if content["id"] == "ruin_knight")
    assert ruin_knight["deployable"] is False and ruin_knight["wall_damage_multiplier"] == 3
    assert ruin_knight["hitpoint_decay_per_second"] == 0
    assert ruin_knight["field_provenance"]["hitpoint_decay_per_second"]["source"] == "supercell-2026-08-30-update"
    supplemental_spells = reference["catalogue_supplemental_spells"]
    assert supplemental_spells == [
        {"spell_id": "angry", "source": "cocmap-2026-09-17-angry-spell",
         "observation_file": "angry-spell-observation-2026-09-17.json",
         "observation_sha256": "bd5c78da20af9b746a897028119b413490f7a754e6ce4827a295814153a8ae15"},
    ]
    angry_observation_path = REFERENCE.parent / supplemental_spells[0]["observation_file"]
    assert hashlib.sha256(angry_observation_path.read_bytes()).hexdigest() == supplemental_spells[0]["observation_sha256"]
    angry_observation = json.loads(angry_observation_path.read_text(encoding="utf-8"))["spell"]
    assert angry_observation["behavior_provenance"] == {"source": "supercell-2026-06-15-anime-fury", "tier": "primary"}
    assert [(level["anger_duration_seconds"], level["research_cost"], level["laboratory_required"])
            for level in angry_observation["levels"]] == [
                (7, None, None), (8, 150000, 14), (10, 250000, 15), (12, 400000, 16)]
    angry = next(spell for spell in catalogue["spells"] if spell["id"] == "angry")
    assert angry["support"] == "catalogued_only"
    assert angry["behavior"]["siege_machines_immune"] is True
    assert [level["anger_duration_seconds"] for level in angry["levels"]] == [7, 8, 10, 12]
    wall_breaker_observation_path = REFERENCE.parent / wall_breaker_observation["observation_file"]
    assert hashlib.sha256(wall_breaker_observation_path.read_bytes()).hexdigest() == wall_breaker_observation["observation_sha256"]
    wall_breaker_table = json.loads(wall_breaker_observation_path.read_text(encoding="utf-8"))
    assert wall_breaker_table["source_id"] == wall_breaker_observation["source"]
    assert wall_breaker_table["observed_root"]["attack_speed_seconds"] == 1
    giant_observation_path = REFERENCE.parent / giant_observation["observation_file"]
    assert hashlib.sha256(giant_observation_path.read_bytes()).hexdigest() == giant_observation["observation_sha256"]
    giant_table = json.loads(giant_observation_path.read_text(encoding="utf-8"))
    assert giant_table["source_id"] == giant_observation["source"]
    assert giant_table["behavior"] == "Prefer defenses; after all defenses are destroyed, revert to nearest-target behavior."
    sound_artifact = reference["secondary_level_observation_artifacts"][0]
    sound_path = REFERENCE.parent / sound_artifact["observation_file"]
    assert hashlib.sha256(sound_path.read_bytes()).hexdigest() == sound_artifact["observation_sha256"]
    sound_levels = json.loads(sound_path.read_text(encoding="utf-8"))["observations"]
    sound_by_key = {(item["content_id"], item["level"]): item for item in sound_levels}
    assert sound_by_key[("barbarian", 13)]["fields"] == {"hitpoints": 310, "dps": 51, "upgrade_cost": 24000000, "upgrade_cost_resource": "Elixir", "upgrade_time_seconds": 1080000, "town_hall_required": 18}
    assert sound_by_key[("x_bow", 13)]["fields"]["dps"] == 245
    assert sound_by_key[("giant_bomb", 12)]["fields"]["damage_radius_tiles"] == 4.2
    assert "golemite_spawn_and_death_damage" in sound_by_key[("golem", 15)]["known_missing"]
    overrides = {(item["content_id"], item["level"]): item for item in reference["catalogue_overrides"]}
    rider = overrides[("dragon_rider", 6)]["fields"]
    assert rider["hitpoints"] == {"value": 6000, "source": "supercell-2026-05-22-balance"}
    assert rider["dps"] == {"value": 510, "source": "supercell-2026-05-22-balance"}
    assert rider["damage_per_shot"]["source"] == "derived"
    assert overrides[("giant", 1)]["fields"]["town_hall_required"] == {
        "value": 2, "source": "goblinsfarm-2026-09-16-giant",
        "note": "Current-build table reports Town Hall 2; the April raw snapshot reports 1."}
    assert overrides[("cannon", 2)]["fields"]["town_hall_required"] == {
        "value": 2, "source": "goblinsfarm-2026-08-26-cannon",
        "note": "Dated Home Village table reports Town Hall 2; the April raw snapshot reports 1."}
    for level, hp, dps, damage in ((1, 6200, 95, 209), (2, 6350, 105, 231), (3, 6500, 115, 253)):
        root_rider = overrides[("root_rider", level)]["fields"]
        assert root_rider["hitpoints"] == {"value": hp, "source": "supercell-2024-06-03-root-rider-balance"}
        assert root_rider["dps"] == {"value": dps, "source": "supercell-2024-06-03-root-rider-balance"}
        assert root_rider["damage_per_shot"]["value"] == damage
    root_rider_extra = next(item for item in reference["catalogue_extra_levels"]
                            if item["content_id"] == "root_rider" and item["level"] == 4)
    assert root_rider_extra["source"] == "goblinsfarm-2026-09-16-root-rider"
    root_rider_observation_path = REFERENCE.parent / root_rider_extra["observation_file"]
    assert hashlib.sha256(root_rider_observation_path.read_bytes()).hexdigest() == root_rider_extra["observation_sha256"]
    root_rider_observation = json.loads(root_rider_observation_path.read_text(encoding="utf-8"))["observations"][0]
    assert root_rider_observation["fields"] == {"hitpoints": 6700, "dps": 125, "upgrade_cost": 30000000, "upgrade_cost_resource": "Elixir", "upgrade_time_seconds": 1382400, "town_hall_required": 18}
    assert root_rider_observation["derived_fields"]["damage_per_shot"]["value"] == 275
    assert overrides[("archer", 1)]["fields"]["town_hall_required"] == {
        "value": 2, "source": "goblinsfarm-2026-08-26-archer",
        "note": "Dated table reports Town Hall 2; the April raw snapshot reports 1."}
    content_overrides = {item["content_id"]: item["fields"] for item in reference["catalogue_content_overrides"]}
    assert content_overrides["mortar"]["min_range_tiles"] == {
        "value": 4, "source": "fandom-2026-09-17-mortar",
        "note": "Community source reports the Home Village range interval as 4–11 tiles; the immutable April payload has no minRange field."}
    assert content_overrides["inferno_tower"]["behavior"] == {
        "value": {"single_target_ramp": {"stage_two_after_seconds": 1.5,
                                           "stage_three_after_seconds": 5.25,
                                           "reset_on_lock_loss": True}},
        "source": "fandom-2026-09-17-inferno-tower",
        "note": "Stage damage values remain the separately pinned normal-mode dps_initial, dps_after_1p5_seconds and dps fields. This source supplies only the three-stage timing and reset-on-lost-lock contract."}
    assert content_overrides["bomb_tower"] == {
        "death_splash_radius_tiles": {"value": 2.75, "source": "fandom-2026-09-17-bomb-tower",
                                       "note": "The published Home Village behavior specifies a 2.75-tile destruction-explosion radius."},
        "death_damage_delay_seconds": {"value": 1, "source": "fandom-2026-09-17-bomb-tower",
                                         "note": "The destruction bomb is primed and detonates one second later."},
        "death_damage_ground_only": {"value": True, "source": "fandom-2026-09-17-bomb-tower",
                                       "note": "The delayed destruction explosion affects ground units only."}}
    assert content_overrides["hidden_tesla"]["reveal_destruction_percent"] == {
        "value": 51, "source": "fandom-2026-09-17-hidden-tesla",
        "note": "Community behavior documentation says all concealed Home Village Hidden Teslas reveal once destruction reaches 51 percent."}
    assert content_overrides["seeking_air_mine"]["projectile_speed_tiles_per_second"] == {
        "value": 3.5, "source": "supercell-2021-08-11-seeking-air-mine",
        "note": "Official maintenance note changed the projectile speed to 3.5 tiles per second. Launch, collision and cancellation ordering remain separately undocumented."}
    assert content_overrides["dragon_rider"]["target_type"] == {
        "value": "both", "source": "supercell-2021-06-13-dragon-rider",
        "note": "Official launch specification states that Dragon Rider targets Ground & Air; no later consulted official delta changes that target class."}
    assert content_overrides["root_rider"]["smashes_walls"]["source"] == "goblinsfarm-2026-09-16-root-rider"
    assert content_overrides["root_rider"]["wall_damage_per_hit"] == {
        "value": 4000, "source": "clashrecord-2026-09-17-root-rider",
        "note": "The structured current table reports 4,000 Wall damage per hit at every level."}
    assert content_overrides["root_rider"]["wall_attack_speed_seconds"]["value"] == 0.4
    assert content_overrides["electro_dragon"]["range_tiles"] == {
        "value": 2.5, "source": "goblinsfarm-2026-09-16-electro-dragon",
        "note": "The dated client-build table reports a 2.5-tile attack range; the historical frozen payload reported 3."}

    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    contents = {content["id"]: content for content in catalogue["contents"]}
    level = next(level for level in contents["dragon_rider"]["levels"] if level["level"] == 6)
    assert (level["hitpoints"], level["dps"], level["damage_per_shot"]) == (6000, 510, 612)
    provenance = level["provenance"]
    assert provenance["source"]["version"] == catalogue["source"]["commit"]
    assert provenance["source"]["raw_sha256"]
    assert provenance["overridden_fields"] == ["damage_per_shot", "dps", "hitpoints"]
    assert contents["dragon_rider"]["target_type"] == "both"
    assert contents["dragon_rider"]["field_provenance"]["target_type"]["source"] == "supercell-2021-06-13-dragon-rider"
    root_rider = contents["root_rider"]
    assert root_rider["target_focus"] == "defenses"
    assert root_rider["field_provenance"]["target_focus"]["source"] == "supercell-2023-12-09-root-rider"
    assert root_rider["smashes_walls"] is True
    assert root_rider["wall_damage_per_hit"] == 4000
    assert root_rider["wall_attack_speed_seconds"] == 0.4
    assert [(level["hitpoints"], level["dps"], level["damage_per_shot"])
            for level in root_rider["levels"]] == [(6200, 95, 209), (6350, 105, 231), (6500, 115, 253), (6700, 125, 275)]
    baby_dragon = contents["baby_dragon"]
    assert (baby_dragon["isolation_radius_tiles"], baby_dragon["rage_damage_multiplier"], baby_dragon["rage_attack_speed_multiplier"]) == (4.5, 2, 1.5)
    assert baby_dragon["field_provenance"]["isolation_radius_tiles"]["source"] == "fandom-2026-09-17-baby-dragon"
    assert baby_dragon["field_provenance"]["rage_attack_speed_multiplier"]["source"] == "chiefpansancolt-62b019df"
    miner = contents["miner"]
    miner12 = next(level for level in miner["levels"] if level["level"] == 12)
    assert (miner12["hitpoints"], miner12["damage_per_shot"], miner12["dps"], miner12["town_hall_required"]) == (2050, 331.5, 195, 18)
    assert miner["burrows"] is True
    assert miner["field_provenance"]["burrows"]["source"] == "fandom-2026-09-17-miner"
    electro_dragon = contents["electro_dragon"]
    electro_dragon8 = next(level for level in electro_dragon["levels"] if level["level"] == 8)
    electro_dragon9 = next(level for level in electro_dragon["levels"] if level["level"] == 9)
    assert (electro_dragon8["hitpoints"], electro_dragon8["damage_per_shot"], electro_dragon8["dps"], electro_dragon8["town_hall_required"]) == (6400, 1662.5, 475, 17)
    assert (electro_dragon9["hitpoints"], electro_dragon9["damage_per_shot"], electro_dragon9["dps"], electro_dragon9["death_damage"], electro_dragon9["town_hall_required"]) == (6900, 1785, 510, 145, 18)
    assert electro_dragon["range_tiles"] == 2.5
    assert electro_dragon9["provenance"]["overridden_fields"] == ["damage_per_shot", "dps", "hitpoints"]
    yeti_extra = next(item for item in reference["catalogue_extra_levels"]
                      if item["content_id"] == "yeti" and item["level"] == 8)
    assert yeti_extra["source"] == "goblinsfarm-2026-09-16-yeti"
    yeti_observation_path = REFERENCE.parent / yeti_extra["observation_file"]
    assert hashlib.sha256(yeti_observation_path.read_bytes()).hexdigest() == yeti_extra["observation_sha256"]
    yeti = contents["yeti"]
    yeti8 = next(level for level in yeti["levels"] if level["level"] == 8)
    assert (yeti8["hitpoints"], yeti8["damage_per_shot"], yeti8["dps"], yeti8["town_hall_required"]) == (4650, 380, 380, 18)
    assert [(level["level"], level["spawned_units"]) for level in yeti["levels"]] == [(1, 8), (2, 9), (3, 10), (4, 11), (5, 12), (6, 13), (7, 13), (8, 14)]
    assert yeti8["laboratory_required"] == 16
    assert contents["goblin"]["resource_damage_multiplier"] == 2
    assert contents["goblin"]["field_provenance"]["resource_damage_multiplier"]["source"] == "fandom-2026-09-17-goblin"
    assert contents["seeking_air_mine"]["projectile_speed_tiles_per_second"] == 3.5
    assert contents["seeking_air_mine"]["field_provenance"]["projectile_speed_tiles_per_second"]["source"] == "supercell-2021-08-11-seeking-air-mine"
    assert contents["balloon"]["death_splash_radius_tiles"] == 1.2
    assert contents["balloon"]["death_damage_ground_only"] is True
    assert contents["golem"]["death_splash_radius_tiles"] == 1.5
    wall_breaker = contents["wall_breaker"]
    assert (wall_breaker["wall_damage_multiplier"], wall_breaker["splash_radius_tiles"], wall_breaker["death_splash_radius_tiles"], wall_breaker["self_destruct_on_attack"]) == (40, 2, 2, True)
    assert wall_breaker["field_provenance"]["wall_damage_multiplier"]["source"] == "fandom-2026-09-17-wall-breaker"
    assert {field: wall_breaker[field] for field in wall_breaker_table["observed_root"]} == wall_breaker_table["observed_root"]
    for observed_level in wall_breaker_table["levels"]:
        imported_level = next(level for level in wall_breaker["levels"] if level["level"] == observed_level["level"])
        assert {field: imported_level[field] for field in observed_level if field != "level"} == {
            field: observed_level[field] for field in observed_level if field != "level"}
    giant = contents["giant"]
    assert {field: giant[field] for field in giant_table["observed_root"]} == giant_table["observed_root"]
    assert next(level for level in giant["levels"] if level["level"] == 1)["provenance"]["overridden_fields"] == ["town_hall_required"]
    for observed_level in giant_table["levels"]:
        imported_level = next(level for level in giant["levels"] if level["level"] == observed_level["level"])
        assert {field: imported_level[field] for field in observed_level if field != "level"} == {
            field: observed_level[field] for field in observed_level if field != "level"}
    for content_id in ("balloon", "wizard", "cannon", "archer_tower", "mortar", "air_defense", "wizard_tower", "bomb_tower"):
        observation = observations[content_id]
        observation_path = REFERENCE.parent / observation["observation_file"]
        assert hashlib.sha256(observation_path.read_bytes()).hexdigest() == observation["observation_sha256"]
        table = json.loads(observation_path.read_text(encoding="utf-8"))
        assert table["source_id"] == observation["source"]
        imported_content = contents[content_id]
        assert {field: imported_content[field] for field in table["observed_root"]} == table["observed_root"]
        for observed_level in table["levels"]:
            imported_level = next(level for level in imported_content["levels"] if level["level"] == observed_level["level"] and level.get("variant", "normal") == "normal")
            assert {field: imported_level[field] for field in observed_level if field != "level"} == {
                field: observed_level[field] for field in observed_level if field != "level"}
        if content_id == "cannon":
            assert imported_content["available_per_town_hall"][-1] == {"townHallLevel": 18, "count": table["available_at_town_hall_18"], "countAfterMerges": 0}
            assert next(level for level in imported_content["levels"] if level["level"] == 2)["provenance"]["overridden_fields"] == ["town_hall_required"]
        if content_id == "archer_tower":
            assert imported_content["available_per_town_hall"][-1]["count"] == table["available_at_town_hall_18"]
        if content_id == "mortar":
            assert table["documented_behavior"] == "Cannot target units standing directly beside it; the publisher does not provide a numeric inner-radius value."
            assert imported_content["available_per_town_hall"][-1]["count"] == table["available_at_town_hall_18"]
            assert imported_content["min_range_tiles"] == 4
            assert imported_content["field_provenance"]["min_range_tiles"]["source"] == "fandom-2026-09-17-mortar"
        if content_id == "air_defense":
            assert imported_content["available_per_town_hall"][-1]["count"] == table["available_at_town_hall_18"]
        if content_id == "wizard_tower":
            assert imported_content["available_per_town_hall"][-1]["count"] == table["available_at_town_hall_18"]
        if content_id == "bomb_tower":
            assert table["documented_behavior"].startswith("Detonates when destroyed")
            assert imported_content["available_per_town_hall"][-1]["count"] == table["available_at_town_hall_18"]
    earthquake = next(spell for spell in catalogue["spells"] if spell["id"] == "earthquake")
    earthquake_level = next(level for level in earthquake["levels"] if level["level"] == 8)
    assert earthquake_level["provenance"]["source"]["version"] == catalogue["source"]["commit"]
    assert earthquake_level["provenance"]["overridden_fields"] == []
    inferno = contents["inferno_tower"]
    inferno_l12 = next(level for level in inferno["levels"] if level["level"] == 12)
    assert inferno["behavior"]["single_target_ramp"] == {"stage_two_after_seconds": 1.5,
                                                           "stage_three_after_seconds": 5.25,
                                                           "reset_on_lock_loss": True}
    assert inferno["field_provenance"]["behavior"]["source"] == "fandom-2026-09-17-inferno-tower"
    assert inferno_l12["mode_stats"]["normal"] == {"damage_per_shot": 422.4, "damage_per_shot_min": None, "splash_damage_max": None, "splash_damage_min": None, "dps": 3300, "dps_initial": 155, "dps_after_1p5_seconds": 330, "number_of_targets": None}
    assert inferno_l12["mode_stats"]["multiTarget"] == {"damage_per_shot": 19.84, "damage_per_shot_min": None, "splash_damage_max": None, "splash_damage_min": None, "dps": 155, "dps_initial": None, "dps_after_1p5_seconds": None, "number_of_targets": 6}
    bomb_tower = contents["bomb_tower"]
    bomb_tower1 = next(level for level in bomb_tower["levels"] if level["level"] == 1 and level["variant"] == "normal")
    bomb_tower13 = next(level for level in bomb_tower["levels"] if level["level"] == 13 and level["variant"] == "normal")
    assert (bomb_tower1["death_damage"], bomb_tower13["death_damage"]) == (150, 700)
    assert bomb_tower1["provenance"]["overridden_fields"] == ["death_damage"]
    assert bomb_tower13["provenance"]["overridden_fields"] == ["death_damage"]
    assert (bomb_tower["death_splash_radius_tiles"], bomb_tower["death_damage_delay_seconds"], bomb_tower["death_damage_ground_only"]) == (2.75, 1, True)
    assert bomb_tower["field_provenance"]["death_damage_delay_seconds"]["source"] == "fandom-2026-09-17-bomb-tower"
    spells = {spell["id"]: spell for spell in catalogue["spells"]}
    for audit in reference["catalogue_level_audit"]:
        entries = spells if audit["family"] == "spell" else contents
        assert audit["id"] in entries
        imported_levels = {level["level"] for level in entries[audit["id"]]["levels"]}
        assert set(audit["levels"]) <= imported_levels
    dark_barracks = next(content for content in catalogue["contents"] if content["id"] == "dark_barracks")
    hero_banner_content = contents["hero_banner"]
    assert hero_banner_content["levels"] == []
    assert hero_banner_content["targetable"] is False and hero_banner_content["counts_toward_destruction"] is False and hero_banner_content["deployment_margin_tiles"] == 1
    assert hero_banner_content["image"] == "images/home/army-buildings/hero-banner/empty.png"
    assert set(hero_banner_content["field_provenance"]) == {"targetable", "counts_toward_destruction", "deployment_margin_tiles", "image", "source_images", "max_placements"}
    assert set(hero_banner_content["source_images"]) == {"normal", "barbarian_king", "archer_queen", "grand_warden", "royal_champion", "minion_prince", "dragon_duke"}
    assert hero_banner_content["max_placements"] == 4
    barracks_l13 = next(level for level in dark_barracks["levels"] if level["level"] == 13)
    assert barracks_l13["hitpoints"] == 1100
    assert barracks_l13["provenance"]["source"]["source"] == "goblinsfarm-2026-08-26-dark-barracks"
    dark_factory = next(content for content in catalogue["contents"] if content["id"] == "dark_spell_factory")
    factory_l8 = next(level for level in dark_factory["levels"] if level["level"] == 8)
    assert factory_l8["hitpoints"] == 1070
    assert factory_l8["provenance"]["source"]["source"] == "goblinsfarm-2026-08-26-dark-spell-factory"
    for content_id, level_number, hitpoints, source_id in (
        ("army_camp", 14, 950, "goblinsfarm-2026-08-26-army-camp"),
        ("workshop", 9, 1800, "goblinsfarm-2026-08-26-workshop"),
        ("blacksmith", 10, 1600, "goblinsfarm-2026-08-26-blacksmith"),
    ):
        imported = next(level for level in contents[content_id]["levels"] if level["level"] == level_number)
        assert imported["hitpoints"] == hitpoints
        assert imported["provenance"]["source"]["source"] == source_id
    for content_id, level_number, hitpoints, dps, source_id in (
        ("barbarian", 13, 310, 51, "goblinsfarm-2026-08-26-barbarian"),
        ("valkyrie", 12, 2900, 255, "goblinsfarm-2026-08-26-valkyrie"),
        ("dragon", 13, 6000, 430, "goblinsfarm-2026-08-26-dragon"),
    ):
        imported = next(level for level in contents[content_id]["levels"] if level["level"] == level_number)
        assert (imported["hitpoints"], imported["dps"]) == (hitpoints, dps)
        assert imported["provenance"]["source"]["source"] == source_id
    xbow_l13 = next(level for level in contents["x_bow"]["levels"] if level["level"] == 13)
    assert (xbow_l13["hitpoints"], xbow_l13["dps"], xbow_l13["damage_per_shot"]) == (5000, 245, 31.36)
    assert xbow_l13["provenance"]["source"]["source"] == "goblinsfarm-2026-08-26-x-bow"
    goblin_l10 = next(level for level in contents["goblin"]["levels"] if level["level"] == 10)
    assert (goblin_l10["hitpoints"], goblin_l10["dps"], goblin_l10["laboratory_required"]) == (166, 82, 16)
    assert goblin_l10["provenance"]["source"]["source"] == "fandom-2026-09-17-goblin"
    balloon_l13 = next(level for level in contents["balloon"]["levels"] if level["level"] == 13)
    assert (balloon_l13["hitpoints"], balloon_l13["dps"], balloon_l13["death_damage"], balloon_l13["laboratory_required"]) == (1360, 326, 425, 16)
    assert balloon_l13["provenance"]["source"]["source"] == "fandom-2026-09-17-balloon"
    golem_l15 = next(level for level in contents["golem"]["levels"] if level["level"] == 15)
    assert (golem_l15["hitpoints"], golem_l15["dps"], golem_l15["death_damage"], golem_l15["laboratory_required"]) == (10600, 120, 1050, 16)
    assert golem_l15["provenance"]["source"]["source"] == "fandom-2026-09-17-golem"
    golemite = contents["golemite"]
    assert golemite["support"] == "spawned_only"
    golemite_l15 = next(level for level in golemite["levels"] if level["level"] == 15)
    assert (golemite_l15["hitpoints"], golemite_l15["damage_per_shot"], golemite_l15["dps"], golemite_l15["death_damage"], golemite_l15["parent_spawn_count"]) == (2040, 66, 22, 210, 4)
    assert golemite_l15["provenance"]["source"]["source"] == "fandom-2026-09-17-golem"
    ground_skeleton_content = contents["trap_skeleton_ground"]
    air_skeleton_content = contents["trap_skeleton_air"]
    assert ground_skeleton_content["support"] == air_skeleton_content["support"] == "spawned_only"
    assert ground_skeleton_content["attack_speed_seconds"] == air_skeleton_content["attack_speed_seconds"] == 0.7
    assert ground_skeleton_content["known_missing"] == ["housing space", "lifetime", "spawn geometry", "retargeting and distraction rules"]
    assert air_skeleton_content["field_provenance"]["housing_space"]["source"] == "supercell-2025-03-24-clash-anytime"
    ground_skeleton_l2 = next(level for level in ground_skeleton_content["levels"] if level["level"] == 2)
    air_skeleton_l2 = next(level for level in air_skeleton_content["levels"] if level["level"] == 2)
    assert (ground_skeleton_l2["hitpoints"], ground_skeleton_l2["damage_per_shot"], ground_skeleton_l2["dps"]) == (45, 21, 30)
    assert (air_skeleton_l2["hitpoints"], air_skeleton_l2["damage_per_shot"], air_skeleton_l2["dps"]) == (45, 21, 30)
    skeleton_trap = contents["skeleton_trap"]
    skeleton_l5 = next(level for level in skeleton_trap["levels"] if level["level"] == 5)
    assert (skeleton_l5["spawned_units"], skeleton_l5["spawned_unit_level"]) == (5, 2)
    tornado = contents["tornado_trap"]
    assert next(level for level in tornado["levels"] if level["level"] == 3)["effect_duration_seconds"] == 7
    giga = contents["giga_bomb"]
    assert giga["activation_housing_space"] == 18
    assert next(level for level in giga["levels"] if level["level"] == 4)["damage"] == 1400
    giant_l12 = next(level for level in contents["giant_bomb"]["levels"] if level["level"] == 12)
    assert (giant_l12["damage"], giant_l12["damage_radius_tiles"]) == (490, 4.2)
    assert giant_l12["provenance"]["source"]["source"] == "goblinsfarm-2026-08-26-giant-bomb"
    print("TH18 reference validation passed")


if __name__ == "__main__":
    main()
