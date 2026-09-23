#!/usr/bin/env python3
"""Run one shared core/RL scenario when the pybind module is built.

This deliberately drives ``CoCSimEnv.step`` rather than calling the binding
directly: it guards the action-index mapping, scenario adapter and observation
kind table together.
"""
from __future__ import annotations

import argparse
import json
import math
import pathlib
import sys


ROOT = pathlib.Path(__file__).resolve().parents[1]


def assert_runtime_catalogue(module) -> None:
    """Compare every core-visible JSON level to the actual GameData object.

    This is deliberately run through the binding, not by reimplementing a C++
    loader in Python. It catches a normal/supercharged selector, unit conversion
    or field-wiring drift for all presently integrated buildings and troops.
    """
    catalogue = json.loads((ROOT / "data" / "catalogue.normalized.json").read_text(encoding="utf-8"))
    actual = {(row["kind"], row["level"], row["supercharged"]): row
              for row in module.catalogue_stats()}

    def equal(actual_value: float, expected_value: float, label: str) -> None:
        assert math.isclose(actual_value, expected_value, rel_tol=0.0, abs_tol=1e-9), (
            label, actual_value, expected_value)

    def quantized_ms(seconds: float) -> int:
        return int(math.floor(seconds * 1000.0 / module.tick_ms + 0.5)) * module.tick_ms

    checked = 0
    for content in catalogue["contents"]:
        for level in content.get("levels", []):
            variant = level.get("variant", "normal")
            key = (content["id"], level["level"], variant == "supercharged")
            row = actual.get(key)
            if row is None:
                continue  # content intentionally outside the current core enum
            label = f"{content['id']} {variant} L{level['level']}"
            if level.get("hitpoints") is not None:
                equal(row["hp"], level["hitpoints"], label + " hp")
            source_damage = level.get("damage_per_shot", level.get("damage"))
            if source_damage is None:
                source_damage = level.get("damage")
            weapon = level.get("weapon") or {}
            weapon_level = next(iter(weapon.get("levels", [])), {})
            if source_damage is None:
                source_damage = weapon_level.get("damagePerHit")
            equal(row["damage"], source_damage or 0.0, label + " damage")
            equal(row["dps"], level.get("dps") or weapon_level.get("dps") or 0.0, label + " dps")
            equal(row["healing_per_second"], level.get("healing_per_second") or 0.0, label + " healing")
            expected_cooldown = quantized_ms(
                weapon.get("attackSpeed") or content.get("attack_speed_seconds") or 0.0
            )
            # Yetimite's public contract omits an attack cadence. Its Core
            # self-destruction still requires one deterministic scheduling
            # boundary, so it uses one fixed logical tick rather than a
            # fabricated calibrated cooldown.
            if content.get("self_destruct_on_attack") and expected_cooldown == 0:
                expected_cooldown = module.tick_ms
            assert row["cooldown_ms"] == expected_cooldown, label + " cooldown"
            assert row["first_burst_delay_ms"] == quantized_ms(
                content.get("first_burst_delay_seconds") or 0.0
            ), label + " first burst delay"
            burst_mode_id = "fastAttack" if content["id"] == "multi_gear_tower" else "gearedUpBurst"
            burst_mode = next(
                (mode for mode in content.get("modes", [])
                 if mode.get("id") == burst_mode_id),
                None,
            )
            burst_level = (level.get("mode_stats") or {}).get(burst_mode_id)
            burst_damage = (burst_level or {}).get("damage_per_shot") or 0.0
            burst_available = bool(burst_mode and burst_damage > 0.0)
            assert row["geared_up_burst_available"] == burst_available, (
                label + " geared-up burst availability"
            )
            equal(row["geared_up_burst_damage"], burst_damage,
                  label + " geared-up burst damage")
            assert row["geared_up_burst_cooldown_ms"] == quantized_ms(
                (burst_mode or {}).get("attack_speed_seconds") or 0.0
            ), label + " geared-up burst cooldown"
            assert row["geared_up_burst_shots"] == (
                (burst_mode or {}).get("shots_per_burst") or 0
            ), label + " geared-up burst shots"
            assert row["geared_up_burst_pause_ms"] == quantized_ms(
                (burst_mode or {}).get("time_between_bursts_seconds") or 0.0
            ), label + " geared-up burst pause"
            equal(row["geared_up_burst_range_tiles"],
                  (burst_mode or {}).get("range_tiles") or 0.0,
                  label + " geared-up burst range")
            equal(row["range_tiles"], weapon.get("range") or content.get("range_tiles") or 0.0, label + " range")
            equal(row["movement_speed"], content.get("movement_speed") or 0.0, label + " speed")
            equal(row["projectile_speed_tiles_per_second"], content.get("projectile_speed_tiles_per_second") or 0.0, label + " projectile speed")
            assert row["projectile_speed_sourced"] == isinstance(
                content.get("projectile_speed_tiles_per_second"), (int, float)
            ), label + " projectile speed provenance"
            equal(row["min_range_tiles"], content.get("min_range_tiles") or 0.0, label + " min range")
            equal(row["trigger_range_tiles"], content.get("trigger_range_tiles") or 0.0, label + " trigger range")
            equal(row["reveal_destruction_percent"], content.get("reveal_destruction_percent") or 0.0, label + " reveal destruction")
            equal(row["death_damage"], weapon.get("deathDamage") or level.get("death_damage") or 0.0, label + " death damage")
            equal(row["death_splash_radius_tiles"], weapon.get("deathDamageRadius") or content.get("death_splash_radius_tiles") or 0.0, label + " death radius")
            assert row["death_damage_delay_ms"] == quantized_ms(
                content.get("death_damage_delay_seconds") or 0.0
            ), label + " death delay"
            assert row["death_damage_ground_only"] == bool(
                content.get("death_damage_ground_only")
            ), label + " death target filter"
            equal(row["wall_damage_multiplier"], level.get("wall_damage_multiplier") or content.get("wall_damage_multiplier") or 1.0, label + " wall multiplier")
            equal(row["wall_damage_per_hit"], content.get("wall_damage_per_hit") or 0.0, label + " wall damage per hit")
            assert row["wall_attack_cooldown_ms"] == quantized_ms(
                content.get("wall_attack_speed_seconds") or 0.0
            ), label + " wall attack cooldown"
            assert row["smashes_walls"] == bool(content.get("smashes_walls")), label + " smashes walls"
            equal(row["resource_damage_multiplier"], content.get("resource_damage_multiplier") or 1.0, label + " resource multiplier")
            equal(row["damage_spawn_threshold"], content.get("damage_spawn_threshold") or 0.0,
                  label + " damage spawn threshold")
            equal(row["defense_damage_multiplier"], content.get("defense_damage_multiplier") or 1.0,
                  label + " defense multiplier")
            assert row["jumps_walls"] == bool(content.get("jumps_walls")), label + " jumps walls"
            equal(row["isolation_radius_tiles"], content.get("isolation_radius_tiles") or 0.0, label + " isolation radius")
            equal(row["rage_damage_multiplier"], content.get("rage_damage_multiplier") or 1.0, label + " rage damage multiplier")
            equal(row["rage_attack_speed_multiplier"], content.get("rage_attack_speed_multiplier") or 1.0, label + " rage attack speed multiplier")
            assert row["burrows"] == bool(content.get("burrows")), label + " burrows"
            assert row["defense_invisibility_duration_ms"] == quantized_ms(
                content.get("defense_invisibility_duration_seconds") or 0.0
            ), label + " defense invisibility duration"
            assert row["deployment_rage_duration_ms"] == quantized_ms(
                content.get("deployment_rage_duration_seconds") or 0.0
            ), label + " deployment rage duration"
            equal(row["deployment_rage_damage_multiplier"],
                  content.get("deployment_rage_damage_multiplier") or 1.0,
                  label + " deployment rage damage multiplier")
            speed_increase = content.get("deployment_rage_movement_speed_increase") or 0.0
            base_speed = content.get("movement_speed") or 0.0
            expected_movement_multiplier = (base_speed + speed_increase) / base_speed if base_speed else 1.0
            equal(row["deployment_rage_movement_speed_multiplier"], expected_movement_multiplier,
                  label + " deployment rage movement multiplier")
            assert row["self_destruct_on_attack"] == bool(content.get("self_destruct_on_attack")), label
            assert row["target_focus"] == (content.get("target_focus") or "any"), label + " focus"
            source_target_type = weapon.get("targetType") or content.get("target_type") or "ground"
            # The X-Bow has two separately replayable modes. Its normal mode
            # is ground-only; air+ground is selected by Placement.mode.
            if content["id"] == "x_bow":
                source_target_type = "ground"
            assert row["target_type"] == source_target_type, label + " target type"
            assert row["flying"] == bool(content.get("flying")), label + " flying"
            assert row["heals"] == bool(content.get("heals")), label + " heals"
            assert row["ranged"] == (
                (weapon.get("range") or content.get("range_tiles") or 0.0) > 1.0
                and not bool(content.get("heals"))
            ), label + " ranged"
            assert row["splash"] == (content.get("damage_type") == "splash"), label + " splash"
            assert row["housing_space"] == int(content.get("housing_space") or 0), label + " housing"
            assert row["activation_housing_space"] == int(content.get("activation_housing_space") or 0), label + " activation"
            assert row["spring_capacity"] == int(level.get("spring_capacity") or 0), label + " spring capacity"
            equal(row["splash_radius_tiles"], level.get("damage_radius_tiles") or content.get("splash_radius_tiles") or 0.0, label + " splash")
            footprint = content.get("footprint_tiles")
            if footprint:
                width, height = map(int, footprint.split("x"))
                assert (row["footprint_width"], row["footprint_height"]) == (width, height), label + " footprint"
            checked += 1
    assert checked == len(actual), (checked, len(actual))


