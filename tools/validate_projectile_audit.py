#!/usr/bin/env python3
"""Keep the executable projectile audit aligned with immutable data and Core."""
from __future__ import annotations

import hashlib
import json
import pathlib
import re

from project_layout import core_source_text, registered_kind_ids


ROOT = pathlib.Path(__file__).resolve().parents[1]
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"
AUDIT = ROOT / "docs" / "projectile-audit-2026-09-17.md"
AUDIT_DELTA = ROOT / "docs" / "projectile-audit-2026-09-18-monolith.md"
SUPER_ARCHER_AUDIT_DELTA = ROOT / "docs" / "projectile-audit-2026-09-18-super-archer.md"
CONTRACTS = ROOT / "data" / "reference" / "projectile-contracts-2026-09-17.json"
CONTRACT_DELTA = ROOT / "data" / "reference" / "projectile-contracts-2026-09-18-monolith.json"
SUPER_ARCHER_CONTRACT_DELTA = ROOT / "data" / "reference" / "projectile-contracts-2026-09-18-super-archer.json"
INFERNO_DRAGON_CONTRACT_DELTA = ROOT / "data" / "reference" / "projectile-contracts-2026-09-18-inferno-dragon.json"
SUPER_WIZARD_CONTRACT_DELTA = ROOT / "data" / "reference" / "projectile-contracts-2026-09-18-super-wizard.json"
SUPER_MINION_CONTRACT_DELTA = ROOT / "data" / "reference" / "projectile-contracts-2026-09-18-super-minion.json"
SUPER_BOWLER_CONTRACT_DELTA = ROOT / "data" / "reference" / "projectile-contracts-2026-09-18-super-bowler.json"
SUPER_DRAGON_CONTRACT_DELTA = ROOT / "data" / "reference" / "projectile-contracts-2026-09-18-super-dragon.json"
DAMAGE_PATHS = ROOT / "data" / "reference" / "combat-damage-paths-2026-09-17.json"
HISTORICAL_CALIBRATION = ROOT / "data" / "reference" / "historical-projectile-speed-calibration-v18.200.9.json"
HEADER = ROOT / "include" / "cocsim" / "core.hpp"
BINDING = ROOT / "bindings" / "module.cpp"


def has_long_range_weapon(content: dict) -> bool:
    """Include per-level nested weapons such as the armed Town Hall.

    Most catalogue entries expose their range at content level. Town Hall
    armaments are intentionally nested under each level, so using only the
    flat value would let a live ranged Core path evade the audit.
    """
    if (content.get("range_tiles") or 0) > 1.0:
        return True
    return any(((level.get("weapon") or {}).get("range") or 0) > 1.0
               for level in content.get("levels", []))


