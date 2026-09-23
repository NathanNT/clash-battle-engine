#!/usr/bin/env python3
"""Verify the Headless read-only Hero support catalogue against pinned data."""
from __future__ import annotations

import argparse
import json
import pathlib
import subprocess
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[1]


def expected_rows() -> list[dict]:
    document = json.loads((ROOT / "data/catalogue.normalized.json").read_text(encoding="utf-8"))
    rows = []
    for content in document["contents"]:
        if content["category"] not in ("pet", "hero-equipment"):
            continue
        hero = content.get("source_fields", {}).get("hero") or content.get("hero", "")
        rows.append({"id": content["id"], "category": content["category"],
                     "hero": hero.replace("-", "_"),
                     "max_level": max(level["level"] for level in content["levels"]),
                     "support": content["support"],
                     "ability_type": ("not_applicable" if content["category"] == "pet"
                                      else content.get("equipment_activation_type") or
                                      content.get("source_fields", {}).get("abilityType", "unverified"))})
    return rows


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", required=True)
    args = parser.parse_args()
    output = subprocess.check_output([args.exe, "hero-support-catalogue"], text=True)
    actual = json.loads(output)
    assert actual["combat_status"] == "catalogued_only", actual
    assert actual["items"] == expected_rows()
    assert len(actual["items"]) == 54
    monolith = next(row for row in actual["items"] if row["id"] == "monolith_arrow")
    assert monolith["hero"] == "archer_queen" and monolith["ability_type"] == "Passive"

    contents = {content["id"]: content for content in
                json.loads((ROOT / "data/catalogue.normalized.json").read_text(encoding="utf-8"))["contents"]}
    assert contents["monolith_arrow"]["field_provenance"]["equipment_activation_type"]["source"] == (
        "clash-ninja-2026-06-03-monolith-arrow-passive")
    primary_max_values = {
        "royal_gem": {"wielder_hp_bonus": 570, "wielder_dps_bonus": 120},
        "earthquake_boots": {"radius_tiles": 8, "building_damage_percent": 40},
        "vampstache": {"wielder_dps_bonus": 120},
        "barbarian_puppet": {"wielder_hp_bonus": 3366, "active_hp_recovery": 1386},
        "archer_puppet": {"wielder_dps_bonus": 159, "active_hp_recovery": 484,
                           "spawned_archer_invisibility_ms": 6500},
        "electro_boots": {"aura_dps": 200, "self_heal_per_second": 45},
    }
    for content_id in (row["id"] for row in actual["items"]):
        content = contents[content_id]
        level = content["levels"][-1]
        record = json.loads(subprocess.check_output(
            [args.exe, "hero-support-level", content_id, str(level["level"])], text=True))
        assert record == {"combat_status": "catalogued_only", "content": content,
                          "level_record": level}
        if content_id == "healing_tome":
            assert record["level_record"]["official_duration_evidence"]["duration_ms"] == 20000
            assert record["level_record"]["official_duration_evidence"]["source_id"] == "supercell-2024-08-27-zh-healing-tome-duration"
        if content_id in primary_max_values:
            assert record["level_record"]["primary_level_evidence"]["values"] == primary_max_values[content_id]
    invalid = subprocess.run([args.exe, "hero-support-level", "lassi", "999"],
                             capture_output=True, text=True, check=False)
    assert invalid.returncode == 2 and "unknown Hero support" in invalid.stderr

    projected = subprocess.run(
        [args.exe, "simulate", str(ROOT / "scenarios/archer-queen-baseline.json"),
         "--deploy", "archer_queen", "4.5", "10.5", "16", "--until", "20"],
        capture_output=True, text=True, check=False)
    assert projected.returncode == 3, projected.stderr
    assert "monolith_arrow_housing_projection=25 tier_projection=0 damage_percent_projection=14" in projected.stdout, projected.stdout

    selected = subprocess.run(
        [args.exe, "simulate", str(ROOT / "scenarios/hero-loadout-selection.json"),
         "--deploy", "archer_queen", "2.5", "2.5", "16", "--until", "20"],
        capture_output=True, text=True, check=False)
    assert selected.returncode == 3, selected.stderr
    loadout_line = next(line for line in selected.stdout.splitlines()
                        if line.startswith("hero_loadouts="))
    loadouts = json.loads(loadout_line.removeprefix("hero_loadouts="))
    assert len(loadouts) == 2
    assert loadouts[0]["pet"] == {"id": "lassi", "level": 15}
    assert [choice["id"] for choice in loadouts[0]["equipment"]] == ["giant_arrow", "monolith_arrow"]

    hero_levels = {"barbarian_king": 110, "archer_queen": 110,
                   "grand_warden": 85, "royal_champion": 55,
                   "minion_prince": 95, "dragon_duke": 25}
    pets = [row for row in actual["items"] if row["category"] == "pet"]
    equipment = {hero: [row for row in actual["items"]
                        if row["category"] == "hero-equipment" and row["hero"] == hero]
                 for hero in hero_levels}
    checked_pets = set()
    checked_equipment = set()
    with tempfile.TemporaryDirectory(prefix="cocsim-hero-support-") as directory:
        temporary = pathlib.Path(directory)
        batch = 0
        for hero, rows in equipment.items():
            for offset in range(0, len(rows), 2):
                selected = rows[offset:offset + 2]
                loadout = {"hero": hero, "equipment": [
                    {"id": row["id"], "level": row["max_level"]} for row in selected]}
                checked_equipment.update(row["id"] for row in selected)
                if batch < len(pets):
                    pet = pets[batch]
                    loadout["pet"] = {"id": pet["id"], "level": pet["max_level"]}
                    checked_pets.add(pet["id"])
                scenario = {
                    "format_version": 3, "width": 30, "height": 25,
                    "seed": 230923, "duration_ms": 3000,
                    "defenders": [{"kind": "gold_storage", "level": 1,
                                   "x": 25.5, "y": 20.5}],
                    "army": [{"kind": hero, "level": hero_levels[hero], "count": 1}],
                    "hero_loadouts": [loadout],
                }
                scenario_path = temporary / f"selection-{batch}.json"
                replay_path = temporary / f"selection-{batch}-replay.json"
                scenario_path.write_text(json.dumps(scenario), encoding="utf-8")
                command = [args.exe, "simulate", str(scenario_path),
                           "--deploy", hero, "2.5", "2.5", "16",
                           "--until", "20", "--save-replay", str(replay_path)]
                simulated = subprocess.run(command, capture_output=True, text=True, check=False)
                assert simulated.returncode == 3, (hero, selected, simulated.stderr)
                line = next(line for line in simulated.stdout.splitlines()
                            if line.startswith("hero_loadouts="))
                assert json.loads(line.removeprefix("hero_loadouts=")) == [loadout], loadout
                replayed = subprocess.run([args.exe, "replay", str(replay_path), "--until", "20"],
                                          capture_output=True, text=True, check=False)
                assert replayed.returncode == 3 and replayed.stdout == simulated.stdout, (
                    hero, selected, replayed.stderr)
                batch += 1
    assert checked_pets == {row["id"] for row in pets}
    assert checked_equipment == {row["id"] for rows in equipment.values() for row in rows}
    assert len(checked_pets) == 12 and len(checked_equipment) == 42
    print(f"Headless Hero support catalogue and {batch} fixed-tick replays validated (12 Pets, 42 Equipment)")


if __name__ == "__main__":
    main()