def assert_runtime_spell_catalogue(module) -> None:
    """Verify the exact immutable spell rows used by the shared Core."""
    catalogue = json.loads((ROOT / "data" / "catalogue.normalized.json").read_text(encoding="utf-8"))
    actual = {(row["kind"], row["level"]): row for row in module.catalogue_spells()}

    def number(value: float | None, default: float = 0.0) -> float:
        return default if value is None else value

    checked = 0
    for spell in catalogue["spells"]:
        pulses = int(number(spell.get("number_of_pulses"), 1.0))
        interval_ms = int(round(number(spell.get("time_between_pulses_seconds")) * 1000.0))
        boost_ms = int(round(number(spell.get("boost_time_seconds")) * 1000.0))
        for level in spell.get("levels", []):
            row = actual.get((spell["id"], level["level"]))
            if row is None:
                continue  # Imported data outside the current shared spell enum.
            declared_ms = int(round(number(level.get("spell_duration_seconds"), boost_ms / 1000.0) * 1000.0))
            expected_duration = max(declared_ms, pulses * interval_ms)
            assert math.isclose(row["radius_tiles"], number(level.get("radius_tiles"), number(spell.get("radius_tiles")))), row
            assert math.isclose(row["damage"], number(level.get("damage"))), row
            assert math.isclose(row["total_healing"], number(level.get("total_healing"))), row
            assert math.isclose(row["damage_multiplier"], number(level.get("damage_increase"), 100.0) / 100.0), row
            assert math.isclose(row["speed_multiplier"], 1.0 + number(level.get("speed_increase")) / 100.0), row
            assert row["duration_ms"] == expected_duration, row
            assert row["pulses"] == pulses and row["pulse_interval_ms"] == interval_ms, row
            assert math.isclose(row["building_damage_percent"], number(level.get("building_damage_percent"))), row
            assert math.isclose(row["troop_damage_percent"], number(level.get("troop_damage_percent"))), row
            checked += 1
    assert checked == len(actual), (checked, len(actual))


