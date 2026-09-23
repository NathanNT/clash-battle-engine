#!/usr/bin/env python3
"""Produce an evidence-based audit of loaded TH18 catalogue entries.

This deliberately distinguishes *loaded* from *conformant*: it reports a
missing core/GUI/RL path or non-primary numeric provenance as a gap.  It does
not manufacture an affirmative fidelity result from a complete-looking row.
"""
from __future__ import annotations

import ast
import collections
import json
import pathlib

from importer.reference import FROZEN_REFERENCE
from project_layout import (
    core_source_text,
    registered_kind_ids,
    registered_spell_ids,
    viewer_source_text,
)


ROOT = pathlib.Path(__file__).resolve().parents[1]
CATALOGUE = ROOT / "data" / "catalogue.normalized.json"
PERMANENT_SCOPE = ROOT / "data" / "reference" / "th18-permanent-scope-2026-09-18.json"
ENV = ROOT / "python" / "cocsim_rl" / "env.py"
BINDINGS = ROOT / "bindings" / "module.cpp"
CORE_TESTS = ROOT / "tests" / "core_tests.cpp"
PERMANENT_BUILDING_TESTS = ROOT / "tests" / "permanent_building_tests.cpp"
PERMANENT_VARIANT_TESTS = ROOT / "tests" / "permanent_variant_tests.cpp"
BOMB_TOWER_TESTS = ROOT / "tests" / "bomb_tower_tests.cpp"
X_BOW_TESTS = ROOT / "tests" / "x_bow_tests.cpp"
INFERNO_TOWER_TESTS = ROOT / "tests" / "inferno_tower_tests.cpp"
VIEWER_SELECTION_TESTS = ROOT / "tests" / "viewer_selection_tests.cpp"
PERMANENT_BUILDING_HEADLESS_FIXTURE = ROOT / "scenarios" / "permanent-passive-buildings.json"
BOMB_TOWER_HEADLESS_FIXTURE = ROOT / "scenarios" / "bomb-tower-baseline.json"
X_BOW_HEADLESS_FIXTURE = ROOT / "scenarios" / "x-bow-baseline.json"
INFERNO_TOWER_HEADLESS_FIXTURE = ROOT / "scenarios" / "inferno-tower-baseline.json"
RL_RUNTIME_TEST = ROOT / "tools" / "validate_rl_runtime.py"

# `audit_structure` below only proves that an item is wired through the
# catalogue/adapters.  Keep behavioural evidence deliberately opt-in: each id
# here has an assertion in a focused C++ domain test (or the frozen historical
# suite), exercised by CTest.
# It is still not a claim that the whole in-game behaviour is fidelity-checked.
CORE_BEHAVIOUR_TESTED_IDS = {
    "air_bomb", "air_defense", "archer", "archer_tower", "balloon", "barbarian", "bomb", "bomb_tower",
    "apprentice_warden", "bowler", "cannon", "dragon", "dragon_rider", "electro_dragon", "eagle_artillery", "giant", "giant_bomb", "ice_golem",
    "giga_bomb", "goblin", "golem", "golemite", "gold_mine", "gold_storage", "healer", "hidden_tesla", "hog_rider", "minion",
    "inferno_tower", "mortar", "multi_archer_tower", "multi_gear_tower", "scattershot", "seeking_air_mine", "skeleton_trap", "sneaky_goblin",
    "pekka", "root_rider", "spring_trap", "thrower", "electro_titan", "tornado_trap", "town_hall", "trap_skeleton_air", "trap_skeleton_ground", "valkyrie", "wall", "wall_breaker", "wizard", "wizard_tower",
    "x_bow",
    "army_camp", "barracks", "blacksmith", "clan_castle", "dark_barracks", "dark_elixir_drill", "dark_elixir_storage", "dark_spell_factory", "elixir_collector", "elixir_storage", "hero_hall", "laboratory", "pet_house", "spell_factory", "workshop",
    "hero_banner", "bobs_hut", "helper_hut", "crafting_station", "monolith", "super_wall_breaker", "super_barbarian", "super_giant", "super_archer", "rocket_balloon", "super_minion", "super_bowler", "super_dragon", "super_hog_rider", "super_hog", "super_rider", "super_miner", "super_valkyrie", "super_yeti", "super_witch", "ice_hound", "ice_pup", "yeti", "yetimite", "witch", "lava_hound", "lava_pup", "headhunter", "druid", "druid_bear", "super_wizard", "baby_dragon", "inferno_dragon", "miner",
}

# Snapshot/replay evidence is intentionally narrower than the behavioural set.
# A shared codec route is not proof that an individual placement survives a
# save/restore boundary: each entry here is named by the focused test and that
# test restores a snapshot and reloads a replay before comparing logical state.
ITEM_SNAPSHOT_REPLAY_TESTED_IDS = {"air_defense", "apprentice_warden", "archer", "barbarian", "bobs_hut", "bomb_tower", "bowler", "crafting_station", "dark_elixir_drill", "electro_dragon", "electro_titan", "elixir_collector", "giant", "gold_mine", "golem", "golemite", "healer", "helper_hut", "ice_golem", "inferno_tower", "monolith", "multi_archer_tower", "multi_gear_tower", "ricochet_cannon", "scattershot", "super_wizard_tower", "super_wall_breaker", "super_barbarian", "super_giant", "super_archer", "rocket_balloon", "super_minion", "super_bowler", "super_dragon", "super_hog_rider", "super_hog", "super_rider", "super_miner", "super_valkyrie", "super_yeti", "super_witch", "ice_hound", "ice_pup", "yeti", "yetimite", "wall_breaker", "witch", "wizard", "lava_hound", "lava_pup", "headhunter", "druid", "druid_bear", "thrower", "root_rider", "sneaky_goblin", "x_bow"}

# `validate_rl_runtime.py` materializes every Core-visible catalogue row through
# the pybind scenario adapter.  Keep this opt-in too: it is an executed adapter
# proof for these named permanent entities, not a claim that every registered
# Kind has been exercised in an environment where the optional Python build is
# absent.
ITEM_RL_RUNTIME_TESTED_IDS = {"air_defense", "apprentice_warden", "archer", "barbarian", "bobs_hut", "bomb_tower", "bowler", "crafting_station", "druid", "electro_dragon", "electro_titan", "giant", "golem", "healer", "helper_hut", "ice_golem", "inferno_tower", "monolith", "super_wall_breaker", "super_barbarian", "super_giant", "super_archer", "super_minion", "super_bowler", "super_dragon", "super_hog_rider", "super_hog", "super_rider", "super_miner", "super_valkyrie", "super_yeti", "super_witch", "ice_hound", "ice_pup", "lava_hound", "lava_pup", "yeti", "wall_breaker", "witch", "wizard", "headhunter", "thrower", "root_rider", "sneaky_goblin", "x_bow"}

