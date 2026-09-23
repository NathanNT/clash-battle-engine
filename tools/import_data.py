#!/usr/bin/env python3
"""Pinned, explicit importer for public CoC data.

It never follows a branch: --commit must be a 40-character Git SHA.  Raw input
is cached separately and the normalized catalogue carries provenance and SHA256.
"""
from __future__ import annotations
import argparse, datetime as dt, hashlib, json, pathlib, urllib.request

from importer.reference import (
    BASE,
    CONTENT_OVERRIDES,
    CURRENT_LEVEL_OVERRIDES,
    DEATH_SPLASH_RADIUS_TILES,
    EXTRA_LEVELS,
    FILES,
    FLYING,
    FROZEN_REFERENCE,
    HEALERS,
    SPELLS,
    SPLASH_RADIUS_TILES,
    SUPPLEMENTAL_CONTENTS,
    SUPPLEMENTAL_SPELLS,
    TARGET_FOCUS,
    apply_content_overrides,
    non_combat_content,
)

def get(url: str) -> bytes:
    with urllib.request.urlopen(url, timeout=30) as response:
        return response.read()

def duration_seconds(value: object) -> int:
    """Convert the source's structured duration without rounding it."""
    if not isinstance(value, dict):
        return 0
    return (int(value.get("days", 0)) * 86_400 + int(value.get("hours", 0)) * 3_600
            + int(value.get("minutes", 0)) * 60 + int(value.get("seconds", 0)))

def normalized_level(level: dict, fallback_image: str | None = None,
                     *, pet: bool = False) -> dict:
    """Keep every scalar level attribute used by the supported home entities."""
    normal = level.get("stats", {}).get("normal", {})
    storage = level.get("storageCapacity") or {}
    # Troops use research*, whereas buildings use build*.  The public form
    # deliberately uses one upgrade vocabulary while retaining the operation.
    is_research = "researchCost" in level
    prefix = "research" if is_research else "build"
    mode_stats = {}
    for mode_id, mode in level.get("stats", {}).items():
        if not isinstance(mode, dict):
            continue
        mode_stats[mode_id] = {
            "damage_per_shot": mode.get("damagePerShot"),
            "damage_per_shot_min": mode.get("damagePerShotMin"),
            "splash_damage_max": mode.get("splashDamageMax"),
            "splash_damage_min": mode.get("splashDamageMin"),
            "dps": mode.get("dps"),
            "dps_initial": mode.get("dpsInitial"),
            "dps_after_1p5_seconds": mode.get("dpsAfter1p5s"),
            "number_of_targets": mode.get("numberOfTargets"),
        }
    return {
        "level": level["level"],
        "variant": "supercharged" if level.get("supercharge") else "normal",
        "hitpoints": level.get("hitpoints"),
        # Pets keep their attack values directly on each level rather than in
        # stats.normal. Preserve those raw values even while the paired-Pet
        # combat lifecycle remains deliberately unsupported by Core.
        "damage_per_shot": level.get("damagePerHit") if pet else normal.get("damagePerShot"),
        **({"damage_vs_walls": level["damageVsWalls"]} if pet and "damageVsWalls" in level else {}),
        # Traps store their one-shot payload directly as `damage`; preserve it
        # separately rather than fabricating an attack stat.
        "damage": level.get("damage"),
        "spring_capacity": level.get("springCapacity"),
        "spawned_units": level.get("spawnedUnits"),
        "spawned_unit_level": level.get("skeletonLevel"),
        "effect_duration_seconds": level.get("duration"),
        "damage_radius_tiles": level.get("damageRadius"),
        "dps": level.get("damagePerSecond") if pet else normal.get("dps"),
        "healing_per_second": level.get("healingPerSecond") if pet else normal.get("healingPerSecond"),
        **({"healing_per_pulse": level["healingPerPulse"]} if pet and "healingPerPulse" in level else {}),
        **({"frostmites_per_summon": level["frostmitesPerSummon"]} if pet and "frostmitesPerSummon" in level else {}),
        **({"max_frostmites": level["maxFrostmites"]} if pet and "maxFrostmites" in level else {}),
        **({"poison_max_dps": level["poisonMaxDps"]} if pet and "poisonMaxDps" in level else {}),
        **({"poison_speed_decrease_percent": level["poisonSpeedDecreasePercent"]} if pet and "poisonSpeedDecreasePercent" in level else {}),
        **({"poison_attack_rate_decrease_percent": level["poisonAttackRateDecreasePercent"]} if pet and "poisonAttackRateDecreasePercent" in level else {}),
        **({"revive_duration_seconds": level["reviveDuration"]} if pet and "reviveDuration" in level else {}),
        **({"invisibility_duration_seconds": level["invisibilityDuration"]} if pet and "invisibilityDuration" in level else {}),
        **({"brainwash_duration_seconds": level["brainwashDuration"]} if pet and "brainwashDuration" in level else {}),
        **({"dps_on_resource_buildings": level["dpsOnResourceBuildings"]} if pet and "dpsOnResourceBuildings" in level else {}),
        "death_damage": normal.get("deathDamage"),
        # Hero source documents publish a stable content icon rather than an
        # authored per-level battlefield sprite. Preserve that exact public
        # path as a visual fallback instead of inventing a `level-N.png` name.
        "image": level.get("images", {}).get("normal") or fallback_image,
        "upgrade_operation": "research" if is_research else "build",
        "upgrade_cost": level.get(f"{prefix}Cost"),
        "upgrade_cost_resource": level.get(f"{prefix}CostResource"),
        "upgrade_time_seconds": duration_seconds(level.get(f"{prefix}Time")),
        "town_hall_required": level.get("townHallRequired"),
        "laboratory_required": level.get("laboratoryRequired"),
        "xp_gained": level.get("xpGained"),
        "capacity": level.get("capacity"),
        "production_rate": level.get("productionRate"),
        "max_buildings": level.get("maxBuildings"),
        "max_traps": level.get("maxTraps"),
        "storage_capacity": storage or None,
        "wall_rings": level.get("wallRings"),
        # The Town Hall weapon has its own per-weapon levels. Preserve the
        # source object verbatim so no current data are silently discarded.
        "weapon": level.get("weapon"),
        # Preserve source-declared mode values even when the current engine has
        # not implemented their particular targeting/ramp mechanics yet.
        "mode_stats": mode_stats,
    }