def main() -> None:
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    contract_bytes = CONTRACTS.read_bytes()
    contract_document = json.loads(contract_bytes)
    contract_delta = json.loads(CONTRACT_DELTA.read_text(encoding="utf-8"))
    super_archer_delta_bytes = SUPER_ARCHER_CONTRACT_DELTA.read_bytes()
    super_archer_delta = json.loads(super_archer_delta_bytes)
    inferno_dragon_delta_bytes = INFERNO_DRAGON_CONTRACT_DELTA.read_bytes()
    inferno_dragon_delta = json.loads(inferno_dragon_delta_bytes)
    super_wizard_delta_bytes = SUPER_WIZARD_CONTRACT_DELTA.read_bytes()
    super_wizard_delta = json.loads(super_wizard_delta_bytes)
    super_minion_delta_bytes = SUPER_MINION_CONTRACT_DELTA.read_bytes()
    super_minion_delta = json.loads(super_minion_delta_bytes)
    super_bowler_delta_bytes = SUPER_BOWLER_CONTRACT_DELTA.read_bytes()
    super_bowler_delta = json.loads(super_bowler_delta_bytes)
    super_dragon_delta_bytes = SUPER_DRAGON_CONTRACT_DELTA.read_bytes()
    super_dragon_delta = json.loads(super_dragon_delta_bytes)
    assert contract_delta["schema_version"] == 1
    assert contract_delta["parent_contract"] == {
        "file": CONTRACTS.name,
        "sha256": hashlib.sha256(contract_bytes).hexdigest(),
    }
    calibration_bytes = HISTORICAL_CALIBRATION.read_bytes()
    assert contract_delta["parent_historical_calibration"] == {
        "file": HISTORICAL_CALIBRATION.name,
        "sha256": hashlib.sha256(calibration_bytes).hexdigest(),
    }
    assert super_archer_delta["parent_contract"] == {
        "file": CONTRACT_DELTA.name,
        "sha256": hashlib.sha256(CONTRACT_DELTA.read_bytes()).hexdigest(),
    }
    assert inferno_dragon_delta["parent_contract"] == {
        "file": SUPER_ARCHER_CONTRACT_DELTA.name,
        "sha256": hashlib.sha256(super_archer_delta_bytes).hexdigest(),
    }
    assert super_wizard_delta["parent_contract"] == {"file": INFERNO_DRAGON_CONTRACT_DELTA.name, "sha256": hashlib.sha256(inferno_dragon_delta_bytes).hexdigest()}
    assert super_minion_delta["parent_contract"] == {"file": SUPER_WIZARD_CONTRACT_DELTA.name, "sha256": hashlib.sha256(super_wizard_delta_bytes).hexdigest()}
    assert super_bowler_delta["parent_contract"] == {"file": SUPER_MINION_CONTRACT_DELTA.name, "sha256": hashlib.sha256(super_minion_delta_bytes).hexdigest()}
    assert super_dragon_delta["parent_contract"] == {"file": SUPER_BOWLER_CONTRACT_DELTA.name, "sha256": hashlib.sha256(super_bowler_delta_bytes).hexdigest()}
    contract_document["catalogue_audit"].update(contract_delta["catalogue_audit_override"])
    contract_document["contracts"].extend(contract_delta["contracts"])
    contract_document["test_evidence"].extend(contract_delta["test_evidence"])
    contract_document["catalogue_audit"].update(super_archer_delta["catalogue_audit_override"])
    contract_document["contracts"].extend(super_archer_delta["contracts"])
    contract_document["test_evidence"].extend(super_archer_delta["test_evidence"])
    contract_document["catalogue_audit"].update(inferno_dragon_delta["catalogue_audit_override"])
    contract_document["contracts"].extend(inferno_dragon_delta["contracts"])
    contract_document["test_evidence"].extend(inferno_dragon_delta["test_evidence"])
    contract_document["catalogue_audit"].update(super_wizard_delta["catalogue_audit_override"])
    contract_document["contracts"].extend(super_wizard_delta["contracts"])
    contract_document["test_evidence"].extend(super_wizard_delta["test_evidence"])
    contract_document["catalogue_audit"].update(super_minion_delta["catalogue_audit_override"])
    contract_document["contracts"].extend(super_minion_delta["contracts"])
    contract_document["test_evidence"].extend(super_minion_delta["test_evidence"])
    contract_document["catalogue_audit"].update(super_bowler_delta["catalogue_audit_override"])
    contract_document["contracts"].extend(super_bowler_delta["contracts"])
    contract_document["test_evidence"].extend(super_bowler_delta["test_evidence"])
    contract_document["catalogue_audit"].update(super_dragon_delta["catalogue_audit_override"])
    contract_document["contracts"].extend(super_dragon_delta["contracts"])
    contract_document["test_evidence"].extend(super_dragon_delta["test_evidence"])
    damage_path_document = json.loads(DAMAGE_PATHS.read_text(encoding="utf-8"))
    historical_calibration = json.loads(calibration_bytes)
    historical_calibration["unavailable_runtime_profiles"].extend(
        contract_delta["historical_unavailable_profiles"])
    historical_calibration["unavailable_runtime_profiles"].extend(
        super_archer_delta["historical_unavailable_profiles"])
    historical_calibration["unavailable_runtime_profiles"].extend(
        inferno_dragon_delta["historical_unavailable_profiles"])
    historical_calibration["unavailable_runtime_profiles"].extend(
        super_wizard_delta["historical_unavailable_profiles"])
    historical_calibration["unavailable_runtime_profiles"].extend(
        super_minion_delta["historical_unavailable_profiles"])
    historical_calibration["unavailable_runtime_profiles"].extend(
        super_bowler_delta["historical_unavailable_profiles"])
    historical_calibration["unavailable_runtime_profiles"].extend(
        super_dragon_delta["historical_unavailable_profiles"])
    audit = (AUDIT.read_text(encoding="utf-8") + "\n" + AUDIT_DELTA.read_text(encoding="utf-8")
             + "\n" + SUPER_ARCHER_AUDIT_DELTA.read_text(encoding="utf-8")
             + "\n" + (ROOT / "docs" / "projectile-audit-2026-09-18-inferno-dragon.md").read_text(encoding="utf-8")
             + "\n" + (ROOT / "docs" / "projectile-audit-2026-09-18-super-wizard.md").read_text(encoding="utf-8")
             + "\n" + (ROOT / "docs" / "projectile-audit-2026-09-18-super-minion.md").read_text(encoding="utf-8")
             + "\n" + (ROOT / "docs" / "projectile-audit-2026-09-18-super-bowler.md").read_text(encoding="utf-8")
             + "\n" + (ROOT / "docs" / "projectile-audit-2026-09-18-super-dragon.md").read_text(encoding="utf-8"))
    core = core_source_text()
    header = HEADER.read_text(encoding="utf-8")
    binding = BINDING.read_text(encoding="utf-8")
    contents = {item["id"]: item for item in catalogue["contents"]}
    # `to_string(Kind)` is the authoritative public identifier set accepted by
    # scenarios.  Derive the audit perimeter from it rather than maintaining a
    # second hand-written list that could omit a newly executable ranged unit.
    runtime_ids = set(registered_kind_ids())
    current = {
        identity for identity in runtime_ids
        if identity in contents
        and has_long_range_weapon(contents[identity])
    }
    # This trap owns a physical projectile but its attack is driven by the
    # sourced activation state rather than a normal weapon range.
    current.add("seeking_air_mine")
    expected = {
        "archer", "cannon", "archer_tower", "mortar", "wizard", "dragon",
        "eagle_artillery", "inferno_tower", "minion", "air_defense",
        "wizard_tower", "x_bow", "dragon_rider", "baby_dragon", "bomb_tower",
        "hidden_tesla", "town_hall", "healer", "seeking_air_mine",
        "multi_archer_tower", "multi_gear_tower", "scattershot", "thrower", "electro_titan", "monolith",
        "super_archer", "inferno_dragon", "super_wizard", "super_minion", "super_bowler", "super_dragon",
    }
    assert current == expected, (current - expected, expected - current)
    assert current <= contents.keys(), current - contents.keys()
    contracts = {entry["id"]: entry for entry in contract_document["contracts"]}
    assert set(contracts) == current, (set(contracts) - current, current - set(contracts))
    assert contract_document["scope"].startswith("Every currently executable")
    assert contract_document["test_evidence"], "projectile coverage has no test evidence"
    # The source inventory is wider than the executable Core perimeter. Keep
    # both counts frozen so a new long-range catalogue entry cannot silently
    # look audited merely because it has not yet acquired a Kind/behavior.
    catalogue_ranged = {
        identity for identity, content in contents.items()
        if identity != "healer" and has_long_range_weapon(content)
    }
    audit_boundary = contract_document["catalogue_audit"]
    executable_weapon_paths = current - {"healer", "seeking_air_mine"}
    assert audit_boundary["non_healer_long_range_candidates"] == len(catalogue_ranged)
    assert audit_boundary["currently_executable_weapon_paths"] == len(executable_weapon_paths)
    assert audit_boundary["currently_executable_special_paths"] == ["healer", "seeking_air_mine"]
    assert audit_boundary["non_executable_long_range_candidates"] == len(catalogue_ranged - executable_weapon_paths)
    assert audit_boundary["boundary"].startswith("Candidates outside cocsim_core")
    generic = current - {"healer", "inferno_tower", "inferno_dragon", "super_wizard", "super_minion", "super_bowler", "super_dragon", "seeking_air_mine", "town_hall", "scattershot", "electro_titan", "monolith", "super_archer"}
    for identity in generic:
        contract = contracts[identity]
        assert contract["model"] == "logical_next_tick_unmeasured", identity
        assert contract["core"] == "BattleState::Projectile", identity
        assert contract["gui"] == "amber_post_impact_trace", identity
        assert contract["rl"] == "projectiles", identity
        assert contract["fidelity"] == "open", identity
        assert contract["provenance"]["status"] == "missing", identity
    super_archer = contracts["super_archer"]
    assert super_archer["model"] == "logical_next_tick_piercing_partial"
    assert super_archer["provenance"]["status"] == "sourced_secondary_partial"
    eagle_burst = contracts["eagle_artillery"]["burst_timing"]
    assert eagle_burst == {
        "shots": 3, "shot_interval_ms": 750, "pause_ms": 10000,
        "status": "catalogue_sourced",
        "needed": "shell travel, impact geometry and dispersion ordering",
    }
    assert contracts["healer"]["model"] == "logical_next_tick_heal_unmeasured"
    assert contracts["healer"]["core"] == "BattleState::Projectile"
    assert contracts["healer"]["gui"] == "green_post_impact_trace"
    assert contracts["healer"]["rl"] == "projectiles"
    monolith_contract = contracts["monolith"]
    assert monolith_contract["model"] == "logical_next_tick_unmeasured"
    assert monolith_contract["core"] == "BattleState::Projectile"
    assert monolith_contract["fidelity"] == "partial"
    assert monolith_contract["target_max_hp_damage_percent"]["levels"] == {
        str(level): level + 10 for level in range(1, 6)
    }
    assert monolith_contract["provenance"]["source_id"] == "fandom-2026-09-18-monolith"
    town_hall_contract = contracts["town_hall"]
    assert town_hall_contract["model"] == "logical_next_tick_unmeasured"
    assert town_hall_contract["core"] == "BattleState::Projectile"
    assert town_hall_contract["gui"] == "amber_post_impact_trace"
    assert town_hall_contract["rl"] == "projectiles"
    assert town_hall_contract["fidelity"] == "partial"
    assert town_hall_contract["multi_target"]["targets"] == 4
    assert town_hall_contract["multi_target"]["flame_max_dps"] == 75
    assert town_hall_contract["multi_target"]["flame_duration_seconds"] == 6.8
    assert town_hall_contract["multi_target"]["flame_data_status"] == "secondary_TH17_only_unapplied"
    assert town_hall_contract["provenance"]["status"] == "partial_official"
    assert town_hall_contract["provenance"]["url"] == "https://supercell.com/en/games/clashofclans/blog/game-updates/the-town-hall-17-update-is-here-2/"
    inferno_contract = contracts["inferno_tower"]
    assert inferno_contract["model"] == "immediate_beam_secondary"
    assert inferno_contract["core"] == "direct deterministic damage"
    assert inferno_contract["gui"] == "violet_beam_trace"
    assert inferno_contract["rl"] == "events_since"
    assert inferno_contract["provenance"]["status"] == "sourced_secondary"
    assert inferno_contract["provenance"]["url"] == "https://www.clash.ninja/guides/the-inferno-tower"
    inferno_dragon_contract = contracts["inferno_dragon"]
    assert inferno_dragon_contract["model"] == "immediate_beam_secondary_partial"
    assert inferno_dragon_contract["core"] == "direct deterministic damage"
    assert inferno_dragon_contract["gui"] == "violet_beam_trace"
    assert inferno_dragon_contract["rl"] == "events_since"
    assert inferno_dragon_contract["provenance"]["source_id"] == "fandom-2026-09-18-inferno-dragon"
    super_wizard_contract = contracts["super_wizard"]
    assert super_wizard_contract["model"] == "logical_next_tick_primary_branching_partial"
    assert super_wizard_contract["provenance"]["source_id"] == "supercell-2021-01-20-super-wizard-balance"
    super_minion_contract = contracts["super_minion"]
    assert super_minion_contract["model"] == "logical_next_tick_opening_range_lower_bound_partial"
    assert super_minion_contract["provenance"]["source_id"] == "supercell-2026-01-28-super-minion-balance"
    super_bowler_contract = contracts["super_bowler"]
    assert super_bowler_contract["model"] == "logical_next_tick_fixed_bounces_partial"
    assert super_bowler_contract["provenance"]["source_id"] == "supercell-2021-09-25-super-bowler-launch"
    assert contracts["super_dragon"]["model"] == "logical_next_tick_aggregate_primary_lower_bound"
    scattershot_contract = contracts["scattershot"]
    assert scattershot_contract["model"] == "logical_next_tick_unmeasured"
    assert scattershot_contract["core"] == "BattleState::Projectile"
    assert scattershot_contract["gui"] == "amber_post_impact_trace"
    assert scattershot_contract["rl"] == "projectiles"
    assert scattershot_contract["fidelity"] == "partial"
    assert scattershot_contract["provenance"]["source_id"] == "fandom-2026-09-17-scattershot"
    electro_titan_contract = contracts["electro_titan"]
    assert electro_titan_contract["model"] == "logical_next_tick_unmeasured"
    assert electro_titan_contract["core"] == "BattleState::Projectile"
    assert electro_titan_contract["gui"] == "amber_post_impact_trace"
    assert electro_titan_contract["rl"] == "projectiles"
    assert electro_titan_contract["fidelity"] == "partial"
    assert electro_titan_contract["provenance"]["source_id"] == "fandom-2026-09-18-electro-titan"
    mine_contract = contracts["seeking_air_mine"]
    assert mine_contract["model"] == "moving_homing_sourced"
    assert mine_contract["speed_tiles_per_second"] == 3.5
    assert mine_contract["provenance"]["source_id"] == "supercell-2021-08-11-seeking-air-mine"
    for identity in current:
        assert identity.replace("_", " ") in audit or identity in audit, identity
    quantified = {identity: contents[identity].get("projectile_speed_tiles_per_second")
                  for identity in current if contents[identity].get("projectile_speed_tiles_per_second") is not None}
    assert quantified == {"seeking_air_mine": 3.5}, quantified
    assert historical_calibration["source"]["source_id"] == "enjoyop2-game-data-assets-2026-09-17"
    calibration = historical_calibration["calibration"]
    assert calibration["content_id"] == "seeking_air_mine"
    assert calibration["historical_internal_speed"] / calibration["inferred_internal_units_per_tile"] == calibration["official_tiles_per_second"] == 3.5
    assert historical_calibration["current_reference_gate"]["application"] == "not_imported"
    assert all(example["application"] == "not_imported" for example in historical_calibration["unapplied_examples"])
    # An archived client table is comparison evidence, never a backdoor for
    # current GameData. Keep the complete executable perimeter explicit so a
    # newly observed speed cannot silently become a current combat rule.
    historical_profiles = {profile["content_id"]: profile
                           for profile in historical_calibration["historical_unapplied_profiles"]}
    # The archived pre-TH18 table has no verified Town Hall armament record;
    # it is documented as an explicit missing calibration instead of being
    # back-filled with guessed old client data. The two merged defenses were
    # not present in the v18.200.9 archive, so the archival document carries
    # an explicit unavailable record instead of a fabricated profile.
    unavailable_historical = {entry["content_id"]: entry for entry in historical_calibration["unavailable_runtime_profiles"]}
    assert set(unavailable_historical) == {"multi_archer_tower", "multi_gear_tower", "scattershot", "thrower", "electro_titan", "monolith", "super_archer", "inferno_dragon", "super_wizard", "super_minion", "super_bowler", "super_dragon"}
    assert all(entry["reason"] for entry in unavailable_historical.values())
    assert "No reproducible historical" in unavailable_historical["monolith"]["reason"]
    expected_historical_profiles = current - {"inferno_tower", "seeking_air_mine", "town_hall", *unavailable_historical}
    assert set(historical_profiles) == expected_historical_profiles
    assert all(profile["application"] == "not_imported"
               for profile in historical_profiles.values())
    assert all(not contents[identity].get("projectile_speed_tiles_per_second")
               for identity in historical_profiles)
    assert "struct ProjectileView" in header and "ProjectileId id" in header and "target_position" in header and "bool speed_sourced" in header and "observe_projectiles" in header and "projectile_speed_sourced" in header
    assert "speed_tiles_per_second" in core and "next_projectile_id_" in core and "COCSIM-SNAPSHOT-18" in core and "projectiles_ = std::move(active)" in core
    assert 'row["id"]=projectile.id' in binding and "projectile_views" in binding and 'row["target_x"]=projectile.target_position.x' in binding and 'row["scheduled_impact_ms"]=projectile.scheduled_impact_ms' in binding and 'row["speed_sourced"]=projectile.speed_sourced' in binding and 'row["projectile_speed_sourced"]=stat.projectile_speed_sourced' in binding
    assert "seeking air mine launched" in core and "const bool inferno_beam=inferno_single || a.kind==Kind::InfernoTower" in core
    runtime_test = (ROOT / "tools" / "validate_rl_runtime.py").read_text(encoding="utf-8")
    assert "events_since" in binding and 'event["detail"] == "inferno beam"' in runtime_test
    assert 'row["has_origin"] = event.has_origin' in binding
    assert 'row["has_target_position"] = event.has_target_position' in binding
    assert 'row["projectile_id"] = event.projectile_id' in binding
    assert 'event["has_origin"] and event["has_target_position"]' in runtime_test
    assert 'and event["projectile_id"] == healing_id' in runtime_test
    assert 'town_hall_projectiles' in runtime_test
    assert '"targets"' in core and 'level_multi_target_count' in core
    assert '"projectile heal",p.id' in core
    # Catalogue timing metadata comes from immutable GameData and is projected
    # read-only to RL; it must not be rebuilt from a UI timer.
    assert 'row["first_burst_delay_ms"]=stat.attributes.first_burst_delay' in binding
    assert 'row["first_burst_delay_ms"] == quantized_ms(' in runtime_test
    # Inventory every hp-changing control-flow family, not only weapons whose
    # range happens to be greater than one tile. This guards against quietly
    # adding a direct long-range attack outside the projectile contract.
    expected_damage_paths = {
        "ranged_weapon_projectile", "inferno_beam", "melee_contact",
        "contact_splash", "root_rider_wall_contact", "spell_lightning_and_earthquake",
        "spell_heal", "passive_aura", "death_explosion", "triggered_trap", "tornado_trap",
    }
    paths = {path["id"]: path for path in damage_path_document["paths"]}
    assert set(paths) == expected_damage_paths, (set(paths), expected_damage_paths)
    assert damage_path_document["scope"].startswith("Every call path")
    assert damage_path_document["guard"]["only_documented_immediate_ranged_actor"] == "inferno_beam"
    assert paths["ranged_weapon_projectile"]["snapshot"] == "V14"
    assert paths["passive_aura"]["snapshot"] == "V14 entity aura timer"
    assert paths["passive_aura"]["delivery"] == "periodic_area_effect"
    assert paths["inferno_beam"]["delivery"] == "documented_immediate_beam"
    assert paths["inferno_beam"]["rl"] == "events_since"
    # The documented direct-beam bypasses are explicitly guarded by the beam
    # flag. Other ranged attacks must enter projectiles_ and all delayed
    # effects use their own serialised queue/state.
    assert "if(a.ranged&&!inferno_beam)" in core
    assert "(!p.homing || p.speed_tiles_per_second <= 0.0)" in core
    assert "must not silently become a homing shot" in core
    assert "invalid ballistic projectile" in core
    for marker in ("void BattleState::cast_spell(const Command& c)",
                   "void BattleState::update_spells()",
                   "void BattleState::update_death_explosions()",
                   "void BattleState::update_traps()",
                   "void BattleState::update_tornado_traps()"):
        assert marker in core, marker
    print(f"Projectile audit validation passed ({len(current)} executable ranged/healing entries; {len(quantified)} sourced speed)")


if __name__ == "__main__":
    main()