# The viewer selection regression checks the exact Core footprint, half-open
# cell edges, attacker exclusion and entity-id tie breaking for these entries.
# Its result is a GUI geometry proof only; it says nothing about combat
# fidelity or undocumented behaviour.
ITEM_VIEWER_SELECTION_TESTED_IDS = {"air_defense", "bobs_hut", "helper_hut", "crafting_station"}

# The public CLI fixture deploys five Barbarians through its argument parser
# and must resolve both passive buildings.  This is deliberately separate from
# a direct BattleState test: it proves the headless scenario codec and command
# adapter are current too.
ITEM_HEADLESS_CLI_TESTED_IDS = {"air_defense", "apprentice_warden", "archer", "barbarian", "bobs_hut", "bomb_tower", "bowler", "crafting_station", "druid", "electro_dragon", "electro_titan", "giant", "golem", "healer", "helper_hut", "ice_golem", "inferno_tower", "monolith", "super_wall_breaker", "super_barbarian", "super_giant", "super_archer", "rocket_balloon", "super_minion", "super_bowler", "super_dragon", "super_hog_rider", "super_miner", "super_valkyrie", "super_yeti", "super_witch", "wizard", "ice_hound", "yeti", "wall_breaker", "witch", "lava_hound", "headhunter", "thrower", "root_rider", "sneaky_goblin", "x_bow"}


def cpp_kind(content_id: str) -> str:
    special = {"x_bow": "XBow", "pekka": "Pekka"}
    return special.get(content_id, "".join(part.title() for part in content_id.split("_")))


def present(value: object) -> bool:
    """`0` is a legitimate source value (for example Spring Trap L1 damage)."""
    return value is not None


def missing_fields(content: dict[str, object]) -> list[str]:
    """Return missing combat-relevant source fields, by content family.

    This is a data-completeness check, not a claim that the simulator uses every
    value faithfully. Fields with no meaning for a family are not demanded.
    """
    category = content["category"]
    levels = content.get("levels", [])
    missing: list[str] = []

    def root(*names: str) -> None:
        for name in names:
            if not present(content.get(name)):
                missing.append(name)

    def each(*names: str) -> None:
        for level in levels:
            absent = [name for name in names if not present(level.get(name))]
            if absent:
                missing.append(f"L{level['level']}:" + ",".join(absent))

    if category == "troop":
        root("housing_space", "range_tiles", "attack_speed_seconds", "movement_speed", "target_type", "target_focus", "damage_type")
        each("hitpoints")
        if content.get("heals"):
            each("healing_per_second")
        else:
            each("damage_per_shot", "dps")
    elif category == "defense":
        root("footprint_tiles", "range_tiles", "attack_speed_seconds", "target_type", "damage_type")
        each("hitpoints", "damage_per_shot", "dps")
    elif category in {"wall", "town-hall", "resource", "army", "research"}:
        root("footprint_tiles")
        each("hitpoints")
    elif category == "trap":
        root("footprint_tiles", "trigger_range_tiles", "target_type")
        content_id = content["id"]
        if content_id == "spring_trap":
            each("damage", "spring_capacity")
        elif content_id == "skeleton_trap":
            each("spawned_units", "spawned_unit_level")
        elif content_id == "tornado_trap":
            root("splash_radius_tiles")
            each("damage", "effect_duration_seconds")
        else:
            root("splash_radius_tiles")
            each("damage")
    return missing