def normalized_spell_level(level: dict) -> dict:
    return {"level": level["level"], "radius_tiles": level.get("radius"), "damage": level.get("damage"), "total_healing": level.get("totalHealing"), "healing_per_pulse": level.get("healingPerPulse"), "building_damage_percent": level.get("buildingDamagePercent"), "troop_damage_percent": level.get("troopDamagePercent"), "damage_increase": level.get("damageIncrease"), "speed_increase": level.get("speedIncrease"), "spell_duration_seconds": level.get("spellDuration"), "town_hall_required": level.get("townHallRequired"), "laboratory_required": level.get("laboratoryRequired"), "research_cost": level.get("researchCost"), "research_cost_resource": level.get("researchCostResource"), "research_time_seconds": duration_seconds(level.get("researchTime"))}

def normalized_modes(doc: dict) -> list[dict]:
    """Retain every public weapon mode instead of flattening it to `normal`.

    The X-Bow source labels its alternate mode `airAndGround` and describes the
    normal mode as ground-only. This mapping is therefore an explicit
    interpretation of source-provided mode names/descriptions, not a guessed
    numerical stat. Other contents keep their declared document target type.
    """
    source_modes = doc.get("modes", {})
    has_air_and_ground = "airAndGround" in source_modes
    result = []
    for mode_id, mode in source_modes.items():
        target_type = doc.get("targetType")
        if has_air_and_ground and mode_id == "normal":
            target_type = "ground"
        elif mode_id == "airAndGround":
            target_type = "both"
        result.append({"id": mode_id, "range_tiles": mode.get("range", doc.get("range")), "attack_speed_seconds": mode.get("attackSpeed", doc.get("attackSpeed")), "shots_per_burst": mode.get("shotsPerBurst", 1), "time_between_bursts_seconds": mode.get("timeBetweenBursts", 0), "number_of_targets": mode.get("numberOfTargets", 0), "target_type": target_type, "damage_type": mode.get("damageType", doc.get("damageType")),
                       "spell_radius_tiles": mode.get("spellRadius"), "spell_duration_seconds": mode.get("spellDuration"), "damage_increase_percent": mode.get("damageIncrease"), "max_damage_per_second": mode.get("maxDps"), "movement_speed_decrease_percent": mode.get("speedDecrease"), "attack_rate_decrease_percent": mode.get("attackRateDecrease"), "troop_damage_percent": mode.get("troopDamagePercent"), "recharge_time_seconds": mode.get("rechargeTime")})
    return result

