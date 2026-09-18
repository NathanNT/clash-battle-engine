#!/usr/bin/env python3
"""Validate the additive active TH18 reference and its imported delta level."""
from __future__ import annotations

import hashlib
import json
import pathlib

from importer.reference import FROZEN_REFERENCE


ROOT = pathlib.Path(__file__).resolve().parents[1]
PARENT = ROOT / "data" / "reference" / "th18-2026-09-17.json"
DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-monolith-l5.json"
SUPER_WALL_BREAKER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-wall-breaker.json"
SUPER_BARBARIAN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-barbarian.json"
SUPER_GIANT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-giant.json"
SUPER_ARCHER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-archer.json"
ROCKET_BALLOON_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-rocket-balloon.json"
INFERNO_DRAGON_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-inferno-dragon.json"
SUPER_WIZARD_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-wizard.json"
SUPER_MINION_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-minion.json"
SUPER_BOWLER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-bowler.json"
SUPER_DRAGON_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-dragon.json"
OBSERVATION = ROOT / "data" / "reference" / "monolith-l5-observation-2026-09-18.json"
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"


def main() -> None:
    parent_bytes = PARENT.read_bytes()
    parent = json.loads(parent_bytes)
    delta_bytes = DELTA.read_bytes()
    delta = json.loads(delta_bytes)
    super_wall_breaker_bytes = SUPER_WALL_BREAKER_DELTA.read_bytes()
    super_wall_breaker_delta = json.loads(super_wall_breaker_bytes)
    super_barbarian_delta = json.loads(SUPER_BARBARIAN_DELTA.read_text(encoding="utf-8"))
    super_giant_bytes = SUPER_GIANT_DELTA.read_bytes()
    super_giant_delta = json.loads(super_giant_bytes)
    super_archer_bytes = SUPER_ARCHER_DELTA.read_bytes()
    super_archer_delta = json.loads(super_archer_bytes)
    rocket_balloon_bytes = ROCKET_BALLOON_DELTA.read_bytes()
    rocket_balloon_delta = json.loads(rocket_balloon_bytes)
    inferno_dragon_bytes = INFERNO_DRAGON_DELTA.read_bytes()
    inferno_dragon_delta = json.loads(inferno_dragon_bytes)
    super_wizard_bytes = SUPER_WIZARD_DELTA.read_bytes()
    super_wizard_delta = json.loads(super_wizard_bytes)
    super_minion_bytes = SUPER_MINION_DELTA.read_bytes()
    super_minion_delta = json.loads(super_minion_bytes)
    super_bowler_bytes = SUPER_BOWLER_DELTA.read_bytes()
    super_bowler_delta = json.loads(super_bowler_bytes)
    super_dragon_bytes = SUPER_DRAGON_DELTA.read_bytes()
    super_dragon_delta = json.loads(super_dragon_bytes)
    observation_bytes = OBSERVATION.read_bytes()
    observation = json.loads(observation_bytes)
    assert delta["schema_version"] == 1
    assert delta["parent_reference"] == {
        "file": PARENT.name,
        "reference_id": parent["reference_id"],
        "sha256": hashlib.sha256(parent_bytes).hexdigest(),
    }
    assert delta["reference_id"] == "home-village-th18-2026-09-18-monolith-l5"
    assert super_wall_breaker_delta["parent_reference"] == {
        "file": DELTA.name,
        "reference_id": delta["reference_id"],
        "sha256": hashlib.sha256(delta_bytes).hexdigest(),
    }
    assert super_barbarian_delta["parent_reference"] == {
        "file": SUPER_WALL_BREAKER_DELTA.name,
        "reference_id": super_wall_breaker_delta["reference_id"],
        "sha256": hashlib.sha256(super_wall_breaker_bytes).hexdigest(),
    }
    assert super_giant_delta["parent_reference"] == {
        "file": SUPER_BARBARIAN_DELTA.name,
        "reference_id": super_barbarian_delta["reference_id"],
        "sha256": hashlib.sha256(SUPER_BARBARIAN_DELTA.read_bytes()).hexdigest(),
    }
    assert super_archer_delta["parent_reference"] == {
        "file": SUPER_GIANT_DELTA.name,
        "reference_id": super_giant_delta["reference_id"],
        "sha256": hashlib.sha256(super_giant_bytes).hexdigest(),
    }
    assert rocket_balloon_delta["parent_reference"] == {
        "file": SUPER_ARCHER_DELTA.name,
        "reference_id": super_archer_delta["reference_id"],
        "sha256": hashlib.sha256(super_archer_bytes).hexdigest(),
    }
    assert inferno_dragon_delta["parent_reference"] == {
        "file": ROCKET_BALLOON_DELTA.name,
        "reference_id": rocket_balloon_delta["reference_id"],
        "sha256": hashlib.sha256(rocket_balloon_bytes).hexdigest(),
    }
    assert super_wizard_delta["parent_reference"] == {"file": INFERNO_DRAGON_DELTA.name, "reference_id": inferno_dragon_delta["reference_id"], "sha256": hashlib.sha256(inferno_dragon_bytes).hexdigest()}
    assert super_minion_delta["parent_reference"] == {"file": SUPER_WIZARD_DELTA.name, "reference_id": super_wizard_delta["reference_id"], "sha256": hashlib.sha256(super_wizard_bytes).hexdigest()}
    assert super_bowler_delta["parent_reference"] == {"file": SUPER_MINION_DELTA.name, "reference_id": super_minion_delta["reference_id"], "sha256": hashlib.sha256(super_minion_bytes).hexdigest()}
    assert super_dragon_delta["parent_reference"] == {"file": SUPER_BOWLER_DELTA.name, "reference_id": super_bowler_delta["reference_id"], "sha256": hashlib.sha256(super_bowler_bytes).hexdigest()}
    assert super_barbarian_delta["game_version"] == (
        "The immutable level payload is source commit 62b019df868f9decb49e97a0ff5ae15fb27af9ba. "
        "Neither the official launch note nor the community modifier table identifies a current game/client build; "
        "that version remains explicitly unknown rather than inferred."
    )
    super_barbarian_sources = {source["id"]: source for source in super_barbarian_delta["sources"]}
    assert super_barbarian_sources["supercell-2020-03-30-super-barbarian-rage"]["version"] == "not published by the official launch note"
    assert super_barbarian_sources["fandom-2026-09-18-super-barbarian"]["version"] == "not published by the community table"
    assert FROZEN_REFERENCE["reference_id"] == super_dragon_delta["reference_id"]
    assert FROZEN_REFERENCE["reference_chain"]["ids"] == [
        parent["reference_id"], delta["reference_id"], super_wall_breaker_delta["reference_id"],
        super_barbarian_delta["reference_id"], super_giant_delta["reference_id"], super_archer_delta["reference_id"], rocket_balloon_delta["reference_id"], inferno_dragon_delta["reference_id"], super_wizard_delta["reference_id"], super_minion_delta["reference_id"], super_bowler_delta["reference_id"], super_dragon_delta["reference_id"]
    ]
    source_ids = {source["id"] for source in FROZEN_REFERENCE["sources"]}
    assert {"goblinsfarm-2026-09-16-monolith", "fandom-2026-09-18-monolith"} <= source_ids
    overrides = {
        (item["content_id"], item["level"]): item["fields"]["monolith_bonus_damage_percent"]
        for item in FROZEN_REFERENCE["catalogue_overrides"]
        if item["content_id"] == "monolith"
    }
    assert [overrides[("monolith", level)]["value"] for level in range(1, 6)] == [11, 12, 13, 14, 15]
    assert {overrides[("monolith", level)]["source"] for level in range(1, 6)} == {
        "fandom-2026-09-18-monolith"
    }
    extra = next(item for item in FROZEN_REFERENCE["catalogue_extra_levels"]
                 if item["content_id"] == "monolith" and item["level"] == 5)
    assert extra["observation_file"] == OBSERVATION.name
    assert extra["observation_sha256"] == hashlib.sha256(observation_bytes).hexdigest()
    level = observation["observations"][0]
    assert level["content_id"] == "monolith" and level["level"] == 5
    assert level["fields"] == {
        "hitpoints": 5959, "dps": 225, "upgrade_cost": 470000,
        "upgrade_cost_resource": "Dark Elixir", "upgrade_time_seconds": 1296000,
        "town_hall_required": 18,
    }
    assert level["derived_fields"]["damage_per_shot"]["value"] == 337.5
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    monolith = next(content for content in catalogue["contents"] if content["id"] == "monolith")
    imported = next(item for item in monolith["levels"]
                    if item["level"] == 5 and item.get("variant") == "normal")
    assert (imported["hitpoints"], imported["damage_per_shot"], imported["dps"],
            imported["upgrade_cost"], imported["town_hall_required"]) == (5959, 337.5, 225, 470000, 18)
    imported_bonus = {
        item["level"]: item["monolith_bonus_damage_percent"]
        for item in monolith["levels"]
        if item.get("variant") == "normal"
    }
    assert [imported_bonus[level] for level in range(1, 6)] == [11, 12, 13, 14, 15]
    source = imported["provenance"]["source"]
    assert source["source"] == "goblinsfarm-2026-09-16-monolith"
    assert source["raw_sha256"] == extra["observation_sha256"]
    super_wall_breaker = next(content for content in catalogue["contents"] if content["id"] == "super_wall_breaker")
    derived = super_wall_breaker["field_provenance"]
    assert (super_wall_breaker["support"], super_wall_breaker["wall_damage_multiplier"],
            super_wall_breaker["splash_radius_tiles"], super_wall_breaker["death_splash_radius_tiles"],
            super_wall_breaker["self_destruct_on_attack"]) == (
                "core_supported_secondary_partial", 40, 2, 2, True)
    assert {derived[field]["source"] for field in (
        "wall_damage_multiplier", "splash_radius_tiles", "death_splash_radius_tiles")} == {
            "fandom-2026-09-17-wall-breaker"}
    assert derived["self_destruct_on_attack"]["source"] == "supercell-2020-03-30-spring-super-troops"
    super_barbarian = next(content for content in catalogue["contents"] if content["id"] == "super_barbarian")
    assert (super_barbarian["support"], super_barbarian["deployment_rage_duration_seconds"],
            super_barbarian["deployment_rage_damage_multiplier"],
            super_barbarian["deployment_rage_movement_speed_increase"]) == (
                "core_supported_secondary_partial", 8, 1.7, 16)
    barbarian_provenance = super_barbarian["field_provenance"]
    assert barbarian_provenance["deployment_rage_duration_seconds"]["source"] == "supercell-2020-03-30-super-barbarian-rage"
    assert {barbarian_provenance[field]["source"] for field in (
        "deployment_rage_damage_multiplier", "deployment_rage_movement_speed_increase")} == {
        "fandom-2026-09-18-super-barbarian"}
    super_giant = next(content for content in catalogue["contents"] if content["id"] == "super_giant")
    assert (super_giant["support"], super_giant["wall_damage_multiplier"]) == ("core_supported_secondary_partial", 5)
    assert super_giant["field_provenance"]["wall_damage_multiplier"]["source"] == "clashpost-2026-09-18-super-giant"
    super_archer = next(content for content in catalogue["contents"] if content["id"] == "super_archer")
    assert (super_archer["support"], super_archer["piercing_projectile_range_tiles"]) == ("core_supported_secondary_partial", 12)
    assert super_archer["field_provenance"]["piercing_projectile_range_tiles"]["source"] == "fandom-2026-09-18-super-archer"
    rocket_balloon = next(content for content in catalogue["contents"] if content["id"] == "rocket_balloon")
    assert (rocket_balloon["support"], rocket_balloon["deployment_rage_duration_seconds"],
            rocket_balloon["deployment_rage_damage_multiplier"],
            rocket_balloon["deployment_rage_movement_speed_increase"]) == (
                "core_supported_secondary_partial", 4, 1, 52)
    rocket_provenance = rocket_balloon["field_provenance"]
    assert rocket_provenance["deployment_rage_duration_seconds"]["source"] == "supercell-2021-06-12-rocket-balloon-video"
    assert rocket_provenance["deployment_rage_movement_speed_increase"]["source"] == "fandom-2026-09-18-rocket-balloon"
    inferno_dragon = next(content for content in catalogue["contents"] if content["id"] == "inferno_dragon")
    assert (inferno_dragon["support"], inferno_dragon["inferno_ramp"]) == (
        "core_supported_secondary_partial", {"stage_two_after_seconds": 1.7, "stage_three_after_seconds": 3.2})
    assert inferno_dragon["field_provenance"]["inferno_ramp"]["source"] == "fandom-2026-09-18-inferno-dragon"
    assert {source["id"] for source in inferno_dragon_delta["sources"]} == {
        "supercell-2021-09-24-inferno-dragon-ramp", "supercell-2021-04-12-inferno-dragon-cooldown", "fandom-2026-09-18-inferno-dragon"}
    super_wizard = next(content for content in catalogue["contents"] if content["id"] == "super_wizard")
    assert (super_wizard["support"], super_wizard["chain_damage_multiplier"], super_wizard["chain_target_count"], super_wizard["chain_radius_tiles"]) == ("core_supported_secondary_partial", .4, 10, 3)
    super_minion = next(content for content in catalogue["contents"] if content["id"] == "super_minion")
    assert (super_minion["support"], super_minion["opening_long_shot_count"], super_minion["opening_long_shot_range_tiles"], super_minion["opening_long_shot_damage_multiplier"]) == ("core_supported_primary_range_partial", 8, 10.25, 1.0)
    assert super_minion["field_provenance"]["opening_long_shot_count"]["source"] == "supercell-2026-01-28-super-minion-balance"
    super_bowler = next(content for content in catalogue["contents"] if content["id"] == "super_bowler")
    level_ten = next(level for level in super_bowler["levels"] if level["level"] == 10)
    assert (super_bowler["support"], super_bowler["bounce_impact_count"],
            super_bowler["bounce_step_tiles"], super_bowler["bounce_splash_radius_tiles"],
            level_ten["hitpoints"], level_ten["damage_per_shot"], level_ten["dps"]) == (
                "core_supported_primary_secondary_partial", 3, 3, .6, 3700, 682, 310)
    assert set(level_ten["provenance"]["overridden_fields"]) == {"damage_per_shot", "dps", "hitpoints"}
    assert super_bowler["field_provenance"]["bounce_impact_count"]["source"] == "supercell-2021-09-25-super-bowler-launch"
    super_dragon = next(content for content in catalogue["contents"] if content["id"] == "super_dragon")
    level_thirteen = next(level for level in super_dragon["levels"] if level["level"] == 13)
    assert (super_dragon["support"], super_dragon["attack_speed_seconds"], level_thirteen["hitpoints"], level_thirteen["dps"], level_thirteen["damage_per_shot"]) == ("core_supported_primary_secondary_lower_bound", 1.8, 8400, 537, 966.6)
    assert super_dragon["field_provenance"]["attack_speed_seconds"]["source"] == "goblinsfarm-2026-08-26-super-dragon"
    print("Active TH18 reference validation passed: all active deltas through Super Dragon are hash-pinned")


if __name__ == "__main__":
    main()