def assert_hero_support_catalogue(module) -> None:
    """The RL binding exposes Core catalogue metadata without combat actors."""
    catalogue = json.loads((ROOT / "data" / "catalogue.normalized.json").read_text(encoding="utf-8"))
    expected = []
    for content in catalogue["contents"]:
        if content["category"] not in ("pet", "hero-equipment"):
            continue
        hero = content.get("source_fields", {}).get("hero") or content.get("hero", "")
        expected.append({"id": content["id"], "category": content["category"],
                         "hero": hero.replace("-", "_"),
                         "max_level": max(level["level"] for level in content["levels"]),
                         "support": content["support"],
                         "ability_type": ("not_applicable" if content["category"] == "pet"
                                          else content.get("equipment_activation_type") or
                                          content.get("source_fields", {}).get("abilityType", "unverified"))})
    assert module.hero_support_catalogue() == expected
    assert len(expected) == 54
    monolith = next(row for row in expected if row["id"] == "monolith_arrow")
    assert monolith["hero"] == "archer_queen" and monolith["ability_type"] == "Passive"
    contents = {content["id"]: content for content in catalogue["contents"]}
    primary_max_values = {
        "royal_gem": {"wielder_hp_bonus": 570, "wielder_dps_bonus": 120},
        "earthquake_boots": {"radius_tiles": 8, "building_damage_percent": 40},
        "vampstache": {"wielder_dps_bonus": 120},
        "barbarian_puppet": {"wielder_hp_bonus": 3366, "active_hp_recovery": 1386},
        "archer_puppet": {"wielder_dps_bonus": 159, "active_hp_recovery": 484,
                           "spawned_archer_invisibility_ms": 6500},
        "electro_boots": {"aura_dps": 200, "self_heal_per_second": 45},
    }
    for content_id in ("lassi", "giant_arrow", "monolith_arrow", "healing_tome", "greedy_raven", *primary_max_values):
        content = contents[content_id]
        level = content["levels"][-1]
        assert module.hero_support_level(content_id, level["level"]) == {
            "combat_status": "catalogued_only", "content": content,
            "level_record": level}
        if content_id == "healing_tome":
            assert level["official_duration_evidence"]["duration_ms"] == 20000
            assert level["official_duration_evidence"]["source_id"] == "supercell-2024-08-27-zh-healing-tome-duration"
        if content_id in primary_max_values:
            assert level["primary_level_evidence"]["values"] == primary_max_values[content_id]
    try:
        module.hero_support_level("lassi", 999)
    except ValueError:
        pass
    else:
        raise AssertionError("invalid Hero support level was accepted")


def assert_all_hero_support_loadouts(module) -> None:
    """Every catalogued selection survives the Python-to-Core scenario path."""
    hero_levels = {"barbarian_king": 110, "archer_queen": 110,
                   "grand_warden": 85, "royal_champion": 55,
                   "minion_prince": 95, "dragon_duke": 25}
    catalogue = json.loads((ROOT / "data" / "catalogue.normalized.json").read_text(encoding="utf-8"))
    contents = {content["id"]: content for content in catalogue["contents"]}
    checked = set()
    for row in module.hero_support_catalogue():
        content = contents[row["id"]]
        level = max(record["level"] for record in content["levels"])
        assert module.hero_support_level(row["id"], level) == {
            "combat_status": "catalogued_only", "content": content,
            "level_record": next(record for record in content["levels"]
                                 if record["level"] == level)}
        hero = row["hero"] if row["category"] == "hero-equipment" else "archer_queen"
        choice = {"id": row["id"], "level": level}
        loadout = {"hero": hero,
                   "equipment": [] if row["category"] == "pet" else [choice]}
        if row["category"] == "pet":
            loadout["pet"] = choice
        battle = module.NativeBattle()
        battle.reset_scenario({
            "width": 30, "height": 25, "duration_ms": 3000,
            "defenders": [{"kind": "gold_storage", "level": 1, "x": 25.5, "y": 20.5}],
            "army": [{"kind": hero, "level": hero_levels[hero], "count": 1}],
            "spells": [], "hero_loadouts": [loadout],
        }, 23)
        assert battle.observation()["hero_loadouts"] == [loadout], row["id"]
        assert battle.deploy(hero, 2.5, 2.5), row["id"]
        battle.advance(2)
        observed = battle.observation()
        assert observed["time_ms"] == 2 * module.tick_ms and observed["hero_loadouts"] == [loadout], row["id"]
        assert any(entity["kind"] == hero and entity["side"] == "attacker"
                   for entity in observed["entities"]), row["id"]
        checked.add(row["id"])
    assert len(checked) == 54