def normalized_opaque_level(level: dict) -> dict:
    """Preserve an auxiliary schema verbatim without pretending it is troop DPS."""
    return {
        "level": level["level"], "variant": "normal",
        "image": level.get("images", {}).get("normal"),
        "source_fields": {key: value for key, value in level.items()
                          if key not in {"level", "images"}},
    }

def normalized_hero_equipment(identity: str, source_id: str, doc: dict,
                              source_provenance: dict) -> dict:
    """Store equipment data without assigning it an invented standalone actor."""
    levels = [normalized_opaque_level(level) for level in doc.get("levels", [])
              if "level" in level]
    for level in levels:
        override = CURRENT_LEVEL_OVERRIDES.get(
            (identity, level["level"], level.get("variant", "normal")), {})
        level.update(override)
        level["provenance"] = {"level": level["level"], "source": source_provenance,
                               "overridden_fields": sorted(override)}
    return {
        "id": identity, "source_id": source_id, "category": "hero-equipment",
        "source_fields": {key: value for key, value in doc.items()
                          if key not in {"id", "dataId", "name", "description", "base", "category", "images", "levels"}},
        "levels": levels, "support": "catalogued_only",
    }

def normalized_super_troop(parent_id: str, parent_doc: dict, super_doc: dict,
                           source_provenance: dict) -> dict:
    """Extract a Super Troop variant embedded in its immutable parent payload.

    The source models Super Troops under ``superTroop`` rather than as separate
    files.  Preserve that relationship instead of fabricating a second upstream
    URL or treating a boost as a normal research upgrade.  Targeting, footprint
    and flight are inherited only where the nested source does not repeat them;
    the explicit provenance marker prevents that inheritance from being read as
    an independently verified current-version behavior contract.
    """
    identity = super_doc["id"].replace("-", "_")
    levels = [normalized_level(level) for level in super_doc.get("levels", [])
              if "level" in level]
    for level in levels:
        # Super Troops are nested under their parent source document. Apply
        # additive per-level corrections using the Super Troop identity (not
        # the parent identity) exactly as ordinary entries do, and retain the
        # corrected-field provenance instead of silently leaving a dated TH18
        # observation unable to affect normalized GameData.
        override = CURRENT_LEVEL_OVERRIDES.get((identity, level["level"], level.get("variant", "normal")), {})
        level.update(override)
        level["upgrade_operation"] = "not_applicable"
        level["provenance"] = {
            "level": level["level"],
            "source": {**source_provenance, "source_path": "superTroop.levels"},
            "overridden_fields": sorted(override),
        }
    # Super Troops are separate normalized content ids even though their raw
    # payload is nested under a parent troop.  Apply hash-pinned missing-level
    # supplements here too; limiting EXTRA_LEVELS to ordinary top-level docs
    # silently discarded a current Super Troop row on each regeneration.
    imported_numbers = {(level["level"], level.get("variant", "normal")) for level in levels}
    for level in EXTRA_LEVELS.get(identity, []):
        key = (level["level"], level.get("variant", "normal"))
        if key in imported_numbers:
            raise RuntimeError(f"supplement duplicates raw Super Troop level: {identity} L{level['level']} ({key[1]})")
        override = CURRENT_LEVEL_OVERRIDES.get((identity, level["level"], level.get("variant", "normal")), {})
        level.update(override)
        provenance = level.setdefault("provenance", {})
        provenance["overridden_fields"] = sorted(
            set(provenance.get("overridden_fields", [])) | set(override)
        )
        levels.append(level)
    levels.sort(key=lambda level: level["level"])
    inherited = {"source": {**source_provenance, "source_path": "parent troop"},
                 "note": "Nested superTroop payload omits this field; retained from its source parent, not independently verified."}
    return {
        "id": identity,
        "source_id": super_doc["id"],
        "category": "troop",
        "variant_of": parent_id,
        "source_path": "superTroop",
        "target_focus": TARGET_FOCUS.get(parent_id, "any"),
        "flying": parent_id in FLYING,
        "heals": False,
        "footprint_tiles": parent_doc.get("size", "1x1"),
        "housing_space": super_doc.get("housingSpace"),
        "range_tiles": super_doc.get("range", parent_doc.get("range")),
        "attack_speed_seconds": super_doc.get("attackSpeed", parent_doc.get("attackSpeed")),
        "movement_speed": super_doc.get("movementSpeed", parent_doc.get("movementSpeed")),
        "target_type": super_doc.get("targetType", parent_doc.get("targetType")),
        "damage_type": super_doc.get("damageType", parent_doc.get("damageType")),
        "modes": normalized_modes(super_doc),
        "levels": levels,
        "regular_level_required": super_doc.get("regularLevelRequired"),
        "boost_cost": super_doc.get("boostCost"),
        "boost_cost_resource": super_doc.get("boostCostResource"),
        "boost_duration_seconds": duration_seconds(super_doc.get("boostDuration")),
        "special_ability": super_doc.get("specialAbility"),
        "support": "catalogued_only",
        "field_provenance": {
            "target_focus": inherited, "flying": inherited,
            "footprint_tiles": inherited, "target_type": inherited,
            "damage_type": inherited,
        },
    }