def main() -> None:
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    reference = FROZEN_REFERENCE
    permanent_scope = json.loads(PERMANENT_SCOPE.read_text(encoding="utf-8"))
    core = core_source_text()
    # Viewer responsibilities are deliberately split between the UI entry
    # point and headers such as assets.hpp/render.hpp.  Auditing only main.cpp
    # would regress the coverage matrix after a harmless extraction.
    viewer = viewer_source_text()
    bindings = BINDINGS.read_text(encoding="utf-8")
    core_tests = CORE_TESTS.read_text(encoding="utf-8")
    test_sources = "\n".join(path.read_text(encoding="utf-8") for path in sorted((ROOT / "tests").glob("*.cpp")))
    permanent_building_tests = PERMANENT_BUILDING_TESTS.read_text(encoding="utf-8")
    air_defense_tests = (ROOT / "tests" / "air_defense_tests.cpp").read_text(encoding="utf-8")
    monolith_tests = (ROOT / "tests" / "monolith_tests.cpp").read_text(encoding="utf-8")
    super_wall_breaker_tests = (ROOT / "tests" / "super_wall_breaker_tests.cpp").read_text(encoding="utf-8")
    super_barbarian_tests = (ROOT / "tests" / "super_barbarian_tests.cpp").read_text(encoding="utf-8")
    super_giant_tests = (ROOT / "tests" / "super_giant_tests.cpp").read_text(encoding="utf-8")
    super_archer_tests = (ROOT / "tests" / "super_archer_tests.cpp").read_text(encoding="utf-8")
    rocket_balloon_tests = (ROOT / "tests" / "rocket_balloon_tests.cpp").read_text(encoding="utf-8")
    super_minion_tests = (ROOT / "tests" / "super_minion_tests.cpp").read_text(encoding="utf-8")
    super_bowler_tests = (ROOT / "tests" / "super_bowler_tests.cpp").read_text(encoding="utf-8")
    super_dragon_tests = (ROOT / "tests" / "super_dragon_tests.cpp").read_text(encoding="utf-8")
    electro_dragon_tests = (ROOT / "tests" / "electro_dragon_tests.cpp").read_text(encoding="utf-8")
    bowler_tests = (ROOT / "tests" / "bowler_tests.cpp").read_text(encoding="utf-8")
    ice_golem_tests = (ROOT / "tests" / "ice_golem_tests.cpp").read_text(encoding="utf-8")
    apprentice_warden_tests = (ROOT / "tests" / "apprentice_warden_tests.cpp").read_text(encoding="utf-8")
    super_hog_rider_tests = (ROOT / "tests" / "super_hog_rider_tests.cpp").read_text(encoding="utf-8")
    super_miner_tests = (ROOT / "tests" / "super_miner_tests.cpp").read_text(encoding="utf-8")
    super_valkyrie_tests = (ROOT / "tests" / "super_valkyrie_tests.cpp").read_text(encoding="utf-8")
    super_yeti_tests = (ROOT / "tests" / "super_yeti_tests.cpp").read_text(encoding="utf-8")
    super_witch_tests = (ROOT / "tests" / "super_witch_tests.cpp").read_text(encoding="utf-8")
    ice_hound_tests = (ROOT / "tests" / "ice_hound_tests.cpp").read_text(encoding="utf-8")
    yeti_tests = (ROOT / "tests" / "yeti_tests.cpp").read_text(encoding="utf-8")
    witch_tests = (ROOT / "tests" / "witch_tests.cpp").read_text(encoding="utf-8")
    lava_hound_tests = (ROOT / "tests" / "lava_hound_tests.cpp").read_text(encoding="utf-8")
    headhunter_tests = (ROOT / "tests" / "headhunter_tests.cpp").read_text(encoding="utf-8")
    druid_tests = (ROOT / "tests" / "druid_tests.cpp").read_text(encoding="utf-8")
    golem_tests = (ROOT / "tests" / "golem_tests.cpp").read_text(encoding="utf-8")
    wizard_tests = (ROOT / "tests" / "wizard_tests.cpp").read_text(encoding="utf-8")
    giant_tests = (ROOT / "tests" / "giant_tests.cpp").read_text(encoding="utf-8")
    archer_tests = (ROOT / "tests" / "archer_tests.cpp").read_text(encoding="utf-8")
    barbarian_tests = (ROOT / "tests" / "barbarian_tests.cpp").read_text(encoding="utf-8")
    healer_tests = (ROOT / "tests" / "healer_tests.cpp").read_text(encoding="utf-8")
    wall_breaker_tests = (ROOT / "tests" / "wall_breaker_tests.cpp").read_text(encoding="utf-8")
    electro_titan_tests = (ROOT / "tests" / "electro_titan_tests.cpp").read_text(encoding="utf-8")
    thrower_tests = (ROOT / "tests" / "thrower_tests.cpp").read_text(encoding="utf-8")
    root_rider_tests = (ROOT / "tests" / "root_rider_tests.cpp").read_text(encoding="utf-8")
    sneaky_goblin_tests = (ROOT / "tests" / "sneaky_goblin_tests.cpp").read_text(encoding="utf-8")
    inferno_tower_tests = INFERNO_TOWER_TESTS.read_text(encoding="utf-8")
    viewer_selection_tests = VIEWER_SELECTION_TESTS.read_text(encoding="utf-8")
    permanent_building_headless_fixture = json.loads(PERMANENT_BUILDING_HEADLESS_FIXTURE.read_text(encoding="utf-8"))
    rl_runtime_test = RL_RUNTIME_TEST.read_text(encoding="utf-8")
    env = ast.parse(ENV.read_text(encoding="utf-8"))
    core_kinds = set(registered_kind_ids())
    variant_supported_by_core = 'data_.find(p.kind,p.level,p.variant)' in core
    variant_supported_by_rl = 'd.contains("variant")' in bindings
    variant_supported_by_viewer = 'data.find(image_kind, candidate_level, variant)' in viewer and 'entity.supercharged ? "supercharged" : "normal"' in viewer
    rl_kinds = next(
        set(ast.literal_eval(node.value))
        for node in env.body
        if isinstance(node, ast.Assign)
        and any(isinstance(target, ast.Name) and target.id == "KINDS" for target in node.targets)
    )
    # New identifiers remain append-only in the Core enum. The RL adapter may
    # mirror that append explicitly instead of rewriting its long historical
    # literal, so include those literal append calls in the audited projection.
    rl_kinds.update({
        ast.literal_eval(node.value.args[0])
        for node in env.body
        if isinstance(node, ast.Expr) and isinstance(node.value, ast.Call)
        and isinstance(node.value.func, ast.Attribute)
        and isinstance(node.value.func.value, ast.Name) and node.value.func.value.id == "KINDS"
        and node.value.func.attr == "append" and len(node.value.args) == 1
        and isinstance(node.value.args[0], ast.Constant) and isinstance(node.value.args[0].value, str)
    })
    rl_spells = next(
        set(ast.literal_eval(node.value))
        for node in env.body
        if isinstance(node, ast.Assign)
        and any(isinstance(target, ast.Name) and target.id == "SPELLS" for target in node.targets)
    )
    core_spells = set(registered_spell_ids())
    runtime_spell_cast_coverage = (
        "for spell_index, spell in enumerate(SPELLS):" in rl_runtime_test
        and "remaining = env.native.availability()[\"spells\"].get(spell)" in rl_runtime_test
        and "assert remaining == 0" in rl_runtime_test
    )
    construction_coverage = (
        "CATALOGUE_MATERIALIZATION_EXHAUSTIVE_ELIGIBLE" in core_tests
        and "for (const auto& stats : data.supported())" in core_tests
        and "it->max_hp == stats.hp" in core_tests
        and "it->footprint_width == stats.footprint_width" in core_tests
    )
    if not construction_coverage:
        raise RuntimeError("missing exhaustive eligible Core catalogue-materialization regression")
    spell_materialization_coverage = (
        "SPELL_MATERIALIZATION_EXHAUSTIVE" in core_tests
        and "for (const auto& stats : data.supported_spells())" in core_tests
        and "effect.radius == stats.radius" in core_tests
        and "event.detail == \"lightning\"" in core_tests
        and "event.detail == \"earthquake\"" in core_tests
    )
    if not spell_materialization_coverage:
        raise RuntimeError("missing exhaustive Core spell-materialization regression")
    viewer_uses_core_scenario = "load_scenario(argv[++i], initial, error)" in viewer
    viewer_troop_ids = core_kinds if viewer_uses_core_scenario else set()
    # A core route is not sufficient for the visualizer: traps have their own
    # on-disk family.  Test the maintained route list, rather than assuming the
    # generic defense fallback can display them.
    viewer_trap_asset_kinds = {
        "bomb", "giant_bomb", "air_bomb", "seeking_air_mine", "spring_trap", "giga_bomb", "tornado_trap", "skeleton_trap"
    }
    viewer_trap_route = all(
        f"Kind::{''.join(part.title() for part in kind.split('_'))}" in viewer
        for kind in viewer_trap_asset_kinds
    ) and '"images/home/traps/" + id' in viewer

    official_max: dict[str, set[int]] = {}
    for item in reference["official_deltas"]:
        if "id" not in item:
            continue
        levels = item.get("levels")
        if levels and all(isinstance(level, int) for level in levels):
            official_max.setdefault(item["id"], set()).update(levels)
        elif isinstance(item.get("level"), int):
            official_max.setdefault(item["id"], set()).add(item["level"])

    rows: list[tuple[str, str, str, str, str, str, str, str]] = []
    failures: list[str] = []
    materialization_eligible_ids: set[str] = set()
    source_id = catalogue["source"]["commit"]
    reference_source_ids = {source["id"] for source in reference["sources"]}
    declared_without_test_reference = sorted(
        content_id for content_id in CORE_BEHAVIOUR_TESTED_IDS
        if f"Kind::{cpp_kind(content_id)}" not in test_sources
    )
    if declared_without_test_reference:
        raise RuntimeError(
            "behavioural coverage declaration lacks a core test reference: "
            + ", ".join(declared_without_test_reference)
        )
    snapshot_replay_sources = {
        "gold_mine": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "elixir_collector": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "air_defense": air_defense_tests,
        "bomb_tower": BOMB_TOWER_TESTS.read_text(encoding="utf-8"),
        "dark_elixir_drill": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "x_bow": X_BOW_TESTS.read_text(encoding="utf-8"),
        "inferno_tower": inferno_tower_tests,
        "bobs_hut": permanent_building_tests,
        "helper_hut": permanent_building_tests,
        "crafting_station": permanent_building_tests,
        "monolith": monolith_tests,
        "multi_archer_tower": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "multi_gear_tower": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "ricochet_cannon": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "scattershot": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "super_wizard_tower": PERMANENT_VARIANT_TESTS.read_text(encoding="utf-8"),
        "super_wall_breaker": super_wall_breaker_tests,
        "super_barbarian": super_barbarian_tests,
        "super_giant": super_giant_tests,
        "super_archer": super_archer_tests,
        "rocket_balloon": rocket_balloon_tests,
        "super_minion": super_minion_tests,
        "super_bowler": super_bowler_tests,
        "super_dragon": super_dragon_tests,
        "electro_dragon": electro_dragon_tests,
        "bowler": bowler_tests,
        "ice_golem": ice_golem_tests,
        "apprentice_warden": apprentice_warden_tests,
        "super_hog_rider": super_hog_rider_tests,
        "super_hog": super_hog_rider_tests,
        "super_rider": super_hog_rider_tests,
        "super_miner": super_miner_tests,
        "super_valkyrie": super_valkyrie_tests,
        "super_yeti": super_yeti_tests,
        "super_witch": super_witch_tests,
        "ice_hound": ice_hound_tests,
        "ice_pup": ice_hound_tests,
        "yeti": yeti_tests,
        "yetimite": yeti_tests,
        "witch": witch_tests,
        "lava_hound": lava_hound_tests,
        "lava_pup": lava_hound_tests,
        "headhunter": headhunter_tests,
        "druid": druid_tests,
        "druid_bear": druid_tests,
        "golem": golem_tests,
        "golemite": golem_tests,
        "wizard": wizard_tests,
        "giant": giant_tests,
        "archer": archer_tests,
        "barbarian": barbarian_tests,
        "healer": healer_tests,
        "wall_breaker": wall_breaker_tests,
        "electro_titan": electro_titan_tests,
        "thrower": thrower_tests,
        "root_rider": root_rider_tests,
        "sneaky_goblin": sneaky_goblin_tests,
    }
    declared_without_snapshot_replay_proof = sorted(
        content_id for content_id in ITEM_SNAPSHOT_REPLAY_TESTED_IDS
        if (f"Kind::{cpp_kind(content_id)}" not in snapshot_replay_sources.get(content_id, "")
            or ".snapshot()" not in snapshot_replay_sources.get(content_id, "")
            or ".restore(snapshot)" not in snapshot_replay_sources.get(content_id, "")
            or "save_replay(replay_path" not in snapshot_replay_sources.get(content_id, "")
            or "load_replay(replay_path" not in snapshot_replay_sources.get(content_id, ""))
    )
    if declared_without_snapshot_replay_proof:
        raise RuntimeError(
            "snapshot/replay coverage declaration lacks focused proof: "
            + ", ".join(declared_without_snapshot_replay_proof)
        )
    required_rl_runtime_loop = (
        "for stats in _cocsim.catalogue_stats():" in rl_runtime_test
        and "native.reset_scenario({" in rl_runtime_test
        and 'assert entity["kind"] == stats["kind"]' in rl_runtime_test
    )
    if not required_rl_runtime_loop:
        raise RuntimeError("missing exhaustive pybind scenario-materialization regression")
    declared_without_rl_runtime_proof = sorted(
        content_id for content_id in ITEM_RL_RUNTIME_TESTED_IDS
        if content_id not in core_kinds
    )
    if declared_without_rl_runtime_proof:
        raise RuntimeError(
            "RL runtime coverage declaration lacks a Core kind: "
            + ", ".join(declared_without_rl_runtime_proof)
        )
    declared_without_viewer_selection_proof = sorted(
        content_id for content_id in ITEM_VIEWER_SELECTION_TESTED_IDS
        if (f"Kind::{cpp_kind(content_id)}" not in viewer_selection_tests
            or "viewer::defender_at" not in viewer_selection_tests
            or "entity.id < *result" not in (ROOT / "src" / "viewer" / "selection.hpp").read_text(encoding="utf-8")
        )
    )
    if declared_without_viewer_selection_proof:
        raise RuntimeError(
            "Viewer selection coverage declaration lacks focused proof: "
            + ", ".join(declared_without_viewer_selection_proof)
        )
    fixture_defenders = {entry["kind"] for entry in permanent_building_headless_fixture["defenders"]}
    air_defense_headless_fixture = json.loads(
        (ROOT / "scenarios" / "air-defense-air-target.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in air_defense_headless_fixture["defenders"])
    monolith_headless_fixture = json.loads(
        (ROOT / "scenarios" / "monolith-max-hp.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in monolith_headless_fixture["defenders"])
    super_wall_breaker_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-wall-breaker-wall.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_wall_breaker_headless_fixture["army"])
    super_barbarian_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-barbarian-rage.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_barbarian_headless_fixture["army"])
    super_giant_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-giant-wall.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_giant_headless_fixture["army"])
    super_archer_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-archer-piercing.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_archer_headless_fixture["army"])
    rocket_balloon_headless_fixture = json.loads(
        (ROOT / "scenarios" / "rocket-balloon-boost.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in rocket_balloon_headless_fixture["army"])
    super_minion_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-minion-long-shot.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_minion_headless_fixture["army"])
    super_bowler_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-bowler-triple-strike.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_bowler_headless_fixture["army"])
    super_dragon_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-dragon-aggregate.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_dragon_headless_fixture["army"])
    electro_dragon_headless_fixture = json.loads(
        (ROOT / "scenarios" / "electro-dragon-chain.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in electro_dragon_headless_fixture["army"])
    bowler_headless_fixture = json.loads(
        (ROOT / "scenarios" / "bowler-double-strike.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in bowler_headless_fixture["army"])
    ice_golem_headless_fixture = json.loads(
        (ROOT / "scenarios" / "ice-golem-death-freeze.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in ice_golem_headless_fixture["army"])
    apprentice_warden_headless_fixture = json.loads(
        (ROOT / "scenarios" / "apprentice-warden-life-aura.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in apprentice_warden_headless_fixture["army"])
    super_hog_rider_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-hog-rider-split.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_hog_rider_headless_fixture["army"])
    super_miner_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-miner-ramp.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_miner_headless_fixture["army"])
    super_valkyrie_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-valkyrie-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_valkyrie_headless_fixture["army"])
    super_yeti_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-yeti-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_yeti_headless_fixture["army"])
    super_witch_headless_fixture = json.loads(
        (ROOT / "scenarios" / "super-witch-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in super_witch_headless_fixture["army"])
    ice_hound_headless_fixture = json.loads(
        (ROOT / "scenarios" / "ice-hound-ice-pup-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in ice_hound_headless_fixture["army"])
    yeti_headless_fixture = json.loads(
        (ROOT / "scenarios" / "yeti-yetimite-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in yeti_headless_fixture["army"])
    witch_headless_fixture = json.loads(
        (ROOT / "scenarios" / "witch-skeleton-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in witch_headless_fixture["army"])
    lava_hound_headless_fixture = json.loads(
        (ROOT / "scenarios" / "lava-hound-lava-pup-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in lava_hound_headless_fixture["army"])
    headhunter_headless_fixture = json.loads(
        (ROOT / "scenarios" / "headhunter-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in headhunter_headless_fixture["army"])
    druid_headless_fixture = json.loads(
        (ROOT / "scenarios" / "druid-bear-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in druid_headless_fixture["army"])
    golem_headless_fixture = json.loads(
        (ROOT / "scenarios" / "golem-golemite-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in golem_headless_fixture["defenders"])
    wizard_headless_fixture = json.loads(
        (ROOT / "scenarios" / "wizard-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in wizard_headless_fixture["defenders"])
    giant_headless_fixture = json.loads(
        (ROOT / "scenarios" / "giant-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in giant_headless_fixture["defenders"])
    archer_headless_fixture = json.loads(
        (ROOT / "scenarios" / "archer-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in archer_headless_fixture["defenders"])
    barbarian_headless_fixture = json.loads((ROOT / "scenarios" / "barbarian-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in barbarian_headless_fixture["defenders"])
    healer_headless_fixture = json.loads((ROOT / "scenarios" / "healer-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in healer_headless_fixture["army"])
    wall_breaker_headless_fixture = json.loads((ROOT / "scenarios" / "wall-breaker-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in wall_breaker_headless_fixture["army"])
    electro_titan_headless_fixture = json.loads((ROOT / "scenarios" / "electro-titan-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in electro_titan_headless_fixture["army"])
    thrower_headless_fixture = json.loads((ROOT / "scenarios" / "thrower-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in thrower_headless_fixture["army"])
    root_rider_headless_fixture = json.loads((ROOT / "scenarios" / "root-rider-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in root_rider_headless_fixture["army"])
    sneaky_goblin_headless_fixture = json.loads((ROOT / "scenarios" / "sneaky-goblin-baseline.json").read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in sneaky_goblin_headless_fixture["army"])
    bomb_tower_headless_fixture = json.loads(BOMB_TOWER_HEADLESS_FIXTURE.read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in bomb_tower_headless_fixture["defenders"])
    x_bow_headless_fixture = json.loads(X_BOW_HEADLESS_FIXTURE.read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in x_bow_headless_fixture["defenders"])
    inferno_tower_headless_fixture = json.loads(INFERNO_TOWER_HEADLESS_FIXTURE.read_text(encoding="utf-8"))
    fixture_defenders.update(entry["kind"] for entry in inferno_tower_headless_fixture["defenders"])
    cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    declared_without_headless_cli_proof = sorted(
        content_id for content_id in ITEM_HEADLESS_CLI_TESTED_IDS
        if content_id not in fixture_defenders
    )
    if (declared_without_headless_cli_proof
            or "permanent_building_headless_cli_validation" not in cmake
            or "permanent-passive-buildings.json" not in cmake
            or "air-defense-air-target.json" not in cmake
            or "air_defense_headless_cli_validation" not in cmake
            or "monolith-max-hp.json" not in cmake
            or "monolith_headless_cli_validation" not in cmake
            or "super-wall-breaker-wall.json" not in cmake
            or "super_wall_breaker_headless_cli_validation" not in cmake
            or "super-barbarian-rage.json" not in cmake
            or "super_barbarian_headless_cli_validation" not in cmake
            or "super-giant-wall.json" not in cmake
            or "super_giant_headless_cli_validation" not in cmake
            or "super-archer-piercing.json" not in cmake
            or "super_archer_headless_cli_validation" not in cmake
            or "rocket-balloon-boost.json" not in cmake
            or "rocket_balloon_headless_cli_validation" not in cmake
            or "super-minion-long-shot.json" not in cmake
            or "super_minion_headless_cli_validation" not in cmake
            or "super-bowler-triple-strike.json" not in cmake
            or "super_bowler_headless_cli_validation" not in cmake
            or "super-dragon-aggregate.json" not in cmake
            or "super_dragon_headless_cli_validation" not in cmake
            or "electro-dragon-chain.json" not in cmake
            or "electro_dragon_headless_cli_validation" not in cmake
            or "bowler-double-strike.json" not in cmake
            or "bowler_headless_cli_validation" not in cmake
            or "ice-golem-death-freeze.json" not in cmake
            or "ice_golem_headless_cli_validation" not in cmake
            or "apprentice-warden-life-aura.json" not in cmake
            or "apprentice_warden_headless_cli_validation" not in cmake
            or "super-hog-rider-split.json" not in cmake
            or "super_hog_rider_headless_cli_validation" not in cmake
            or "super-miner-ramp.json" not in cmake
            or "super_miner_headless_cli_validation" not in cmake
            or "super-valkyrie-baseline.json" not in cmake
            or "super_valkyrie_headless_cli_validation" not in cmake
            or "super-yeti-baseline.json" not in cmake
            or "super_yeti_headless_cli_validation" not in cmake
            or "super-witch-baseline.json" not in cmake
            or "super_witch_headless_cli_validation" not in cmake
            or "ice-hound-ice-pup-baseline.json" not in cmake
            or "ice_hound_headless_cli_validation" not in cmake
            or "yeti-yetimite-baseline.json" not in cmake
            or "yeti_headless_cli_validation" not in cmake
            or "witch-skeleton-baseline.json" not in cmake
            or "witch_headless_cli_validation" not in cmake
            or "lava-hound-lava-pup-baseline.json" not in cmake
            or "lava_hound_headless_cli_validation" not in cmake
            or "headhunter-baseline.json" not in cmake
            or "headhunter_headless_cli_validation" not in cmake
            or "druid-bear-baseline.json" not in cmake
            or "druid_headless_cli_validation" not in cmake
            or "golem-golemite-baseline.json" not in cmake
            or "golem_headless_cli_validation" not in cmake
            or "wizard-baseline.json" not in cmake
            or "wizard_headless_cli_validation" not in cmake
            or "giant-baseline.json" not in cmake
            or "giant_headless_cli_validation" not in cmake
            or "archer-baseline.json" not in cmake
            or "archer_headless_cli_validation" not in cmake
            or "barbarian-baseline.json" not in cmake
            or "barbarian_headless_cli_validation" not in cmake
            or "healer-baseline.json" not in cmake
            or "healer_headless_cli_validation" not in cmake
            or "wall-breaker-baseline.json" not in cmake
            or "wall_breaker_headless_cli_validation" not in cmake
            or "electro-titan-baseline.json" not in cmake
            or "electro_titan_headless_cli_validation" not in cmake
            or "thrower-baseline.json" not in cmake
            or "thrower_headless_cli_validation" not in cmake
            or "root-rider-baseline.json" not in cmake
            or "root_rider_headless_cli_validation" not in cmake
            or "sneaky-goblin-baseline.json" not in cmake
            or "sneaky_goblin_headless_cli_validation" not in cmake
            or "inferno-tower-baseline.json" not in cmake
            or "inferno_tower_headless_cli_validation" not in cmake):
        raise RuntimeError(
            "headless CLI coverage declaration lacks fixture proof: "
            + ", ".join(declared_without_headless_cli_proof)
        )
    for content in catalogue["contents"]:
        content_id = content["id"]
        levels = content.get("levels", [])
        # A supercharged record shares its numeric level with its normal record.
        # The explicit variant selector is preserved by Core, scenarios/replays,
        # Viewer and RL; it must never be silently reduced to normal data.
        ignored_variants = [
            level.get("variant", "normal") for level in levels
            if level.get("variant", "normal") != "normal"
        ]
        level_numbers = {level["level"] for level in levels}
        provenance = all(
            level.get("provenance", {}).get("source", {}).get("raw_sha256")
            and (level["provenance"]["source"].get("version") == source_id
                 or level["provenance"]["source"].get("source") in reference_source_ids)
            for level in levels
        )
        tiers = {
            level.get("provenance", {}).get("source", {}).get("tier")
            for level in levels
        }
        source_grade = "primaire" if tiers == {"primary"} else "secondaire" if tiers else "sans_niveau"
        data_gaps = missing_fields(content)
        data_state = "complet" if levels and not data_gaps else "incomplet" if levels else "non_applicable"
        required = official_max.get(content_id, set())
        official_level_ok = required <= level_numbers
        # A metadata-only entry (currently Hero Banner) has a Kind solely so
        # parsers can name it.  With no combat level GameData cannot construct
        # it, therefore a string enum must never be reported as a Core/GUI/RL
        # route in the coverage matrix.
        constructible = bool(levels)
        non_combat_supported = not constructible and 'data_.find_non_combat(p.kind)' in core
        non_combat_rl = non_combat_supported and 'non_combat_obstacles' in bindings
        # The renderer receives a Scenario value directly after the Viewer
        # extraction; keep this check tied to that shared scenario route rather
        # than the former monolithic main.cpp spelling.
        non_combat_gui = non_combat_supported and 'scenario.non_combat_obstacles' in viewer
        non_combat_source_asset = non_combat_gui and bool(content.get("image")) and (ROOT / "assets" / str(content["image"])).is_file()
        in_core = constructible and content_id in core_kinds and (not ignored_variants or variant_supported_by_core)
        in_rl = constructible and content_id in rl_kinds and (not ignored_variants or variant_supported_by_rl)
        if not constructible:
            gui = "non_combat_obstacle_source_asset" if non_combat_source_asset else "non_combat_obstacle_render_fallback" if non_combat_gui else "metadata_only_no_scenario_route"
        elif content_id not in viewer_troop_ids:
            gui = "render_only_or_absent"
        elif content["category"] == "trap" and content_id in viewer_trap_asset_kinds and not viewer_trap_route:
            gui = "asset_route_missing"
        elif content.get("support") == "spawned_only":
            # Golemite has a documented source alias to the Golem visual;
            # Skeleton Trap subtroops currently have no such sourced asset.
            has_source_visual = content_id == "golemite" or any(level.get("image") for level in levels)
            gui = "spawned_from_core" if has_source_visual else "spawned_from_core_no_source_asset"
        elif content["category"] == "troop":
            gui = "deployable_from_scenario"
        else:
            gui = "visible_from_scenario"
        if ignored_variants and not variant_supported_by_viewer:
            gui = "base_only"
        status = "NON_CONFORME"
        if not levels:
            status = "PRESENT_A_VERIFIER" if non_combat_supported and non_combat_rl and non_combat_gui else "METADATA_ONLY_OPEN"
        elif content["category"] == "troop" and in_core and in_rl and gui in {"deployable_from_scenario", "spawned_from_core"} and provenance and official_level_ok and not data_gaps:
            status = "PRESENT_A_VERIFIER"
        elif content["category"] != "troop" and in_core and in_rl and gui == "visible_from_scenario" and provenance and official_level_ok and not data_gaps:
            status = "PRESENT_A_VERIFIER"
        if ignored_variants and not (variant_supported_by_core and variant_supported_by_rl and variant_supported_by_viewer):
            variants = ", ".join(sorted(set(ignored_variants)))
            failures.append(f"{content_id}: {len(ignored_variants)} level record(s) of variant {variants} lack a complete core/GUI/RL selection route")
        elif constructible and not in_core:
            failures.append(f"{content_id}: catalogue loaded but no core Kind")
        if in_core and not in_rl:
            failures.append(f"{content_id}: core Kind absent from RL KINDS")
        if (in_core and content["category"] != "trap" and content_id != "hidden_tesla"
                and content.get("support") != "spawned_only"):
            materialization_eligible_ids.add(content_id)
        if in_core and levels and content_id not in CORE_BEHAVIOUR_TESTED_IDS:
            failures.append(f"{content_id}: core Kind has no declared behavioural regression")
        if gui == "asset_route_missing":
            failures.append(f"{content_id}: Viewer trap asset route is missing")
        if gui == "spawned_from_core_no_source_asset":
            failures.append(f"{content_id}: spawned by core but no source Viewer asset")
        if not provenance:
            failures.append(f"{content_id}: an imported level has no pinned raw provenance")
        if not official_level_ok:
            failures.append(f"{content_id}: missing official level(s) {sorted(required - level_numbers)}")
        if data_gaps:
            failures.append(f"{content_id}: source combat data incomplete ({'; '.join(data_gaps)})")
        core_state = "obstacle_non_ciblable" if non_combat_supported else "base_only" if ignored_variants and not variant_supported_by_core else "oui" if in_core else "non"
        adapter_state = "obstacle_non_ciblable" if non_combat_rl else "base_only" if ignored_variants and not variant_supported_by_rl else "oui" if in_rl else "non"
        rows.append((content_id, content["category"], ",".join(map(str, sorted(level_numbers))) or "—", source_grade + "/" + data_state, core_state, gui, adapter_state, status))

    print("# Audit de conformité TH18 — données chargées")
    print()
    print(f"Généré depuis le registre `{reference['reference_id']}`. La colonne "
          "Données vérifie les champs source pertinents à chaque niveau ; `PRESENT_A_VERIFIER` "
          "signifie que le chemin de données existe, pas une fidélité de combat validée.")
    print()
    print("| Élément | Catégorie | Niveaux importés | Données | Core | GUI | RL | État |")
    print("|---|---|---|---|---:|---|---:|---|")
    for row in rows:
        print("| " + " | ".join(row) + " |")
    print()
    print("## Écarts structurels détectés")
    for failure in failures:
        print(f"- {failure}")
    print(f"\nRésultat : {len(rows)} entrées de catalogue, {len(failures)} écart(s) structurel(s).")

    # The persistent goal has a narrower boundary than the historical whole
    # Home Village audit. Keep a separate row for every in-scope permanent
    # entity, and deliberately report only evidence that exists today. Shared
    # Core plumbing is useful, but it is not an item-specific serialization or
    # fidelity proof.
    permanent_ids = set().union(*(set(group) for group in permanent_scope["included_groups"].values()))
    content_by_id = {content["id"]: content for content in catalogue["contents"]}
    # The dated scope is an inventory snapshot, not a claim that a required
    # child must remain absent forever.  A later active delta may supply a
    # source-backed spawned-only row (for example Lava Pup); retain only the
    # still-missing children in the explicit absent-child table.
    unmaterializable_children = {
        content_id: entry
        for content_id, entry in permanent_scope["unmaterializable_required_spawned_subunits"].items()
        if content_id not in content_by_id
    }
    delta_by_id: dict[str, list[dict[str, object]]] = collections.defaultdict(list)
    for entry in json.loads((ROOT / "data" / "reference" / "th18-documentation-delta-2026-09-17.json").read_text(encoding="utf-8"))["entries"]:
        for content_id in entry.get("scope", {}).get("ids", []):
            delta_by_id[content_id].append(entry)

    def source_asset_state(content: dict[str, object]) -> str:
        paths = [level.get("image") for level in content.get("levels", [])]
        # A partially declared level set is not an asset-complete entity.  In
        # particular, a post-freeze level may have exact stats before a public
        # sprite URL is available; existing lower-level files cannot conceal
        # that missing level-specific rendering evidence.
        if paths and any(not isinstance(path, str) or not path for path in paths):
            return "non_documente"
        if not paths:
            paths = [content.get("image")]
        declared = [path for path in paths if isinstance(path, str) and path]
        if not declared:
            return "non_documente"
        return "oui" if all((ROOT / "assets" / path).is_file() for path in declared) else "manquant"

    print("\n## Matrice de conformité — périmètre permanent TH18")
    print("`oui` exige une preuve propre à l'élément; `partage` désigne une voie "
          "commune sans preuve spécifique; `non_verifie` ne doit jamais être lu comme achevé.")
    print("| Élément | Inventaire | Données/niveaux | Core matérialisable | Comportement | Headless | GUI | RL | Assets source | Snapshot/replay | Tests | Fidélité | Incertitudes |")
    print("|---|---:|---|---|---|---|---|---|---|---|---|---|---|")
    for content_id in sorted(permanent_ids):
        content = content_by_id[content_id]
        levels = content.get("levels", [])
        placement_only = content_id == "hero_banner"
        data = "placement_source" if placement_only else "oui" if levels else "non"
        if placement_only:
            core_state = "oui" if 'data_.find_non_combat(p.kind)' in core else "non"
            rl_state = "oui" if 'non_combat_obstacles' in bindings else "non"
            gui_state = "oui" if 'scenario.non_combat_obstacles' in viewer else "non"
        else:
            core_state = "oui" if content_id in core_kinds else "non"
            if content_id not in rl_kinds:
                rl_state = "non"
            elif content_id in ITEM_RL_RUNTIME_TESTED_IDS:
                rl_state = "oui"
            else:
                rl_state = "partage"
            if content_id not in viewer_troop_ids:
                gui_state = "non"
            elif content_id in ITEM_VIEWER_SELECTION_TESTED_IDS:
                gui_state = "oui"
            else:
                gui_state = "partage"
        behavior = "oui" if content_id in CORE_BEHAVIOUR_TESTED_IDS else "non"
        headless = "oui" if content_id in ITEM_HEADLESS_CLI_TESTED_IDS else "partage" if core_state == "oui" else "non"
        tests = "comportement" if behavior == "oui" else "structure"
        if content_id in ITEM_RL_RUNTIME_TESTED_IDS:
            tests += "+rl_runtime"
        if content_id in ITEM_VIEWER_SELECTION_TESTED_IDS:
            tests += "+viewer_selection"
        if content_id in ITEM_HEADLESS_CLI_TESTED_IDS:
            tests += "+headless_cli"
        snapshot_replay = "oui" if content_id in ITEM_SNAPSHOT_REPLAY_TESTED_IDS else "non_verifie"
        uncertainty = "ouverte" if delta_by_id.get(content_id) else "non_inventoriee"
        print("| " + " | ".join((
            content_id, "oui", data, core_state, behavior, headless, gui_state, rl_state,
            source_asset_state(content), snapshot_replay, tests,
            "non_verifie", uncertainty,
        )) + " |")
    for content_id, entry in sorted(unmaterializable_children.items()):
        print("| " + " | ".join((
            content_id, "oui", "non_catalogue", "non", "non", "non", "non", "non",
            "non_documente", "non", "structure+audit", "non_verifie", "ouverte",
        )) + " |")
    print(f"\nRésultat périmètre permanent : {len(permanent_ids)} éléments matérialisables et "
          f"{len(unmaterializable_children)} sous-unité(s) requise(s) sans catalogue; aucune ligne n'est promue "
          "en fidélité vérifiée sans preuve versionnée de comportement, d'assets et de sérialisation.")

    # The prior table deliberately audits only *loaded* entries.  Keep that
    # useful distinction, but also expose officially documented inventory
    # members that have not reached either catalogue namespace yet.  Otherwise
    # a missing TH18 content item would be invisible to a catalogue-only pass.
    official_content_families = {
        "building", "crafted_defense", "defense", "equipment", "guardian",
        "merged_defense", "pet", "siege", "spell", "spawned_unit", "trap",
        "troop", "wall",
    }
    loaded_ids = {content["id"] for content in catalogue["contents"]}
    loaded_ids.update(spell["id"] for spell in catalogue.get("spells", []))
    expected: dict[tuple[str, str], dict[str, object]] = {}
    for delta in reference["official_deltas"]:
        family = delta.get("family")
        content_id = delta.get("id")
        if family in official_content_families and isinstance(content_id, str):
            expected.setdefault((family, content_id), delta)
    missing_expected = [
        (family, content_id, delta)
        for (family, content_id), delta in sorted(expected.items())
        if content_id not in loaded_ids
    ]
    print("\n## Inventaire officiel gelé absent du catalogue")
    if not missing_expected:
        print("- aucun")
    else:
        for family, content_id, delta in missing_expected:
            level = delta.get("level", delta.get("levels", "—"))
            print(f"- {content_id} ({family}, niveau(x) {level}, source {delta['source']})")
    print(f"\nRésultat inventaire : {len(expected)} éléments explicitement documentés par les deltas officiels, "
          f"{len(missing_expected)} absent(s) du catalogue.")

    # Official notes are deltas, not a roster. Keep the dated community roster
    # separate from those stronger sources: it makes omissions visible without
    # promoting its entries to primary numeric or behavioural evidence.
    indexed: dict[tuple[str, str], str] = {}
    for index in reference.get("secondary_inventory_indices", []):
        source = index["source"]
        for content_id in index["ids"]:
            indexed.setdefault((index["family"], content_id), source)
    missing_indexed = [
        (family, content_id, source)
        for (family, content_id), source in sorted(indexed.items())
        if content_id not in loaded_ids
    ]
    print("\n## Inventaire secondaire daté absent du catalogue")
    if not missing_indexed:
        print("- aucun")
    else:
        for family, content_id, source in missing_indexed:
            print(f"- {content_id} ({family}, source secondaire {source})")
    print(f"\nRésultat index secondaire : {len(indexed)} éléments de roster, "
          f"{len(missing_indexed)} absent(s) du catalogue. Ces lignes ne constituent pas une preuve de statistiques ni de comportement.")

    # One matrix is the authoritative coverage view: both imported data and
    # every source-indexed omission have a row.  Tests/fidelity deliberately
    # remain conservative: this audit proves plumbing, not game fidelity.
    loaded_rows = {row[0]: row for row in rows}
    spell_ids = {spell["id"] for spell in catalogue.get("spells", [])}
    matrix: dict[str, dict[str, set[str]]] = {}
    for (family, content_id), delta in expected.items():
        item = matrix.setdefault(content_id, {"families": set(), "sources": set()})
        item["families"].add(family)
        item["sources"].add(delta["source"])
    for (family, content_id), source in indexed.items():
        item = matrix.setdefault(content_id, {"families": set(), "sources": set()})
        item["families"].add(family)
        item["sources"].add(source)
    # Supplemental entries are added by dated active deltas, so they can be
    # required spawned children absent from the frozen inventory snapshot.
    for supplemental in reference.get("catalogue_supplemental_contents", []):
        content_id = supplemental["content_id"]
        content = content_by_id.get(content_id, {})
        family = "spawned_unit" if content.get("support") == "spawned_only" else "supplemental"
        item = matrix.setdefault(content_id, {"families": set(), "sources": set()})
        item["families"].add(family)
        item["sources"].add(supplemental["source"])
    print("\n## Matrice de couverture — inventaire attendu")
    print("| Élément | Famille | Sources | Catalogue | Core | GUI | RL | Tests | Fidélité |")
    print("|---|---|---|---:|---|---|---|---|---|")
    coverage = collections.Counter()
    for content_id, item in sorted(matrix.items()):
        if content_id in loaded_rows:
            row = loaded_rows[content_id]
            catalogue_state, core_state, gui_state, rl_state = "oui", row[4], row[5], row[6]
            if row[4] == "obstacle_non_ciblable":
                tests = "core+GUI_source_asset+RL:non_combat_obstacle" if gui_state == "non_combat_obstacle_source_asset" else "core+GUI_fallback+RL:non_combat_obstacle"
                fidelity = "a_verifier"
            elif core_state == "oui" and rl_state == "oui":
                evidence = ["rl_runtime:catalogue"]
                if content_id in materialization_eligible_ids:
                    evidence.insert(0, "core_materialization_eligible_exhaustive")
                if content_id in CORE_BEHAVIOUR_TESTED_IDS:
                    evidence.append("core_behavior")
                tests = "+".join(evidence)
                fidelity = "a_verifier"
            else:
                tests = "audit_structure"
                fidelity = "a_verifier"
        elif content_id in spell_ids:
            catalogue_state = "oui"
            core_state = "oui" if content_id in core_spells else "non"
            gui_state = "interface_sort"
            rl_state = "oui" if content_id in rl_spells else "non"
            tests = ("core_spell_materialization_exhaustive+rl_runtime:cast_reserve"
                     if runtime_spell_cast_coverage and spell_materialization_coverage and content_id in rl_spells
                     else "audit_structure")
            fidelity = "a_verifier"
        else:
            catalogue_state, core_state, gui_state, rl_state = "non", "non", "non", "non"
            tests, fidelity = "non", "manquant"
        coverage["catalogue"] += catalogue_state == "oui"
        coverage["core"] += core_state in {"oui", "obstacle_non_ciblable"}
        coverage["gui"] += gui_state not in {"non", "render_only_or_absent", "metadata_only_no_scenario_route", "non_combat_obstacle_render_fallback", "spawned_from_core_no_source_asset"}
        coverage["rl"] += rl_state in {"oui", "obstacle_non_ciblable"}
        coverage["core_behavior"] += "core_behavior" in tests
        coverage["core_construction"] += "core_materialization_eligible_exhaustive" in tests
        coverage["spell_materialization"] += "core_spell_materialization_exhaustive" in tests
        coverage["fidelity_verified"] += fidelity == "verifie"
        print("| " + " | ".join((content_id, ",".join(sorted(item["families"])), ",".join(sorted(item["sources"])), catalogue_state, core_state, gui_state, rl_state, tests, fidelity)) + " |")
    print()
    print("### Synthèse de couverture")
    print(f"- inventaire attendu : {len(matrix)} éléments")
    print(f"- catalogue : {coverage['catalogue']}/{len(matrix)} ; core : {coverage['core']}/{len(matrix)} ; "
          f"GUI sans écart visuel connu : {coverage['gui']}/{len(matrix)} ; RL : {coverage['rl']}/{len(matrix)}")
    print(f"- régression comportementale core exécutée : {coverage['core_behavior']}/{len(matrix)} ; "
          f"fidélité vérifiée : {coverage['fidelity_verified']}/{len(matrix)}")
    print(f"- matérialisation Core exhaustive des enregistrements éligibles (PV/portée/emprise/métadonnées) : "
          f"{coverage['core_construction']}/{len(matrix)} ; pièges/Tesla cachée/invocations ont leurs régressions dédiées, "
          "et cette preuve ne valide pas les mécaniques spécifiques")
    print(f"- matérialisation exhaustive des sorts Core (rayon/durée ou effet instantané) : "
          f"{coverage['spell_materialization']}/{len(matrix)} ; cette preuve ne valide pas les interactions fines")


if __name__ == "__main__":
    main()