def assert_hero_modes_and_banner_variants(module) -> None:
    """Python preserves the same sourced Hero mode and visual values as Core."""
    base = {
        "width": 30, "height": 25, "duration_ms": 3000,
        "defenders": [{"kind": "gold_storage", "level": 1, "x": 25.5, "y": 20.5}],
        "spells": [],
    }
    mode_hashes = {}
    for mode, expected_flying in (("ground", False), ("air", True)):
        battle = module.NativeBattle()
        battle.reset_scenario({**base, "army": [
            {"kind": "grand_warden", "level": 85, "count": 1, "mode": mode}]}, 31)
        assert battle.deploy("grand_warden", 2.5, 10.5)
        battle.advance(2)
        observed = battle.observation()
        warden = next(entity for entity in observed["entities"]
                      if entity["kind"] == "grand_warden" and entity["side"] == "attacker")
        assert warden["level"] == 85 and warden["flying"] is expected_flying
        assert observed["time_ms"] == 2 * module.tick_ms
        mode_hashes[mode] = observed["state_hash"]
    assert mode_hashes["air"] != mode_hashes["ground"]

    banner_hashes = []
    for variant in ("barbarian_king", "archer_queen", "grand_warden",
                    "royal_champion", "minion_prince", "dragon_duke"):
        battle = module.NativeBattle()
        battle.reset_scenario({**base, "army": [], "non_combat_obstacles": [
            {"kind": "hero_banner", "x": 12.5, "y": 12.5, "variant": variant}]}, 31)
        observed = battle.observation()
        assert observed["non_combat_obstacles"] == [
            {"kind": "hero_banner", "x": 12.5, "y": 12.5, "variant": variant}]
        banner_hashes.append(observed["state_hash"])
    assert len(set(banner_hashes)) == 1


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--module-dir", required=True)
    args = parser.parse_args()
    sys.path.insert(0, args.module_dir)
    sys.path.insert(0, str(ROOT / "python"))

    import _cocsim
    from cocsim_rl.env import CoCSimEnv

    assert_runtime_catalogue(_cocsim)
    assert_runtime_spell_catalogue(_cocsim)
    assert_hero_support_catalogue(_cocsim)
    assert_all_hero_support_loadouts(_cocsim)
    assert_hero_modes_and_banner_variants(_cocsim)

    housing_battle = _cocsim.NativeBattle()
    housing_battle.reset_scenario({
        "width": 40, "height": 30, "duration_ms": 3000,
        "defenders": [{"kind": "gold_storage", "level": 1, "x": 35.5, "y": 25.5}],
        "army": [{"kind": "archer_queen", "level": 110, "count": 1},
                 {"kind": "electro_dragon", "level": 9, "count": 8}],
        "spells": [],
    }, 1)
    assert housing_battle.observation()["monolith_arrow_housing_projection"] == 0
    assert housing_battle.observation()["monolith_arrow_damage_percent_projection"] == 14
    assert housing_battle.deploy("archer_queen", 1.5, 1.5)
    for index in range(6):
        assert housing_battle.deploy("electro_dragon", 2.5 + index, 1.5)
    housing_battle.advance(2)
    assert housing_battle.observation()["monolith_arrow_housing_projection"] == 205
    assert housing_battle.observation()["monolith_arrow_tier_projection"] == 1
    assert housing_battle.observation()["monolith_arrow_damage_percent_projection"] == 10
    for index in range(2):
        assert housing_battle.deploy("electro_dragon", 8.5 + index, 1.5)
    housing_battle.advance(2)
    assert housing_battle.observation()["monolith_arrow_housing_projection"] == 265
    assert housing_battle.observation()["monolith_arrow_tier_projection"] == 2
    assert housing_battle.observation()["monolith_arrow_damage_percent_projection"] == 5

    loadout_battle = _cocsim.NativeBattle()
    loadout_battle.reset_scenario({
        "width": 30, "height": 25, "duration_ms": 3000,
        "defenders": [{"kind": "gold_storage", "level": 1, "x": 25.5, "y": 20.5}],
        "army": [{"kind": "archer_queen", "level": 110, "count": 1}],
        "spells": [],
        "hero_loadouts": [{"hero": "archer_queen", "pet": {"id": "lassi", "level": 15},
                           "equipment": [{"id": "giant_arrow", "level": 18},
                                         {"id": "monolith_arrow", "level": 27}]}],
    }, 1)
    assert loadout_battle.observation()["hero_loadouts"] == [
        {"hero": "archer_queen", "pet": {"id": "lassi", "level": 15},
         "equipment": [{"id": "giant_arrow", "level": 18},
                       {"id": "monolith_arrow", "level": 27}]}
    ]

    # Instantiate every level/variant that GameData actually exposes through
    # the public Python scenario adapter. This is intentionally broader than
    # the deploy-action loop below: passive buildings and armed traps must be
    # constructible and observable through the same core path as a defense.
    instantiated = 0
    for stats in _cocsim.catalogue_stats():
        native = _cocsim.NativeBattle()
        native.reset_scenario({
            "width": 20,
            "height": 20,
            "duration_ms": 1000,
            "defenders": [{
                "kind": stats["kind"], "level": stats["level"], "x": 10, "y": 10,
                **({"mode": "ground"} if stats["kind"] == "skeleton_trap" else {}),
                **({"variant": "supercharged"} if stats["supercharged"] else {}),
            }],
            "army": [],
            "spells": [],
        }, 1)
        observed = native.observation()["entities"]
        concealed = {"hidden_tesla", "bomb", "giant_bomb", "air_bomb",
                     "seeking_air_mine", "spring_trap", "giga_bomb",
                     "tornado_trap", "skeleton_trap"}
        if stats["kind"] in concealed:
            # Concealment is the intended core observation contract; reset
            # itself has already validated the level/variant against GameData.
            assert not observed, stats
        else:
            assert len(observed) == 1, stats
            entity = observed[0]
            assert entity["kind"] == stats["kind"]
            assert entity["supercharged"] == stats["supercharged"]
            assert entity["max_hp"] == stats["hp"]
        instantiated += 1

    scenario = {
        "width": 20,
        "height": 20,
        "duration_ms": 3000,
        "defenders": [{"kind": "skeleton_trap", "level": 5, "x": 10, "y": 10, "mode": "ground"}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1}],
        "spells": [],
    }
    env = CoCSimEnv(decision_ms=96, width=20, height=20, max_entities=16)
    env.reset(seed=1, options={"scenario": scenario})
    # WAIT is index 0; the first troop/tile action is 1 + y * width + x.
    _, _, _, _, info = env.step(1 + 10 * 20 + 6)
    spawned = [entity for entity in env.native.observation()["entities"]
               if entity["kind"] == "trap_skeleton_ground"]
    assert not info["invalid_action"]
    assert len(spawned) == 5, spawned

    # A supercharged record shares a numeric level with a normal one. Verify
    # that the Python scenario adapter preserves the explicit variant selector
    # into the same core state instead of silently loading Air Defense L1.
    supercharged = {
        "width": 20,
        "height": 20,
        "duration_ms": 3000,
        "defenders": [{"kind": "air_defense", "level": 1, "x": 10, "y": 10,
                       "variant": "supercharged"}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1}],
        "spells": [],
    }
    env.reset(seed=1, options={"scenario": supercharged})
    supercharged_entity = env.native.observation()["entities"][0]
    assert supercharged_entity["supercharged"]
    assert supercharged_entity["max_hp"] == 2000

    # Metadata-only buildings are parsed by the same Scenario contract but do
    # not become combat entities. The deployment rejection is emitted by Core,
    # and the binding exposes the immutable obstacle to the RL caller.
    banner = _cocsim.NativeBattle()
    banner.reset_scenario({
        "width": 20, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "cannon", "level": 1, "x": 15.5, "y": 15.5}],
        "non_combat_obstacles": [{"kind": "hero_banner", "x": 10, "y": 10}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1}], "spells": [],
    }, 1)
    obstacle = banner.observation()["non_combat_obstacles"]
    assert obstacle == [{"kind": "hero_banner", "x": 10.0, "y": 10.0,
                         "variant": "normal"}]
    assert banner.deploy("barbarian", 8, 10)
    banner.advance(2)
    assert not any(entity["side"] == "attacker" for entity in banner.observation()["entities"])

    # A binding-level spell action must preserve its actual reserve level, not
    # silently turn every RL spell into level 1.  The active view is the same
    # immutable Core projection the Viewer receives.
    haste = _cocsim.NativeBattle()
    haste.reset_scenario({
        "width": 40, "height": 20, "duration_ms": 40000,
        "defenders": [{"kind": "gold_storage", "level": 1, "x": 35.5, "y": 10.5}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1}],
        "spells": [{"kind": "haste", "level": 7, "count": 1}],
    }, 1)
    assert haste.deploy("barbarian", 0.5, 10.5)
    assert haste.cast_spell("haste", 5.5, 10.5)
    haste.advance(2)
    active_spell = haste.observation()["spell_effects"]
    assert active_spell == [{"kind": "haste", "x": 5.5, "y": 10.5,
                             "radius_tiles": 5.0, "expires_ms": 30016}]

    # A delayed destruction effect is scheduled by Core.  RL receives the
    # exact pending state, including its T+ms boundary, but cannot mutate it.
    bomb_tower = _cocsim.NativeBattle()
    bomb_tower.reset_scenario({
        "width": 24, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "bomb_tower", "level": 1, "x": 10.5, "y": 10.5}],
        "army": [{"kind": "golem", "level": 1, "count": 1}],
        "spells": [{"kind": "lightning", "level": 1, "count": 5}],
    }, 1)
    assert bomb_tower.deploy("golem", 7.99, 10.5)
    for _ in range(5):
        assert bomb_tower.cast_spell("lightning", 10.5, 10.5)
    bomb_tower.advance(2)
    pending_death = bomb_tower.observation()["death_explosions"]
    assert pending_death == [{"x": 10.5, "y": 10.5, "radius_tiles": 2.75,
                              "impact_ms": 1024, "ground_only": True}]
    bomb_tower.advance(63)
    assert not bomb_tower.observation()["death_explosions"]

    # X-Bow mode selection is scenario data passed directly to Core. RL sees
    # the resulting shared projectile/target state; it cannot independently
    # reinterpret ground-only versus air-and-ground targeting.
    x_bow = _cocsim.NativeBattle()
    x_bow.reset_scenario({
        "width": 32, "height": 20, "duration_ms": 4000,
        "defenders": [{"kind": "x_bow", "level": 13, "mode": "air_and_ground",
                       "x": 20.5, "y": 10.5}],
        "army": [{"kind": "dragon", "level": 1, "count": 1}], "spells": [],
    }, 1)
    assert x_bow.deploy("dragon", 10.5, 10.5)
    x_bow.advance(2)
    x_bow_projectiles = x_bow.observation()["projectiles"]
    assert len(x_bow_projectiles) == 1 and x_bow_projectiles[0]["owner_id"] == 1

    # Inferno Tower mode and supercharge selection are likewise scenario data
    # consumed only by Core. RL observes the direct multi-target beam rather
    # than reconstructing a projectile or a separate ramp rule in Python.
    inferno_multi = _cocsim.NativeBattle()
    inferno_multi.reset_scenario({
        "width": 32, "height": 20, "duration_ms": 4000,
        "defenders": [{"kind": "inferno_tower", "level": 2, "mode": "multi_target",
                       "variant": "supercharged", "x": 20.5, "y": 10.5}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1}], "spells": [],
    }, 1)
    assert inferno_multi.deploy("barbarian", 12.0, 10.5)
    inferno_multi.advance(2)
    inferno_events = inferno_multi.events_since(0)["events"]
    assert any(event["type"] == "attack" and event["detail"] == "inferno beam"
               and event["value"] == 21.12 for event in inferno_events), inferno_events
    inferno_state = inferno_multi.observation()
    assert not inferno_state["projectiles"]
    assert any(entity["kind"] == "inferno_tower" and entity["supercharged"]
               for entity in inferno_state["entities"]), inferno_state

    # The same Core projectile projection is available to RL. The sole
    # currently source-quantified trajectory, Seeking Air Mine, is not a GUI
    # animation: it has a 3.5-tile/s velocity and advances in fixed ticks.
    seeking_mine = _cocsim.NativeBattle()
    seeking_mine.reset_scenario({
        "width": 20, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "seeking_air_mine", "level": 1, "x": 10.5, "y": 10.5}],
        "army": [{"kind": "dragon", "level": 1, "count": 1}], "spells": [],
    }, 1)
    assert seeking_mine.deploy("dragon", 8.5, 10.5)
    seeking_mine.advance(2)
    launched = seeking_mine.observation()["projectiles"]
    assert len(launched) == 1
    assert launched[0]["id"] > 0
    assert launched[0]["speed_tiles_per_second"] == 3.5 and launched[0]["speed_sourced"] and launched[0]["homing"]
    assert launched[0]["scheduled_impact_ms"] == 0
    assert (launched[0]["origin_x"], launched[0]["origin_y"], launched[0]["x"], launched[0]["y"]) == (10.5, 10.5, 10.5, 10.5)
    # The endpoint for a trajectory renderer is in the standard immutable
    # observation.  An RL client never reconstructs a target lookup or writes
    # to the battle state to obtain it.
    assert (launched[0]["target_x"], launched[0]["target_y"]) == (8.5, 10.5)
    launched_view = seeking_mine.projectile_views()
    assert launched_view == launched
    mine_events = seeking_mine.events_since(0)["events"]
    assert any(event["type"] == "projectile"
               and event["detail"] == "seeking air mine launched"
               and event["projectile_id"] == launched[0]["id"]
               and event["has_origin"] and event["has_target_position"]
               for event in mine_events), mine_events
    seeking_mine.advance(10)
    in_flight = seeking_mine.observation()["projectiles"]
    assert len(in_flight) == 1 and in_flight[0]["id"] == launched[0]["id"] and 8.5 < in_flight[0]["x"] < 10.5
    assert (in_flight[0]["target_x"], in_flight[0]["target_y"]) == (8.5, 10.5)
    assert seeking_mine.projectile_views() == in_flight
    seeking_mine.advance(100)
    resolved_mine_events = seeking_mine.events_since(0)["events"]
    assert any(event["type"] == "impact"
               and event["detail"] == "projectile impact"
               and event["projectile_id"] == launched[0]["id"]
               and event["has_origin"] and event["has_target_position"]
               for event in resolved_mine_events), resolved_mine_events
    assert seeking_mine.observation()["projectiles"] == []

    # An unmeasured ranged weapon is still a Core projectile, rather than a
    # hit at target-acquisition time. Its exact one-tick policy is visible to
    # RL as a scheduled impact, while a homing flight above deliberately has
    # no fabricated deadline.
    logical_cannon = _cocsim.NativeBattle()
    logical_cannon.reset_scenario({
        "width": 20, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "cannon", "level": 1, "x": 10.5, "y": 10.5}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1}], "spells": [],
    }, 1)
    assert logical_cannon.deploy("barbarian", 6.5, 10.5)
    logical_cannon.advance(2)
    logical_views = logical_cannon.observation()["projectiles"]
    assert len(logical_views) == 1
    assert logical_views[0]["speed_tiles_per_second"] == 0.0
    assert not logical_views[0]["speed_sourced"]
    assert not logical_views[0]["homing"]
    assert logical_views[0]["scheduled_impact_ms"] == 2 * _cocsim.tick_ms
    assert logical_cannon.projectile_views() == logical_views
    logical_cannon.advance(1)
    assert logical_cannon.observation()["projectiles"] == []

    # Town Hall weapons store their range and simultaneous-target count inside
    # each level's nested armament. The adapter must therefore expose the same
    # four individual Core projectiles as the GUI, rather than flattening them
    # into a direct multi-target hit.
    town_hall = _cocsim.NativeBattle()
    town_hall.reset_scenario({
        "width": 30, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "town_hall", "level": 12, "x": 15, "y": 10}],
        "army": [{"kind": "barbarian", "level": 1, "count": 4}], "spells": [],
    }, 1)
    for x, y in ((6, 10), (6, 8), (6, 12), (8, 10)):
        assert town_hall.deploy("barbarian", x, y)
    town_hall.advance(2)
    town_hall_projectiles = town_hall.observation()["projectiles"]
    assert len(town_hall_projectiles) == 4
    assert [projectile["id"] for projectile in town_hall_projectiles] == [1, 2, 3, 4]
    assert {projectile["target_id"] for projectile in town_hall_projectiles} == {2, 3, 4, 5}
    assert all(projectile["owner_id"] == 1 and not projectile["homing"]
               and projectile["scheduled_impact_ms"] == 2 * _cocsim.tick_ms
               for projectile in town_hall_projectiles)
    town_hall.advance(1)
    assert town_hall.observation()["projectiles"] == []

    # Friendly projectiles use the same Core collection, identity and event
    # contract as damage projectiles.  In particular the GUI/RL-facing heal
    # event must be attributable to the launched Healer projectile rather
    # than reconstructed from an entity lookup in Python.
    healer = _cocsim.NativeBattle()
    healer.reset_scenario({
        "width": 20, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "cannon", "level": 1, "x": 10.5, "y": 10.5}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1},
                  {"kind": "healer", "level": 1, "count": 1}], "spells": [],
    }, 1)
    assert healer.deploy("barbarian", 7.5, 10.5)
    assert healer.deploy("healer", 7.5, 11.5)
    healer.advance(3)
    healing_projectiles = [projectile for projectile in healer.observation()["projectiles"]
                           if projectile["owner_id"] == 3 and projectile["target_id"] == 2]
    assert len(healing_projectiles) == 1 and not healing_projectiles[0]["homing"]
    healing_id = healing_projectiles[0]["id"]
    healing_events = healer.events_since(0)["events"]
    assert any(event["type"] == "projectile"
               and event["detail"] == "logical healing projectile"
               and event["projectile_id"] == healing_id
               and event["has_origin"] and event["has_target_position"]
               for event in healing_events), healing_events
    healer.advance(1)
    resolved_healing_events = healer.events_since(0)["events"]
    assert any(event["type"] == "healed" and event["detail"] == "projectile heal"
               and event["projectile_id"] == healing_id
               for event in resolved_healing_events), resolved_healing_events
    assert any(event["type"] == "impact" and event["detail"] == "projectile impact"
               and event["projectile_id"] == healing_id
               for event in resolved_healing_events), resolved_healing_events

    # Direct attacks have the same read-only event cursor. The Inferno Tower
    # is a documented beam, so RL must see the Core beam event rather than a
    # fictitious projectile created for presentation.
    inferno_beam = _cocsim.NativeBattle()
    inferno_beam.reset_scenario({
        "width": 20, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "inferno_tower", "level": 1, "x": 10.5, "y": 10.5}],
        "army": [{"kind": "barbarian", "level": 1, "count": 1}], "spells": [],
    }, 1)
    initial_cursor = inferno_beam.events_since(0)
    assert initial_cursor["next_cursor"] == 0 and initial_cursor["events"] == []
    assert inferno_beam.deploy("barbarian", 6.5, 10.5)
    inferno_beam.advance(2)
    events = inferno_beam.events_since(initial_cursor["next_cursor"])
    assert any(event["type"] == "attack" and event["detail"] == "inferno beam"
               and event["actor_id"] == 1 and event["target_id"] == 2
               and event["has_origin"] and event["has_target_position"]
               and event["origin_x"] == 10.5 and event["origin_y"] == 10.5
               and event["target_x"] == 6.5 and event["target_y"] == 10.5
               for event in events["events"]), events
    assert not inferno_beam.observation()["projectiles"]
    stable_hash = inferno_beam.observation()["state_hash"]
    assert inferno_beam.events_since(events["next_cursor"])["events"] == []
    assert inferno_beam.observation()["state_hash"] == stable_hash

    # The April 2026 official timing change for a geared-up Mortar is also
    # driven by Core for RL: no opening burst may occur in its first 0.5 s.
    geared_mortar = _cocsim.NativeBattle()
    geared_mortar.reset_scenario({
        "width": 30, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "mortar", "level": 8, "x": 15, "y": 10,
                       "mode": "geared_up_burst"}],
        "army": [{"kind": "golem", "level": 1, "count": 1}], "spells": [],
    }, 1)
    assert geared_mortar.deploy("golem", 2, 10)
    geared_mortar.advance(31)
    before_delay = geared_mortar.events_since(0)["events"]
    assert not any(event["type"] == "attack" and event["actor_id"] == 1
                   for event in before_delay), before_delay
    cursor = len(before_delay)
    geared_mortar.advance(13)
    assert any(event["type"] == "attack" and event["actor_id"] == 1
               for event in geared_mortar.events_since(cursor)["events"])

    # Mode selection is also passed through the same Core constructor for RL.
    # The Cannon's sourced geared-up burst must not silently fall back to its
    # normal 0.8-second weapon: it emits four 192-ms-spaced projectiles.
    cannon_burst = _cocsim.NativeBattle()
    cannon_burst.reset_scenario({
        "width": 20, "height": 5, "duration_ms": 3000,
        "defenders": [{"kind": "cannon", "level": 7, "x": 16.5, "y": 2.5,
                       "mode": "geared_up_burst"}],
        "army": [{"kind": "golem", "level": 1, "count": 1}], "spells": [],
    }, 1)
    assert cannon_burst.deploy("golem", 8, 2.5)
    cannon_burst.advance(65)
    cannon_times = [event["time_ms"] for event in cannon_burst.events_since(0)["events"]
                    if event["type"] == "projectile" and event["actor_id"] == 1]
    assert len(cannon_times) == 4 and [later - earlier for earlier, later in
                                       zip(cannon_times, cannon_times[1:])] == [192, 192, 192]

    # Hidden Tesla's secondary-sourced 51% reveal is resolved by Core. RL only
    # observes concealment; it does not maintain a parallel destruction rule.
    tesla_threshold = _cocsim.NativeBattle()
    tesla_threshold.reset_scenario({
        "width": 420, "height": 20, "duration_ms": 1000,
        "defenders": [{"kind": "hidden_tesla", "level": 1, "x": 2.5, "y": 10.5}]
        + [{"kind": "gold_mine", "level": 1, "x": 10.5 + 4 * index, "y": 10.5}
           for index in range(100)],
        "army": [], "spells": [{"kind": "lightning", "level": 12, "count": 52}],
    }, 1)
    for index in range(50):
        assert tesla_threshold.cast_spell("lightning", 10.5 + 4 * index, 10.5)
    tesla_threshold.advance(2)
    assert not any(entity["kind"] == "hidden_tesla"
                   for entity in tesla_threshold.observation()["entities"])
    for index in range(50, 52):
        assert tesla_threshold.cast_spell("lightning", 10.5 + 4 * index, 10.5)
    tesla_threshold.advance(2)
    assert any(entity["kind"] == "hidden_tesla"
               for entity in tesla_threshold.observation()["entities"])

    # The result is a Core-derived army value rather than only a unit count.
    # Housing space distinguishes two living troops of materially different
    # combat weight and excludes spawned-only catalogue rows.
    army_value = _cocsim.NativeBattle()
    army_value.reset_scenario({
        "width": 20, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "gold_mine", "level": 1, "x": 15.5, "y": 10.5}],
        "army": [{"kind": "barbarian", "level": 13, "count": 1},
                 {"kind": "dragon", "level": 13, "count": 1}], "spells": [],
    }, 1)
    army_value_result = army_value.result()
    assert army_value_result["troops_remaining"] == 2
    assert army_value_result["remaining_housing_space"] > army_value_result["troops_remaining"], army_value_result

    # A catalogue level is part of a Core command.  The action space chooses a
    # troop kind, so the binding must select the first non-empty Core reserve
    # for that kind instead of fabricating Level 1.  This is deliberately
    # exercised through CoCSimEnv rather than by calling BattleState directly.
    from cocsim_rl.env import SPELLS, TROOPS
    level_reserve = {
        "width": 20, "height": 20, "duration_ms": 3000,
        "defenders": [{"kind": "gold_mine", "level": 1, "x": 15.5, "y": 10.5}],
        "army": [{"kind": "barbarian", "level": 13, "count": 1}], "spells": [],
    }
    env.reset(seed=4, options={"scenario": level_reserve})
    barbarian_index = TROOPS.index("barbarian")
    _, _, _, _, info = env.step(1 + barbarian_index * env.tile_count)
    assert not info["invalid_action"]
    deployed = [entity for entity in env.native.observation()["entities"]
                if entity["kind"] == "barbarian" and entity["side"] == "attacker"]
    assert len(deployed) == 1 and deployed[0]["max_hp"] == 310.0, deployed

    # Every currently exposed spell must be accepted through the actual RL
    # action index and consume the same Core reserve used by the Viewer.
    for spell_index, spell in enumerate(SPELLS):
        spell_scenario = {
            "width": 20, "height": 20, "duration_ms": 3000,
            "defenders": [{"kind": "cannon", "level": 1, "x": 12, "y": 10}],
            "army": [], "spells": [{"kind": spell, "level": 1, "count": 1}],
        }
        env.reset(seed=3, options={"scenario": spell_scenario})
        action = 1 + (len(TROOPS) + spell_index) * env.tile_count + 10 * env.width + 10
        _, _, _, _, info = env.step(action)
        assert not info["invalid_action"], spell
        remaining = env.native.availability()["spells"].get(spell)
        assert remaining == 0, (spell, remaining, env.native.observation())

    # Exercise every troop exposed by the RL action space through its actual
    # action index.  This catches a drift that a static KINDS comparison cannot
    # see (reserve availability, action masking, binding parsing or observation
    # names).
    for troop_index, troop in enumerate(TROOPS):
        # Super Troops are catalogued at their sourced boost level, not L1.
        # The test intentionally submits that exact Core level through RL.
        level = {"sneaky_goblin": 7, "super_wall_breaker": 7, "super_barbarian": 8,
                 "super_giant": 14, "super_archer": 14, "rocket_balloon": 12,
                 "inferno_dragon": 12, "super_wizard": 14, "super_minion": 14,
                 "super_bowler": 10, "super_dragon": 13, "super_hog_rider": 10,
                 "super_miner": 12, "super_valkyrie": 12, "super_yeti": 8,
                 "super_witch": 8, "ice_hound": 8, "yeti": 8,
                 "barbarian_king": 101, "archer_queen": 101,
                 "grand_warden": 76, "royal_champion": 51,
                 "minion_prince": 91, "dragon_duke": 21}.get(troop, 1)
        troop_scenario = {
            "width": 20,
            "height": 20,
            "duration_ms": 3000,
            "defenders": [{"kind": "cannon", "level": 1, "x": 12, "y": 10}],
            "army": [{"kind": troop, "level": level, "count": 1}],
            "spells": [],
        }
        env.reset(seed=2, options={"scenario": troop_scenario})
        action = 1 + troop_index * env.tile_count  # tile (0, 0)
        _, _, _, _, info = env.step(action)
        assert not info["invalid_action"], troop
        observed = env.native.observation()["entities"]
        assert any(entity["kind"] == troop and entity["side"] == "attacker"
                   for entity in observed), troop
        if troop == "miner":
            assert any(entity["kind"] == "miner" and entity["underground"]
                       for entity in observed), observed
        if troop == "super_barbarian":
            assert any(entity["kind"] == "super_barbarian" and entity["deployment_rage_active"]
                       for entity in observed), observed
        if troop == "rocket_balloon":
            assert any(entity["kind"] == "rocket_balloon" and entity["deployment_rage_active"]
                       for entity in observed), observed
    print(f"RL runtime validation passed ({instantiated} GameData scenarios + Hero Banner + Skeleton Trap + {len(TROOPS)} deployable troops)")


if __name__ == "__main__":
    main()