def main() -> None:
    p = argparse.ArgumentParser(); p.add_argument("--commit", required=True); p.add_argument("--out", default="data"); args = p.parse_args()
    if len(args.commit) != 40 or any(c not in "0123456789abcdef" for c in args.commit.lower()): p.error("--commit must be a full immutable 40-character SHA")
    root = pathlib.Path(args.out); raw = root / "raw" / args.commit; raw.mkdir(parents=True, exist_ok=True)
    # Re-importing the same immutable raw commit must not rewrite consultation
    # dates or claim that its bytes were fetched again. Preserve the original
    # source records when their URL and digest still match.
    prior_catalogue_path = root / "catalogue.normalized.json"
    prior_catalogue = (json.loads(prior_catalogue_path.read_text(encoding="utf-8"))
                       if prior_catalogue_path.exists() else {})
    prior_sources = {}
    if prior_catalogue.get("source", {}).get("commit") == args.commit:
        for content in prior_catalogue.get("contents", []) + prior_catalogue.get("spells", []):
            for level in content.get("levels", []):
                source = level.get("provenance", {}).get("source", {})
                if source.get("url") and source.get("raw_sha256") and "source_path" not in source:
                    prior_sources[(source["url"], source["raw_sha256"])] = source
    def source_provenance_for(url: str, digest: str) -> dict:
        return dict(prior_sources.get((url, digest)) or {
            "tier": "secondary", "source": "chiefpansancolt-62b019df",
            "url": url, "version": args.commit, "raw_sha256": digest,
            "consulted_at_utc": dt.datetime.now(dt.timezone.utc).isoformat()})
    entries, spells, raw_manifest = [], [], []
    for identity, (source_id, rel) in FILES.items():
        url = BASE.format(commit=args.commit) + rel
        cached = raw / pathlib.Path(rel).name
        # A cache path is namespaced by the immutable full commit and is
        # therefore an exact source artefact, not a mutable fallback.  Read it
        # first: this keeps imports reproducible and avoids needless network
        # dependency after the initial acquisition.
        if cached.exists():
            payload = cached.read_bytes()
        else:
            try:
                payload = get(url)
                cached.write_bytes(payload)
            except Exception as exc:
                print(f"skip {identity}: {exc}")
                continue
        digest = hashlib.sha256(payload).hexdigest()
        doc = json.loads(payload); assert doc["id"] == source_id, (source_id, doc.get("id"))
        if doc.get("base") != "home": raise ValueError(f"{identity}: not home base")
        source_provenance = source_provenance_for(url, digest)
        if doc.get("category") == "hero-equipment":
            entries.append(apply_content_overrides(
                normalized_hero_equipment(identity, source_id, doc, source_provenance), identity))
            raw_manifest.append({"url":url,"relative_path":rel,"sha256":digest})
            continue
        normal_mode = doc.get("modes", {}).get("normal", {})
        hero_icon = doc.get("images", {}).get("icon") if doc.get("category") == "hero" else None
        levels = [normalized_level(level, hero_icon, pet=doc.get("category") == "pet") for level in doc.get("levels", [])
                  if "level" in level]
        for level in levels:
            override = CURRENT_LEVEL_OVERRIDES.get((identity, level["level"], level.get("variant", "normal")), {})
            level.update(override)
            level["provenance"] = {"level": level["level"], "source": source_provenance, "overridden_fields": sorted(override)}
        imported_numbers = {(level["level"], level.get("variant", "normal")) for level in levels}
        for level in EXTRA_LEVELS.get(identity, []):
            key = (level["level"], level.get("variant", "normal"))
            if key in imported_numbers:
                raise RuntimeError(f"supplement duplicates raw level: {identity} L{level['level']} ({key[1]})")
            # A supplementary level can still need a later, independently
            # sourced field correction (for example a Laboratory requirement
            # published by a different current table).  Apply the same frozen
            # ledger as raw levels and record the fields; never hand-edit the
            # generated catalogue after import.
            override = CURRENT_LEVEL_OVERRIDES.get((identity, level["level"], level.get("variant", "normal")), {})
            level.update(override)
            provenance = level.setdefault("provenance", {})
            provenance["overridden_fields"] = sorted(
                set(provenance.get("overridden_fields", [])) | set(override)
            )
            levels.append(level)
        levels.sort(key=lambda level: level["level"])
        if identity == "hero_banner":
            entries.append(non_combat_content(FROZEN_REFERENCE, identity, source_id, doc.get("category", "other")))
        else:
            entries.append(apply_content_overrides({"id": identity, "source_id": source_id, "category": doc.get("category"), "target_focus": TARGET_FOCUS.get(identity, "any"), "flying": identity in FLYING, "heals": identity in HEALERS, "footprint_tiles": doc.get("size"), "housing_space": doc.get("housingSpace", 0), "activation_housing_space": normal_mode.get("activationHousingSpace", doc.get("triggerHousingSpace", 0)), "range_tiles": doc.get("range", normal_mode.get("range")), "trigger_range_tiles": doc.get("triggerRadius", normal_mode.get("triggerRange", 0)), "min_range_tiles": normal_mode.get("minRange", 0), "splash_radius_tiles": SPLASH_RADIUS_TILES.get(identity, doc.get("damageRadius", normal_mode.get("splashRadius", 0))), "death_splash_radius_tiles": DEATH_SPLASH_RADIUS_TILES.get(identity, 0), "shots_per_burst": normal_mode.get("shotsPerBurst", 1), "time_between_bursts_seconds": normal_mode.get("timeBetweenBursts", 0), "attack_speed_seconds": doc.get("attackSpeed", normal_mode.get("attackSpeed")), "aura_range_tiles": doc.get("auraRange"), "aura_attack_speed_seconds": doc.get("auraAttackSpeed"), "movement_speed": doc.get("movementSpeed"), "projectile_speed_tiles_per_second": doc.get("projectileSpeed"), "target_type": doc.get("targetType"), "damage_type": doc.get("damageType", normal_mode.get("damageType")), "modes": normalized_modes(doc), "levels": levels, "available_per_town_hall": doc.get("availablePerTownHall", []), "support": "catalogued_only"}, identity))
        if super_doc := doc.get("superTroop"):
            super_entry = normalized_super_troop(identity, doc, super_doc, source_provenance)
            # Super Troops have their own content ids (for example
            # `sneaky_goblin`, not `goblin`).  Apply frozen behavioural
            # overrides against that id so the generated catalogue retains
            # provenance instead of requiring a hand edit after every import.
            entries.append(apply_content_overrides(super_entry, super_entry["id"]))
        raw_manifest.append({"url":url,"relative_path":rel,"sha256":digest})
    for identity, (source_id, rel) in SPELLS.items():
        url = BASE.format(commit=args.commit) + rel; cached = raw / pathlib.Path(rel).name
        if cached.exists():
            payload = cached.read_bytes()
        else:
            try:
                payload = get(url); cached.write_bytes(payload)
            except Exception as exc:
                print(f"skip {identity}: {exc}"); continue
        digest = hashlib.sha256(payload).hexdigest(); doc = json.loads(payload)
        assert doc["id"] == source_id, (source_id, doc.get("id"))
        source_provenance = source_provenance_for(url, digest)
        spell_levels = [normalized_spell_level(level) for level in doc.get("levels", [])]
        for level in spell_levels:
            level["provenance"] = {"level": level["level"], "source": source_provenance, "overridden_fields": []}
        spells.append({"id": identity, "source_id": source_id, "radius_tiles": doc.get("radius"), "number_of_pulses": doc.get("numberOfPulses", 1), "time_between_pulses_seconds": doc.get("timeBetweenPulses", 0), "boost_time_seconds": doc.get("boostTime", 0), "levels": spell_levels})
        raw_manifest.append({"url":url,"relative_path":rel,"sha256":digest})
    existing_spell_ids = {spell["id"] for spell in spells}
    for spell in SUPPLEMENTAL_SPELLS:
        if spell["id"] in existing_spell_ids:
            raise RuntimeError(f"supplement spell duplicates raw entry: {spell['id']}")
        spells.append(spell)
    existing_ids = {entry["id"] for entry in entries}
    for content in SUPPLEMENTAL_CONTENTS:
        if content["id"] in existing_ids:
            raise RuntimeError(f"supplement content duplicates raw entry: {content['id']}")
        # A dated official delta may supersede one qualitative field of a
        # hashed secondary supplemental record.  Apply it through the same
        # versioned override path as raw-snapshot content; never hand-edit the
        # generated catalogue after import.
        for level in content["levels"]:
            override = CURRENT_LEVEL_OVERRIDES.get((content["id"], level["level"], level.get("variant", "normal")), {})
            level.update(override)
            provenance = level.setdefault("provenance", {})
            provenance["overridden_fields"] = sorted(
                set(provenance.get("overridden_fields", [])) | set(override)
            )
        entries.append(apply_content_overrides(content, content["id"]))
    if not entries: raise RuntimeError("no source data fetched or found in the immutable raw cache")
    imported_at_utc = (prior_catalogue.get("source", {}).get("imported_at_utc")
                       if prior_catalogue.get("source", {}).get("commit") == args.commit
                       else None) or dt.datetime.now(dt.timezone.utc).isoformat()
    catalogue = {"schema_version":3,"source":{"repository":"chiefpansancolt/clash-of-clans-data","commit":args.commit,"game_version":"unknown","licence":"not verified by this importer; consult the upstream repository before redistribution","imported_at_utc":imported_at_utc},"units":{"distance":"source tiles","attack_speed":"seconds","projectile_speed":"tiles per second","damage":"damage per impact","upgrade_time":"seconds","production_rate":"source productionRate units"},"contents":entries,"spells":spells}
    (root / "catalogue.normalized.json").write_text(json.dumps(catalogue, indent=2) + "\n", encoding="utf-8")
    manifest = {"schema_version":3,"source_commit":args.commit,"imported_at_utc":catalogue["source"]["imported_at_utc"],"files":raw_manifest}
    (root / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
if __name__ == "__main__": main()
