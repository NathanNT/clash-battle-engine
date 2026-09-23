"""Build and verify the per-item Home Village Hero goal coverage baseline."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
REFERENCE = ROOT / "data/reference/th18-2026-09-17.json"
SCOPE = ROOT / "data/reference/th18-2026-09-23-hero-pet-equipment-scope.json"
LASSI_AUDIT = ROOT / "data/reference/th18-2026-09-23-lassi-combat-contract-audit.json"
ELECTRO_OWL_AUDIT = ROOT / "data/reference/th18-2026-09-23-electro-owl-level-and-behavior-audit.json"
MIGHTY_YAK_AUDIT = ROOT / "data/reference/th18-2026-09-23-mighty-yak-level-and-wall-contract-audit.json"
UNICORN_AUDIT = ROOT / "data/reference/th18-2026-09-23-unicorn-healing-and-lifecycle-audit.json"
FROSTY_AUDIT = ROOT / "data/reference/th18-2026-09-23-frosty-frostmite-contract-audit.json"
DIGGY_AUDIT = ROOT / "data/reference/th18-2026-09-23-diggy-current-level-and-behavior-audit.json"
POISON_LIZARD_AUDIT = ROOT / "data/reference/th18-2026-09-23-poison-lizard-level-and-effect-audit.json"
PHOENIX_AUDIT = ROOT / "data/reference/th18-2026-09-23-phoenix-revival-and-interaction-audit.json"
SPIRIT_FOX_AUDIT = ROOT / "data/reference/th18-2026-09-23-spirit-fox-invisibility-contract-audit.json"
ANGRY_JELLY_AUDIT = ROOT / "data/reference/th18-2026-09-23-angry-jelly-brainwash-contract-audit.json"
SNEEZY_AUDIT = ROOT / "data/reference/th18-2026-09-23-sneezy-booger-contract-audit.json"
GREEDY_RAVEN_AUDIT = ROOT / "data/reference/th18-2026-09-23-greedy-raven-resource-target-audit.json"
REVENGE_DECK_AUDIT = ROOT / "data/reference/th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json"
BARBARIAN_PUPPET_AUDIT = ROOT / "data/reference/th18-2026-09-23-barbarian-puppet-summon-contract-audit.json"
BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN = ROOT / "data/reference/th18-2026-09-23-barbarian-puppet-official-bonus-and-spawn-rate.json"
RAGE_VIAL_AUDIT = ROOT / "data/reference/th18-2026-09-23-rage-vial-effect-order-audit.json"
EARTHQUAKE_BOOTS_AUDIT = ROOT / "data/reference/th18-2026-09-23-earthquake-boots-ground-target-audit.json"
EARTHQUAKE_BOOTS_OFFICIAL_RADIUS_DAMAGE = ROOT / "data/reference/th18-2026-09-23-earthquake-boots-official-radius-and-damage-table.json"
VAMPSTACHE_AUDIT = ROOT / "data/reference/th18-2026-09-23-vampstache-heal-event-audit.json"
VAMPSTACHE_OFFICIAL_DPS = ROOT / "data/reference/th18-2026-09-23-vampstache-official-dps-table.json"
GIANT_GAUNTLET_AUDIT = ROOT / "data/reference/th18-2026-09-23-giant-gauntlet-current-effect-audit.json"
SPIKY_BALL_AUDIT = ROOT / "data/reference/th18-2026-09-23-spiky-ball-bounce-and-count-audit.json"
SNAKE_BRACELET_AUDIT = ROOT / "data/reference/th18-2026-09-23-snake-bracelet-spawn-contract-audit.json"
STICK_HORSE_AUDIT = ROOT / "data/reference/th18-2026-09-23-stick-horse-deployment-and-wall-audit.json"
ARCHER_PUPPET_AUDIT = ROOT / "data/reference/th18-2026-09-23-archer-puppet-summon-contract-audit.json"
ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY = ROOT / "data/reference/th18-2026-09-23-archer-puppet-official-bonus-and-invisibility-table.json"
INVISIBILITY_VIAL_AUDIT = ROOT / "data/reference/th18-2026-09-23-invisibility-vial-damage-and-target-audit.json"
GIANT_ARROW_AUDIT = ROOT / "data/reference/th18-2026-09-23-giant-arrow-air-defense-damage-audit.json"
HEALER_PUPPET_AUDIT = ROOT / "data/reference/th18-2026-09-23-healer-puppet-summon-and-healing-audit.json"
FROZEN_ARROW_AUDIT = ROOT / "data/reference/th18-2026-09-23-frozen-arrow-slow-and-impact-audit.json"
MAGIC_MIRROR_AUDIT = ROOT / "data/reference/th18-2026-09-23-magic-mirror-clone-and-invisibility-audit.json"
ACTION_FIGURE_AUDIT = ROOT / "data/reference/th18-2026-09-23-action-figure-giant-giant-contract-audit.json"
MONOLITH_ARROW_AUDIT = ROOT / "data/reference/th18-2026-09-23-monolith-arrow-housing-and-projectile-audit.json"
MONOLITH_ARROW_PASSIVE = ROOT / "data/reference/th18-2026-09-23-monolith-arrow-passive-type-override.json"
HENCHMEN_PUPPET_AUDIT = ROOT / "data/reference/th18-2026-09-23-henchmen-puppet-and-child-contract-audit.json"
DARK_ORB_AUDIT = ROOT / "data/reference/th18-2026-09-23-dark-orb-projectile-and-slow-audit.json"
METAL_PANTS_AUDIT = ROOT / "data/reference/th18-2026-09-23-metal-pants-barrier-and-recovery-audit.json"
NOBLE_IRON_AUDIT = ROOT / "data/reference/th18-2026-09-23-noble-iron-opening-shots-audit.json"
DARK_CROWN_AUDIT = ROOT / "data/reference/th18-2026-09-23-dark-crown-defeat-threshold-audit.json"
METEOR_STAFF_AUDIT = ROOT / "data/reference/th18-2026-09-23-meteor-staff-target-and-cadence-audit.json"
ETERNAL_TOME_AUDIT = ROOT / "data/reference/th18-2026-09-23-eternal-tome-aura-immunity-audit.json"
LIFE_GEM_AUDIT = ROOT / "data/reference/th18-2026-09-23-life-gem-balance-and-aura-audit.json"
RAGE_GEM_AUDIT = ROOT / "data/reference/th18-2026-09-23-rage-gem-aura-and-stacking-audit.json"
HEALING_TOME_AUDIT = ROOT / "data/reference/th18-2026-09-23-healing-tome-aura-and-pulse-audit.json"
HEALING_TOME_OFFICIAL_DURATION = ROOT / "data/reference/th18-2026-09-23-healing-tome-official-duration-table.json"
HEALING_TOME_PRIMARY_LEVEL_PROVENANCE = ROOT / "data/reference/th18-2026-09-23-healing-tome-primary-level-provenance.json"
SIX_EQUIPMENT_PRIMARY_LEVEL_PROVENANCE = ROOT / "data/reference/th18-2026-09-23-six-equipment-primary-level-provenance.json"
FIREBALL_AUDIT = ROOT / "data/reference/th18-2026-09-23-fireball-target-size-and-splash-audit.json"
LAVALOON_PUPPET_AUDIT = ROOT / "data/reference/th18-2026-09-23-lavaloon-puppet-and-child-contract-audit.json"
HEROIC_TORCH_AUDIT = ROOT / "data/reference/th18-2026-09-23-heroic-torch-balance-and-wall-audit.json"
ROYAL_GEM_AUDIT = ROOT / "data/reference/th18-2026-09-23-royal-gem-recovery-and-bonus-audit.json"
ROYAL_GEM_OFFICIAL_BONUS = ROOT / "data/reference/th18-2026-09-23-royal-gem-official-level-bonus-table.json"
ROYAL_GEM_ACTIVE_HEAL_RECHECK = ROOT / "data/reference/th18-2026-09-23-royal-gem-active-heal-source-recheck.json"
SEEKING_SHIELD_AUDIT = ROOT / "data/reference/th18-2026-09-23-seeking-shield-target-and-bounce-audit.json"
HOG_RIDER_PUPPET_AUDIT = ROOT / "data/reference/th18-2026-09-23-hog-rider-puppet-and-child-audit.json"
HASTE_VIAL_AUDIT = ROOT / "data/reference/th18-2026-09-23-haste-vial-cadence-and-speed-audit.json"
ROCKET_SPEAR_AUDIT = ROOT / "data/reference/th18-2026-09-23-rocket-spear-charged-shots-audit.json"
ELECTRO_BOOTS_AUDIT = ROOT / "data/reference/th18-2026-09-23-electro-boots-aura-and-balance-audit.json"
ELECTRO_BOOTS_OFFICIAL_AURA_HEALING = ROOT / "data/reference/th18-2026-09-23-electro-boots-official-aura-and-healing-table.json"
FROST_FLAKE_AUDIT = ROOT / "data/reference/th18-2026-09-23-frost-flake-freeze-and-target-audit.json"
FIRE_HEART_AUDIT = ROOT / "data/reference/th18-2026-09-23-fire-heart-balance-and-death-burst-audit.json"
FLAME_BLOWER_AUDIT = ROOT / "data/reference/th18-2026-09-23-flame-blower-direction-and-activation-audit.json"
STUN_BLASTER_AUDIT = ROOT / "data/reference/th18-2026-09-23-stun-blaster-shockwave-and-target-audit.json"
ELECTRO_FANGS_AUDIT = ROOT / "data/reference/th18-2026-09-23-electro-fangs-chain-and-target-audit.json"
ROCKET_BACKPACK_AUDIT = ROOT / "data/reference/th18-2026-09-23-rocket-backpack-dash-and-balance-audit.json"
SHARED_ATTACHMENT_AUDIT = ROOT / "data/reference/th18-2026-09-23-hero-pet-equipment-attachment-contract-audit.json"
ELECTRO_FANGS_PASSIVE_AUDIT = ROOT / "data/reference/th18-2026-09-23-electro-fangs-passive-type-override.json"
HERO_BATTLEFIELD_ART_RIGHTS_AUDIT = ROOT / "data/reference/th18-2026-09-23-hero-battlefield-art-rights-audit.json"
HERO_BANNER_ASSIGNMENT_AUDIT = ROOT / "data/reference/th18-2026-09-20-hero-banner-assignment-official.json"
GRAND_WARDEN_AIR_MODE = ROOT / "data/reference/th18-2026-09-20-grand-warden-air-mode.json"
CATALOGUE = ROOT / "data/catalogue.normalized.json"
PRIOR_HERO_COVERAGE = ROOT / "data/reference/th18-hero-coverage-2026-09-20.json"
OUTPUT = ROOT / "data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json"
HERO_IDS = ("barbarian_king", "archer_queen", "grand_warden", "royal_champion", "minion_prince", "dragon_duke")


def level_span(levels: list[int]) -> str:
    return f"{min(levels)}-{max(levels)}" if levels else "none"


def build() -> dict:
    frozen = json.loads(REFERENCE.read_text(encoding="utf-8"))
    catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    prior = json.loads(PRIOR_HERO_COVERAGE.read_text(encoding="utf-8"))
    indexed = {entry["family"]: entry["ids"] for entry in frozen["secondary_inventory_indices"]
               if entry.get("family") in ("pet", "equipment")}
    revenge_audit = json.loads(REVENGE_DECK_AUDIT.read_text(encoding="utf-8"))
    assert revenge_audit["secondary_inventory_additions"] == [
        {"family": "equipment", "ids": ["revenge_deck"],
         "source": "supercell-2026-08-12-revenge-deck-release"}]
    indexed["equipment"] = indexed["equipment"] + ["revenge_deck"]
    by_id = {content["id"]: content for content in catalogue["contents"]}
    assert len(indexed["pet"]) == 12 and len(indexed["equipment"]) == 42
    assert len(set(indexed["pet"])) == 12 and len(set(indexed["equipment"])) == 42
    assert set(HERO_IDS) == {item["content_id"] for item in prior["items"]}

    items = []
    for item in prior["items"]:
        items.append({"content_id": item["content_id"], "family": "hero",
                      "catalogue_levels": level_span([level["level"] for level in by_id[item["content_id"]]["levels"]]),
                      "th18_levels": item["th18_levels"], "core": item["core_materializable"],
                      "headless": item["headless"], "viewer": item["viewer"], "rl": item["rl"],
                      "snapshot": item["snapshot"], "replay": item["replay"],
                      "fidelity": "partial", "uncertainties": item["uncertainties"],
                      "battlefield_art_rights": {
                          "status": "partial_no_compatible_redistribution_permission_verified",
                          "audit": {"file": HERO_BATTLEFIELD_ART_RIGHTS_AUDIT.name,
                                    "sha256": hashlib.sha256(HERO_BATTLEFIELD_ART_RIGHTS_AUDIT.read_bytes()).hexdigest()},
                      },
                      "defending_banner_assignment": {
                          "status": "viewer_mouse_all_six_variants_core_snapshot_replay_playback_verified; defensive_hero_combat_partial",
                          "viewer_art": "all_six_local_banner_icons_decode_and_render_distinct_pixels; battlefield_hero_sprite_rights_partial",
                          "rl_variant": "all_six_value_variants_preserved_and_observed; visual_only_variants_equal_combat_hash; rl_runtime_verified",
                          "audit": {"file": HERO_BANNER_ASSIGNMENT_AUDIT.name,
                                    "sha256": hashlib.sha256(HERO_BANNER_ASSIGNMENT_AUDIT.read_bytes()).hexdigest()},
                      }})
        if item["content_id"] == "grand_warden":
            items[-1]["viewer_mode"] = {
                "status": "ground_and_air_prebattle_mouse_selection_core_validation_and_fixed_tick_viewer_replay_verified",
                "source": {"file": GRAND_WARDEN_AIR_MODE.name,
                           "sha256": hashlib.sha256(GRAND_WARDEN_AIR_MODE.read_bytes()).hexdigest()},
            }
            items[-1]["rl_mode"] = {
                "status": "ground_and_air_scenario_modes_preserved; core_flying_state_and_distinct_hash_verified",
                "source": {"file": GRAND_WARDEN_AIR_MODE.name,
                           "sha256": hashlib.sha256(GRAND_WARDEN_AIR_MODE.read_bytes()).hexdigest()},
            }
    for family, category in (("pet", "pet"), ("equipment", "hero-equipment")):
        for content_id in indexed[family]:
            content = by_id[content_id]
            assert content["category"] == category and content["support"] == "catalogued_only"
            levels = [level["level"] for level in content["levels"]]
            first_source = content["levels"][0]["provenance"]["source"]
            record = {"content_id": content_id, "family": family,
                      "catalogue_levels": level_span(levels), "th18_levels": "requires_item_audit",
                      "source": {"url": first_source["url"], "version": first_source["version"],
                                 "raw_sha256": first_source["raw_sha256"], "tier": first_source["tier"]},
                      "core": "not_materialized", "headless": "not_exposed",
                      "viewer": "not_exposed", "rl": "not_exposed",
                      "catalogue_access": "core_headless_viewer_rl_read_only",
                      "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                      "uncertainties": ["current values and full deterministic effect contract",
                                        "Hero attachment and interaction ordering", "visual and asset rights"]}
            if family == "equipment":
                # Frozen supplemental Equipment predates the opaque raw schema
                # and stores its sourced Hero directly on the content record.
                record["eligible_hero"] = content.get("source_fields", {}).get(
                    "hero", content.get("hero", "unverified")).replace("-", "_")
                record["ability_type"] = (content.get("equipment_activation_type") or
                                          content.get("source_fields", {}).get("abilityType", "unverified"))
            if content_id == "barbarian_puppet":
                record["th18_levels"] = "1-18 catalogued; official active summon, 20-second Rage, 500-ms batch rate and per-level HP/recovery"
                record["evidence_audit"] = {"file": BARBARIAN_PUPPET_AUDIT.name,
                                            "sha256": hashlib.sha256(BARBARIAN_PUPPET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["official_bonus_spawn_table"] = {"file": BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN.name,
                                                         "sha256": hashlib.sha256(BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN.read_bytes()).hexdigest(),
                                                         "status": "all_18_hp_recovery_pairs_and_historical_rate_primary_confirmed_spawn_combat_partial"}
                record["required_spawned_unit_id"] = "barbarian_puppet_barbarian"
                record["uncertainties"] = ["current total summon count and partial final batch",
                                           "Equipment selection and activation command",
                                           "spawned Barbarian level and ordinary troop stat relation",
                                           "first batch timing, spawn placement, collision and entity-ID order",
                                           "Rage boundary and modifier stacking",
                                           "Hero HP increase and recovery timing",
                                           "King death, Phoenix and other Equipment interactions",
                                           "spawned-unit targeting, traps and art rights"]
            if content_id == "rage_vial":
                record["th18_levels"] = "1-18 catalogued; official active King Rage and dated 2026 DPS/recovery table"
                record["evidence_audit"] = {"file": RAGE_VIAL_AUDIT.name,
                                            "sha256": hashlib.sha256(RAGE_VIAL_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current-build duration and Rage scalar continuity",
                                           "passive DPS versus active damage stage and rounding",
                                           "movement-speed units and path-step rounding",
                                           "activation and buff expiry tick order",
                                           "HP recovery phase and max-HP cap",
                                           "two-Equipment, Pet, Rage Spell and aura stacking",
                                           "Viewer, replay, snapshot and RL activation contract"]
            if content_id == "earthquake_boots":
                record["th18_levels"] = "1-18 catalogued; official eight-tile radius and L9-18 post-2025 building damage; 2026 ground-target change"
                record["evidence_audit"] = {"file": EARTHQUAKE_BOOTS_AUDIT.name,
                                            "sha256": hashlib.sha256(EARTHQUAKE_BOOTS_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["official_radius_damage_table"] = {"file": EARTHQUAKE_BOOTS_OFFICIAL_RADIUS_DAMAGE.name,
                                                          "sha256": hashlib.sha256(EARTHQUAKE_BOOTS_OFFICIAL_RADIUS_DAMAGE.read_bytes()).hexdigest(),
                                                          "status": "radius_unit_and_L9_to_L18_building_percent_primary_confirmed_combat_application_partial"}
                record["uncertainties"] = ["current L1-8 building and all-level Wall/ground-troop damage and Wall formula",
                                           "radius center, hitbox and edge inclusion",
                                           "Skeleton Trap exception after 2026 ground-target change",
                                           "ground Hero, Pet and spawned-unit eligibility",
                                           "area damage and King attack tick order",
                                           "Hero HP/DPS bonus stacking",
                                           "Viewer art, overlays, replay and RL command contract"]
            if content_id == "vampstache":
                record["th18_levels"] = "1-18 catalogued; official passive King healing and primary per-level DPS table"
                record["evidence_audit"] = {"file": VAMPSTACHE_AUDIT.name,
                                            "sha256": hashlib.sha256(VAMPSTACHE_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["official_dps_table"] = {"file": VAMPSTACHE_OFFICIAL_DPS.name,
                                                "sha256": hashlib.sha256(VAMPSTACHE_OFFICIAL_DPS.read_bytes()).hexdigest(),
                                                "status": "all_18_dps_values_primary_confirmed_heal_and_speed_application_partial"}
                record["uncertainties"] = ["current heal-per-hit and attack-speed values",
                                           "attack launch, hit or damage-impact trigger",
                                           "Wall, immune, miss and multi-target eligibility",
                                           "heal versus damage and death tick order",
                                           "max-HP cap and rounding",
                                           "cadence conversion and other modifier stacking",
                                           "Pet, Phoenix and King defeat interactions",
                                           "Viewer, snapshot, replay and RL loadout contract"]
            if content_id == "giant_gauntlet":
                record["th18_levels"] = "1-27 catalogued; official 2.5-tile area and 2026 all-level duration/reduction buff"
                record["evidence_audit"] = {"file": GIANT_GAUNTLET_AUDIT.name,
                                            "sha256": hashlib.sha256(GIANT_GAUNTLET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current post-January-2026 duration and reduction by level",
                                           "current regeneration and DPS continuity",
                                           "King size, area hitbox and target eligibility",
                                           "damage reduction with traps, Pets and other modifiers",
                                           "regeneration and death tick order",
                                           "activation and expiry tick order",
                                           "Viewer transformation art, replay and RL commands"]
            if content_id == "spiky_ball":
                record["th18_levels"] = "1-27 catalogued; official Building-only bounce and launch/current-count discrepancy"
                record["evidence_audit"] = {"file": SPIKY_BALL_AUDIT.name,
                                            "sha256": hashlib.sha256(SPIKY_BALL_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current target counts and projectile damage at every level",
                                           "size-aware first-target geometry and tie-break",
                                           "later bounce search and repeated-target rule",
                                           "Building destruction and chain termination order",
                                           "projectile travel and impact tick order",
                                           "King defeat and simultaneous Equipment interactions",
                                           "Hero HP/DPS bonus stacking",
                                           "Viewer projectile art, snapshot, replay and RL commands"]
            if content_id == "snake_bracelet":
                record["th18_levels"] = "1-27 catalogued; official damage-triggered summon and dated 2026 Hero HP/DPS table"
                record["evidence_audit"] = {"file": SNAKE_BRACELET_AUDIT.name,
                                            "sha256": hashlib.sha256(SNAKE_BRACELET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["required_spawned_unit_id"] = "snake_bracelet_snake"
                record["uncertainties"] = ["current damage threshold, mitigation and excess accumulation",
                                           "total versus live Snake cap and revival reset",
                                           "spawn position, collision and entity-ID tick order",
                                           "current Snake L1-10 stats and combat targeting",
                                           "King damage, summon and death phase order",
                                           "Hero HP/DPS and attack-speed stacking and rounding",
                                           "Vampstache, Gauntlet, Spiky Ball and Phoenix interactions",
                                           "Viewer art, snapshot, replay and RL loadout contract"]
            if content_id == "stick_horse":
                record["th18_levels"] = "1-27 catalogued; official limited post-deployment Wall hop and speed/damage effect"
                record["evidence_audit"] = {"file": STICK_HORSE_AUDIT.name,
                                            "sha256": hashlib.sha256(STICK_HORSE_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current duration, movement and attack-speed continuity",
                                           "movement unit and integer fixed-tick conversion",
                                           "Wall-hop path cost, collision and route choice",
                                           "whether Wall hop shares the buff expiry",
                                           "deployment and expiration tick order",
                                           "DPS and cadence stacking and rounding",
                                           "Jump Spell, other Equipment and Pet interactions",
                                           "Viewer trail art, snapshot, replay and RL loadout"]
            if content_id == "archer_puppet":
                record["th18_levels"] = "1-18 catalogued; official Queen DPS/recovery and spawned-Archer invisibility bands and rate"
                record["evidence_audit"] = {"file": ARCHER_PUPPET_AUDIT.name,
                                            "sha256": hashlib.sha256(ARCHER_PUPPET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["official_bonus_invisibility_table"] = {"file": ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY.name,
                                                                 "sha256": hashlib.sha256(ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY.read_bytes()).hexdigest(),
                                                                 "status": "all_18_dps_recovery_invisibility_values_primary_confirmed_spawn_combat_partial"}
                record["required_spawned_unit_id"] = "archer_puppet_archer"
                record["uncertainties"] = ["current summon count and invisibility after 2025 buff",
                                           "child Archer level and ordinary troop stat relation",
                                           "first and later spawn group tick and formation",
                                           "invisibility start/end and target-loss behavior",
                                           "Queen HP recovery cap and death phase order",
                                           "Queen DPS bonus stacking and other Equipment interactions",
                                           "Viewer activation, snapshot, replay and RL command"]
            if content_id == "invisibility_vial":
                record["th18_levels"] = "1-18 catalogued; official Queen invisibility, Rage extra-damage exclusion and clone exclusion"
                record["evidence_audit"] = {"file": INVISIBILITY_VIAL_AUDIT.name,
                                            "sha256": hashlib.sha256(INVISIBILITY_VIAL_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current duration and extra shot damage continuity",
                                           "activation and expiry tick order",
                                           "enemy target loss, reacquisition and projectile handling",
                                           "splash, trap and Pet effects during invisibility",
                                           "base damage and extra bonus staging and rounding",
                                           "Rage, Archer Puppet, Magic Mirror and aura interactions",
                                           "Viewer overlay, snapshot, replay and RL activation"]
            if content_id == "giant_arrow":
                record["th18_levels"] = "1-18 catalogued; official 2026 Air Defense 2x and damage at L9/12/15/18"
                record["evidence_audit"] = {"file": GIANT_ARROW_AUDIT.name,
                                            "sha256": hashlib.sha256(GIANT_ARROW_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["post-May damage at 14 unlisted levels and later continuity",
                                           "arrow direction selection and launch geometry",
                                           "line width, hitboxes, air and ground target eligibility",
                                           "projectile speed, impact and target-loss tick order",
                                           "Air Defense bonus staging and integer rounding",
                                           "Queen HP/DPS stacking and simultaneous Equipment activation",
                                           "Viewer projectile art, snapshot, replay and RL commands"]
            if content_id == "healer_puppet":
                record["th18_levels"] = "1-18 catalogued; official active Healer summon and Queen self-heal/HP"
                record["evidence_audit"] = {"file": HEALER_PUPPET_AUDIT.name,
                                            "sha256": hashlib.sha256(HEALER_PUPPET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["required_spawned_unit_id"] = "healer_puppet_healer"
                record["uncertainties"] = ["current summon count, child level and self-heal continuity",
                                           "self-heal start, accumulation, HP cap and death tick",
                                           "child spawn formation, collision and entity-ID order",
                                           "ordinary Healer stat and AI inheritance",
                                           "child first pulse, target choice and post-Queen-death behavior",
                                           "Queen HP bonus and other Equipment interactions",
                                           "Viewer, snapshot, replay and RL activation"]
            if content_id == "frozen_arrow":
                record["th18_levels"] = "1-27 catalogued; official passive hit slow; dated 2024 post-nerf slow bands"
                record["evidence_audit"] = {"file": FROZEN_ARROW_AUDIT.name,
                                            "sha256": hashlib.sha256(FROZEN_ARROW_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current slow percentage and duration continuity",
                                           "slow target eligibility and immunity",
                                           "movement versus attack-cadence effect and rounding",
                                           "projectile impact and slow start/end tick order",
                                           "refresh, stacking and freeze/poison/Rage interaction",
                                           "Queen DPS bonus and other Equipment staging",
                                           "Viewer overlay, snapshot, replay and RL loadout"]
            if content_id == "magic_mirror":
                record["th18_levels"] = "1-27 catalogued; official Queen clone summon and Equipment bonus exclusions"
                record["evidence_audit"] = {"file": MAGIC_MIRROR_AUDIT.name,
                                            "sha256": hashlib.sha256(MAGIC_MIRROR_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["required_spawned_unit_id"] = "magic_mirror_clone"
                record["uncertainties"] = ["current clone count/HP/DPS/duration and Queen recovery",
                                           "clone spawn position, collision and entity-ID order",
                                           "clone attack cadence, targeting and expiry phase",
                                           "Queen invisibility target loss and tick order",
                                           "Equipment exclusion and other modifier staging",
                                           "Pet, trap, aura and spell interactions",
                                           "Viewer clone art, snapshot, replay and RL activation"]
            if content_id == "action_figure":
                record["th18_levels"] = "1-27 catalogued; official Queen active summon with Giant Giant child"
                record["evidence_audit"] = {"file": ACTION_FIGURE_AUDIT.name,
                                            "sha256": hashlib.sha256(ACTION_FIGURE_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["required_spawned_unit_id"] = "action_figure_giant_giant"
                record["uncertainties"] = ["current child level, HP/DPS and attack contract",
                                           "relationship to temporary event Giant Giant",
                                           "child spawn, collision and entity-ID order",
                                           "January 2026 AI priority semantics",
                                           "Queen self-heal and invisibility timing",
                                           "defense, Wall, trap, aura and Pet interactions",
                                           "Viewer art, snapshot, replay and RL activation"]
            if content_id == "monolith_arrow":
                record["th18_levels"] = "1-27 supplemental; official 14/10/5 percent housing tiers and Queen assignment"
                record["evidence_audit"] = {"file": MONOLITH_ARROW_AUDIT.name,
                                            "sha256": hashlib.sha256(MONOLITH_ARROW_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["activation_class_evidence"] = {
                    "file": MONOLITH_ARROW_PASSIVE.name,
                    "sha256": hashlib.sha256(MONOLITH_ARROW_PASSIVE.read_bytes()).hexdigest(),
                    "status": "passive_explicit_secondary_report_core_headless_viewer_rl_catalogue; combat_partial",
                }
                record["read_only_housing_projection"] = {
                    "status": "core_headless_viewer_rl_exposed",
                    "source": MONOLITH_ARROW_AUDIT.name,
                    "scope": "deployed housing, 0-180/181-250/251+ tier and official 14/10/5 maximum-HP percent; no Equipment effect",
                    "snapshot_replay_hash": "verified_164_test_suite_and_rl_runtime",
                }
                record["uncertainties"] = ["primary or build-pinned Passive confirmation and attack-link timing",
                                           "projectile target, collision and impact tick order",
                                           "deployed-housing sample phase and queued commands",
                                           "maximum-HP damage cap and integer rounding",
                                           "10 percent reduction recipient and formula",
                                           "Queen HP and other Equipment/Pet interactions",
                                           "Viewer color/asset, snapshot, replay and RL access"]
            if content_id == "henchmen_puppet":
                record["th18_levels"] = "1-18 catalogued; active Prince summon with two Henchmen L1-7"
                record["evidence_audit"] = {"file": HENCHMEN_PUPPET_AUDIT.name,
                                            "sha256": hashlib.sha256(HENCHMEN_PUPPET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["required_spawned_unit_id"] = "henchmen_puppet_henchman"
                record["uncertainties"] = ["current two-child count, child mapping and stats",
                                           "summon position, collision and entity-ID order",
                                           "one-second invisibility recipient and target loss",
                                           "Prince HP/DPS stacking and recovery phase",
                                           "child targeting, traps, auras and lifetime",
                                           "Viewer art, snapshot, replay and RL activation"]
            if content_id == "dark_orb":
                record["th18_levels"] = "1-18 catalogued; active Minion Prince damage-and-slow orb"
                record["evidence_audit"] = {"file": DARK_ORB_AUDIT.name,
                                            "sha256": hashlib.sha256(DARK_ORB_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current projectile damage and slow bands",
                                           "launch direction, speed, width and collision",
                                           "target eligibility and damage mitigation",
                                           "slow application, stacking and expiry ordering",
                                           "Hero bonuses and Equipment/Pet interactions",
                                           "Viewer art, snapshot, replay and RL activation"]
            if content_id == "metal_pants":
                record["th18_levels"] = "1-18 catalogued; official active Minion Prince damage barrier"
                record["evidence_audit"] = {"file": METAL_PANTS_AUDIT.name,
                                            "sha256": hashlib.sha256(METAL_PANTS_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current barrier reduction, duration and recovery",
                                           "eligible damage origins and mitigation order",
                                           "activation and expiry tick phase",
                                           "rounding, HP cap and Equipment/Pet stacking",
                                           "defender use and Hero Banner interaction",
                                           "Viewer art, snapshot, replay and RL activation"]
            if content_id == "noble_iron":
                record["th18_levels"] = "1-18 catalogued; official passive opening-projectile boost"
                record["evidence_audit"] = {"file": NOBLE_IRON_AUDIT.name,
                                            "sha256": hashlib.sha256(NOBLE_IRON_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current post-January shot, range, speed and damage values",
                                           "charge consumption on launch, miss or impact",
                                           "range acquisition and projectile target loss",
                                           "attack-speed staging, stacking and rounding",
                                           "defender Banner and HP interaction",
                                           "Viewer projectile, snapshot, replay and RL selection"]
            if content_id == "dark_crown":
                record["th18_levels"] = "1-27 catalogued; official three passive ally-defeat boosts"
                record["evidence_audit"] = {"file": DARK_CROWN_AUDIT.name,
                                            "sha256": hashlib.sha256(DARK_CROWN_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current defeated-housing thresholds and eligible classes",
                                           "Hero/Pet/spawn/clone/revival interactions",
                                           "per-stage HP and damage interpretation",
                                           "current-HP fill and attack-speed stacking",
                                           "simultaneous death and stage tick order",
                                           "Viewer stages, snapshot, replay and RL state"]
            if content_id == "meteor_staff":
                record["th18_levels"] = "1-27 catalogued; official passive nearest-Defense meteors"
                record["evidence_audit"] = {"file": METEOR_STAFF_AUDIT.name,
                                            "sha256": hashlib.sha256(METEOR_STAFF_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current strike cooldown and damage",
                                           "first strike and timer reset phase",
                                           "nearest Defense geometry and eligibility",
                                           "launch, target loss, impact and splash order",
                                           "Prince death and Equipment/Pet interactions",
                                           "Viewer meteor art, snapshot, replay and RL state"]
            if content_id == "eternal_tome":
                record["th18_levels"] = "1-18 catalogued; official active aura damage immunity"
                record["evidence_audit"] = {"file": ETERNAL_TOME_AUDIT.name,
                                            "sha256": hashlib.sha256(ETERNAL_TOME_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current L1-18 duration continuity",
                                           "aura radius, boundary and eligible entity classes",
                                           "entry, exit, grace expiry and damage tick order",
                                           "auto-activation, lethal damage, Phoenix revival",
                                           "damage origins, status effects and mitigation stacking",
                                           "Viewer aura/control, snapshot, replay and RL state"]
            if content_id == "life_gem":
                record["th18_levels"] = "1-18 catalogued; official passive HP aura and 2026 balance changes"
                record["evidence_audit"] = {"file": LIFE_GEM_AUDIT.name,
                                            "sha256": hashlib.sha256(LIFE_GEM_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current post-January HP, cap and Warden bonus values",
                                           "aura radius, boundary and eligible entity classes",
                                           "maximum/current HP conversion, exit and re-entry",
                                           "Apprentice Warden, Equipment and Pet stacking",
                                           "movement, healing, damage and death tick order",
                                           "Viewer aura, snapshot, replay and RL state"]
            if content_id == "rage_gem":
                record["th18_levels"] = "1-18 catalogued; official passive damage aura"
                record["evidence_audit"] = {"file": RAGE_GEM_AUDIT.name,
                                            "sha256": hashlib.sha256(RAGE_GEM_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current aura and Warden attack-speed values",
                                           "aura radius, boundary and eligible entity classes",
                                           "damage origin, staging, rounding and projectile carryover",
                                           "Rage and Equipment stacking",
                                           "Warden self-inclusion, death and revival",
                                           "Viewer aura, snapshot, replay and RL state"]
            if content_id == "healing_tome":
                record["th18_levels"] = "1-18 catalogued; official active Warden/nearby-unit healing and post-2024 duration table"
                record["evidence_audit"] = {"file": HEALING_TOME_AUDIT.name,
                                            "sha256": hashlib.sha256(HEALING_TOME_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["official_duration_table"] = {"file": HEALING_TOME_OFFICIAL_DURATION.name,
                                                     "sha256": hashlib.sha256(HEALING_TOME_OFFICIAL_DURATION.read_bytes()).hexdigest(),
                                                     "status": "all_18_post_2024_durations_primary_confirmed_healing_combat_partial"}
                record["primary_level_provenance"] = {"file": HEALING_TOME_PRIMARY_LEVEL_PROVENANCE.name,
                                                      "sha256": hashlib.sha256(HEALING_TOME_PRIMARY_LEVEL_PROVENANCE.read_bytes()).hexdigest(),
                                                      "status": "all_18_durations_read_only_in_shared_core_healing_combat_partial"}
                record["uncertainties"] = ["current duration continuity, heal rate and Warden recovery",
                                           "aura radius, boundary and eligible entity classes",
                                           "first pulse, interval, rounding and expiry tick",
                                           "entry, exit, overheal, death and revival",
                                           "stacking and simultaneous healing/damage order",
                                           "Viewer aura/control, snapshot, replay and RL state"]
            if content_id == "fireball":
                record["th18_levels"] = "1-27 catalogued; official nearest-Defense area projectile"
                record["evidence_audit"] = {"file": FIREBALL_AUDIT.name,
                                            "sha256": hashlib.sha256(FIREBALL_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current projectile damage and radius continuity",
                                           "building-size target metric and eligible Defenses",
                                           "launch, travel, target loss and impact order",
                                           "splash origin, hitboxes and ground/air class filters",
                                           "damage mitigation and other Equipment interactions",
                                           "Viewer projectile, snapshot, replay and RL state"]
            if content_id == "lavaloon_puppet":
                record["th18_levels"] = "1-27 catalogued; official L21-27 three-unit summon and 2026 housing reduction"
                record["evidence_audit"] = {"file": LAVALOON_PUPPET_AUDIT.name,
                                            "sha256": hashlib.sha256(LAVALOON_PUPPET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["required_spawned_unit_id"] = "lavaloon_puppet_lavaloon"
                record["possible_nested_spawned_unit_id"] = "lavaloon_pup"
                record["uncertainties"] = ["current summon counts and child level continuity outside L21-27",
                                           "spawn placement, activation and entity-ID order",
                                           "spawned Lavaloon HP, attack, movement, death and targeting",
                                           "Pup relationship and complete child combat contract",
                                           "housing, Clone, Recall, auras, Pets and trap interactions",
                                           "Warden bonus stacking and ground/air mode",
                                           "Viewer child art, snapshot, replay and RL state"]
            if content_id == "heroic_torch":
                record["th18_levels"] = "1-27 catalogued; official Warden speed, Totem exclusion and 2026 rebalance"
                record["evidence_audit"] = {"file": HEROIC_TORCH_AUDIT.name,
                                            "sha256": hashlib.sha256(HEROIC_TORCH_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current per-level buffs after January 2026",
                                           "aura radius, boundary and eligible entity classes",
                                           "Warden speed versus other active modifiers",
                                           "Wall crossing, clearance and expiry collision resolution",
                                           "damage reduction origin, stage, rounding and stacking",
                                           "activation, duration, death and tick order",
                                           "Viewer aura, snapshot, replay and RL state"]
            if content_id == "royal_gem":
                record["th18_levels"] = "1-18 catalogued; official per-level post-buff HP/DPS bonuses; active heal and combat application partial"
                record["evidence_audit"] = {"file": ROYAL_GEM_AUDIT.name,
                                            "sha256": hashlib.sha256(ROYAL_GEM_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["official_bonus_table"] = {"file": ROYAL_GEM_OFFICIAL_BONUS.name,
                                                   "sha256": hashlib.sha256(ROYAL_GEM_OFFICIAL_BONUS.read_bytes()).hexdigest(),
                                                   "status": "all_18_hp_dps_pairs_primary_confirmed_combat_application_partial"}
                record["active_heal_recheck"] = {"file": ROYAL_GEM_ACTIVE_HEAL_RECHECK.name,
                                                  "sha256": hashlib.sha256(ROYAL_GEM_ACTIVE_HEAL_RECHECK.read_bytes()).hexdigest(),
                                                  "status": "current_per_level_heal_and_activation_order_unverified_core_effect_partial"}
                record["uncertainties"] = ["current per-level active healing",
                                           "activation command, healing cap and tick order",
                                           "HP/DPS bonus timing, max/current HP conversion and damage-per-hit rounding",
                                           "defending Hero ability applicability",
                                           "Pet, Phoenix, trap and two-Equipment interactions",
                                           "Viewer control, snapshot, replay and RL state"]
            if content_id == "seeking_shield":
                record["th18_levels"] = "1-18 catalogued; official four-Defense projectile and size-aware initial selection"
                record["evidence_audit"] = {"file": SEEKING_SHIELD_AUDIT.name,
                                            "sha256": hashlib.sha256(SEEKING_SHIELD_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current projectile damage and HP bonus continuity",
                                           "initial footprint metric, eligibility and ID ties",
                                           "bounce search, repeats and destroyed-target order",
                                           "launch, travel, impact and damage tick order",
                                           "HP bonus stacking and Hero death interactions",
                                           "Viewer projectile, snapshot, replay and RL state"]
            if content_id == "hog_rider_puppet":
                record["th18_levels"] = "1-18 catalogued; official spawned Hog Riders and L1-7 2026 buffs"
                record["evidence_audit"] = {"file": HOG_RIDER_PUPPET_AUDIT.name,
                                            "sha256": hashlib.sha256(HOG_RIDER_PUPPET_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_or_spawned_actor"}
                record["required_spawned_unit_id"] = "hog_rider_puppet_hog_rider"
                record["uncertainties"] = ["current L1-7 summon counts and child levels",
                                           "current Champion invisibility and recovery",
                                           "activation, recovery, invisibility and spawn order",
                                           "child stat inheritance, formation, collision and IDs",
                                           "first attack, Wall crossing, trap and aura interactions",
                                           "Viewer child art, snapshot, replay and RL state"]
            if content_id == "haste_vial":
                record["th18_levels"] = "1-18 catalogued; official Champion movement and attack-speed boost"
                record["evidence_audit"] = {"file": HASTE_VIAL_AUDIT.name,
                                            "sha256": hashlib.sha256(HASTE_VIAL_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current active speed/duration continuity",
                                           "movement units and fixed-tick path conversion",
                                           "attack cadence formula, rounding and expiry",
                                           "active/passive, DPS and other modifier stacking",
                                           "activation, attack launch and death tick order",
                                           "Viewer effect, snapshot, replay and RL state"]
            if content_id == "rocket_spear":
                record["th18_levels"] = "1-27 catalogued; official Champion charged throws with greater damage and range"
                record["evidence_audit"] = {"file": ROCKET_SPEAR_AUDIT.name,
                                            "sha256": hashlib.sha256(ROCKET_SPEAR_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current charged-shot damage, count and range continuity",
                                           "activation and charge consumption tick order",
                                           "projectile travel, target eligibility and collision",
                                           "splash center, affected entities and damage stage",
                                           "HP/DPS bonus and other Equipment stacking",
                                           "Viewer effect, snapshot, replay and RL state"]
            if content_id == "electro_boots":
                record["th18_levels"] = "1-27 catalogued; official post-2025 aura DPS and self-healing rates"
                record["evidence_audit"] = {"file": ELECTRO_BOOTS_AUDIT.name,
                                            "sha256": hashlib.sha256(ELECTRO_BOOTS_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["official_aura_healing_table"] = {"file": ELECTRO_BOOTS_OFFICIAL_AURA_HEALING.name,
                                                         "sha256": hashlib.sha256(ELECTRO_BOOTS_OFFICIAL_AURA_HEALING.read_bytes()).hexdigest(),
                                                         "status": "all_27_aura_dps_and_heal_rates_primary_confirmed_pulse_combat_partial"}
                record["uncertainties"] = ["current continuity after 2025 and pulse cadence/radius",
                                           "target classes and radius geometry",
                                           "fractional pulse damage and self-heal arithmetic",
                                           "deployment, pulse, movement and death tick order",
                                           "Invisibility, Pet and Equipment interactions",
                                           "Viewer aura, snapshot, replay and RL state"]
            if content_id == "frost_flake":
                record["th18_levels"] = "1-27 catalogued; Champion freeze projectiles and permanent Epic inventory"
                record["evidence_audit"] = {"file": FROST_FLAKE_AUDIT.name,
                                            "sha256": hashlib.sha256(FROST_FLAKE_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current damage, freeze duration, count and slowdown values",
                                           "defense priority, fallback and geometry",
                                           "projectile travel, impact and tick order",
                                           "freeze channels, expiry and stacking",
                                           "Hero, Pet, Wall and trap interactions",
                                           "Viewer flake, snapshot, replay and RL state"]
            if content_id == "fire_heart":
                record["th18_levels"] = "1-18 catalogued; official May 2026 DPS replacement and L15/L18 regeneration changes"
                record["evidence_audit"] = {"file": FIRE_HEART_AUDIT.name,
                                            "sha256": hashlib.sha256(FIRE_HEART_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current L16/L17 regeneration and death burst values",
                                           "fixed-tick healing accrual and rounding",
                                           "death burst target classes and geometry",
                                           "death trigger and simultaneous damage order",
                                           "Duke, Pet and other Equipment interactions",
                                           "Viewer heal/burst, snapshot, replay and RL state"]
            if content_id == "flame_blower":
                record["th18_levels"] = "1-18 catalogued; official fixed activation order and August 2026 facing direction"
                record["evidence_audit"] = {"file": FLAME_BLOWER_AUDIT.name,
                                            "sha256": hashlib.sha256(FLAME_BLOWER_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current damage/recovery and pinned L15 anomaly",
                                           "facing sample, direction vector and geometry",
                                           "flame hit, target eligibility and travel order",
                                           "fixed-tick Rocket Backpack interaction",
                                           "Duke death, recovery and Pet order",
                                           "Viewer flames, snapshot, replay and RL state"]
            if content_id == "stun_blaster":
                record["th18_levels"] = "1-18 catalogued; dated Duke HP/DPS bonuses and pinned shockwave labels"
                record["evidence_audit"] = {"file": STUN_BLASTER_AUDIT.name,
                                            "sha256": hashlib.sha256(STUN_BLASTER_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current damage, stun duration and radius",
                                           "pulse count, spacing, damage rounding and timing",
                                           "target classes, resource/Wall exclusion and geometry",
                                           "stun attack interruption, expiration and stacking",
                                           "claimed recovery and Pet/Equipment interactions",
                                           "Viewer shockwave, snapshot, replay and RL state"]
            if content_id == "electro_fangs":
                record["activation_type_audit"] = {
                    "file": ELECTRO_FANGS_PASSIVE_AUDIT.name,
                    "sha256": hashlib.sha256(ELECTRO_FANGS_PASSIVE_AUDIT.read_bytes()).hexdigest()}
                record["th18_levels"] = "1-18 frozen and dated HP/ore; official passive four-target chain and 20% decay"
                record["evidence_audit"] = {"file": ELECTRO_FANGS_AUDIT.name,
                                            "sha256": hashlib.sha256(ELECTRO_FANGS_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["primary per-level chain damage and count",
                                           "link range, candidate sort and entity-ID ties",
                                           "primary target inclusion and damage stage",
                                           "integer decay rounding and hit order",
                                           "Rage, Duke, Pet and League interactions",
                                           "Viewer lightning, snapshot, replay and RL state"]
            if content_id == "rocket_backpack":
                record["th18_levels"] = "1-27 catalogued; official May L21/L27 damage and August dash/Phoenix fixes"
                record["evidence_audit"] = {"file": ROCKET_BACKPACK_AUDIT.name,
                                            "sha256": hashlib.sha256(ROCKET_BACKPACK_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current penetrating damage L22-26",
                                           "dash path, speed, center and collision",
                                           "damage area geometry and target eligibility",
                                           "Flame Blower facing and activation tick order",
                                           "Phoenix expiry, Duke death, Pet and trap ordering",
                                           "Viewer dash, snapshot, replay and RL state"]
            if content_id == "revenge_deck":
                record["th18_levels"] = "1-27 catalogued; officially released August 2026; dated L1-27 HP and ore table"
                record["evidence_audit"] = {"file": REVENGE_DECK_AUDIT.name,
                                            "sha256": hashlib.sha256(REVENGE_DECK_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_equipment_actor"}
                record["uncertainties"] = ["current-build values and post-event acquisition",
                                           "per-level reflected damage and any healing",
                                           "attacker and hit eligibility, cooldown and counter trigger",
                                           "damage, heal and death tick ordering",
                                           "Royal Rampage, Phoenix and Rocket Backpack interactions",
                                           "loadout, stat stacking and adapter serialization",
                                           "visual effect art and redistribution rights"]
            if content_id == "diggy":
                record["th18_levels"] = "1-15 catalogued; L11-15 dated 2026-09-01 game-file supplement, data-only"
                record["evidence_audit"] = {"file": DIGGY_AUDIT.name,
                                            "sha256": hashlib.sha256(DIGGY_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["exact current client build and L11-15 damage per hit",
                                           "2026 underground and skeleton targeting rules",
                                           "L15 stun magnitude and application order",
                                           "Hero reassignment priority and tick order",
                                           "spawn, follow, collision and battlefield art rights"]
            if content_id == "lassi":
                record["th18_levels"] = "1-15 catalogued; current build unverified"
                record["evidence_audit"] = {"file": LASSI_AUDIT.name,
                                            "sha256": hashlib.sha256(LASSI_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["Hero-linked spawn and follow timing",
                                           "target acquisition, loss and post-Hero-death behavior",
                                           "Wall-jump collision and duration", "battlefield art and rights"]
            if content_id == "electro_owl":
                record["th18_levels"] = "1-15 catalogued; official 2024 L11-15 HP and DPS; current build unverified"
                record["evidence_audit"] = {"file": ELECTRO_OWL_AUDIT.name,
                                            "sha256": hashlib.sha256(ELECTRO_OWL_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["current-build Pet values",
                                           "Hero-linked spawn, follow and post-Hero-death targeting",
                                           "chain bounce radius, eligibility and second-target order",
                                           "projectile launch, impact and target-loss timing",
                                           "air collision, visual anchors and battlefield art rights"]
            if content_id == "mighty_yak":
                record["th18_levels"] = "1-15 catalogued; official 2023 L11-15 HP and DPS; current build unverified"
                record["evidence_audit"] = {"file": MIGHTY_YAK_AUDIT.name,
                                            "sha256": hashlib.sha256(MIGHTY_YAK_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["current-build values and movement speed",
                                           "Hero-linked spawn, follow and knockout ordering",
                                           "post-2026 wall selection and seven-tile preference semantics",
                                           "wall multiplier and rage continuity",
                                           "retarget choice and tick timing",
                                           "collision, anchors and battlefield art rights"]
            if content_id == "unicorn":
                record["th18_levels"] = "1-15 catalogued; official L4-10 healing and L11-15 availability; current build unverified"
                record["evidence_audit"] = {"file": UNICORN_AUDIT.name,
                                            "sha256": hashlib.sha256(UNICORN_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["current-build HP and healing values",
                                           "paired deployment and follow position",
                                           "first and repeat healing pulse timing and damage order",
                                           "healing cap, modifiers and target eligibility",
                                           "post-Hero-loss target priority and tie-break",
                                           "collision, anchors and battlefield art rights"]
            if content_id == "poison_lizard":
                record["th18_levels"] = "1-15 catalogued; dated 2026-09-01 HP and DPS table; current effect timing unverified"
                record["evidence_audit"] = {"file": POISON_LIZARD_AUDIT.name,
                                            "sha256": hashlib.sha256(POISON_LIZARD_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["exact current client build and poison-field continuity",
                                           "Hero-linked spawn and follow geometry",
                                           "enemy-unit search radius and target ordering",
                                           "fallback to Hero target and post-Hero-loss transition",
                                           "poison pulse, duration, stacking and immunity",
                                           "projectile impact and target-loss timing",
                                           "collision and battlefield art rights"]
            if content_id == "phoenix":
                record["th18_levels"] = "1-10 catalogued; TH18 maximum and HP/DPS confirmed by dated 2026-09-01 game-file table"
                record["evidence_audit"] = {"file": PHOENIX_AUDIT.name,
                                            "sha256": hashlib.sha256(PHOENIX_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["current-build revival duration continuity",
                                           "egg spawn, follow, targetability and Wall clearance",
                                           "fatal-hit interception and protected Hero HP",
                                           "shield start, expiry and defeat event order",
                                           "Grand Warden passive aura while protected",
                                           "Dragon Duke Rocket Backpack dash completion ordering",
                                           "Revive Spell interaction",
                                           "hatched attack target, splash radius and timing",
                                           "collision and battlefield art rights"]
            if content_id == "spirit_fox":
                record["th18_levels"] = "1-10 catalogued; official 2023 invisibility durations and 2024 six-second visible time; dated 2026-09-01 HP/DPS"
                record["evidence_audit"] = {"file": SPIRIT_FOX_AUDIT.name,
                                            "sha256": hashlib.sha256(SPIRIT_FOX_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["current-build duration continuity",
                                           "paired spawn, follow, separation and retarget timing",
                                           "first Spirit Walk trigger and tick boundaries",
                                           "Fox and Hero target loss while invisible",
                                           "projectile, trap, aura and Equipment interactions",
                                           "attack impact and target ordering",
                                           "collision and battlefield art rights"]
            if content_id == "angry_jelly":
                record["th18_levels"] = "1-10 catalogued; official Brainwash duration and attached immunity; dated 2026 game-file five-tile range"
                record["evidence_audit"] = {"file": ANGRY_JELLY_AUDIT.name,
                                            "sha256": hashlib.sha256(ANGRY_JELLY_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["current-build duration continuity",
                                           "Hero attachment and first Brainwash tick",
                                           "defense-only target and path transition",
                                           "attached Jelly attack eligibility",
                                           "Hero loss versus Brainwash expiry ordering",
                                           "detached target, attack and trap timing",
                                           "old pinned versus dated range discrepancy",
                                           "Equipment and forced-target interaction order",
                                           "collision and battlefield art rights"]
            if content_id == "sneezy":
                record["th18_levels"] = "1-10 catalogued; dated 2026-09-01 HP/DPS and official spawned-Booger trap change"
                record["evidence_audit"] = {"file": SNEEZY_AUDIT.name,
                                            "sha256": hashlib.sha256(SNEEZY_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor_or_child"}
                record["required_spawned_unit_id"] = "booger"
                record["uncertainties"] = ["current-build parent and child values",
                                           "Hero-linked spawn, follow and range-mode semantics",
                                           "first sneeze, cap and replenishment tick order",
                                           "Hero defeat transition and raw rage scalar meaning",
                                           "parent target and attack impact timing",
                                           "Grand Warden follow weighting for Boogers",
                                           "collision and battlefield art rights"]
            if content_id == "greedy_raven":
                record["th18_levels"] = "1-10 catalogued; official five-times resource damage and dated 2026-09-01 HP/DPS"
                record["evidence_audit"] = {"file": GREEDY_RAVEN_AUDIT.name,
                                            "sha256": hashlib.sha256(GREEDY_RAVEN_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor"}
                record["uncertainties"] = ["current-build per-level resource DPS continuity",
                                           "Hero-linked deployment and post-Hero-loss transitions",
                                           "near-Hero search radius and target tie-break",
                                           "resource-building eligibility and fallback target",
                                           "per-hit multiplier, rounding and modifier order",
                                           "projectile impact and target-loss timing",
                                           "collision and battlefield art rights"]
            if content_id == "frosty":
                record["th18_levels"] = "1-15 catalogued; L11-15 officially released; current values unverified"
                record["evidence_audit"] = {"file": FROSTY_AUDIT.name,
                                            "sha256": hashlib.sha256(FROSTY_AUDIT.read_bytes()).hexdigest(),
                                            "status": "partial_no_core_actor_or_child"}
                record["required_spawned_unit_id"] = "frostmite"
                record["uncertainties"] = ["current-build parent and child values",
                                           "Hero-linked spawn, follow and target transitions",
                                           "Frostmite first spawn, cap and collision order",
                                           "Frostmite acquisition, damage and lifetime",
                                           "slow magnitude, duration and stacking",
                                           "battlefield art and redistribution rights"]
            items.append(record)
    assert len(items) == 60
    spawned_subunits = [{"content_id": "hog_rider_puppet_hog_rider", "parent_content_id": "hog_rider_puppet",
                         "source": {"file": HOG_RIDER_PUPPET_AUDIT.name,
                                    "sha256": hashlib.sha256(HOG_RIDER_PUPPET_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child levels/count and ordinary Hog Rider stat inheritance",
                                           "spawn formation, position, collision and entity-ID order",
                                           "target acquisition, first attack and Wall crossing",
                                           "trap, aura, spell, Pet and Equipment interactions",
                                           "battlefield art and redistribution rights"]},
                        {"content_id": "lavaloon_puppet_lavaloon", "parent_content_id": "lavaloon_puppet",
                         "source": {"file": LAVALOON_PUPPET_AUDIT.name,
                                    "sha256": hashlib.sha256(LAVALOON_PUPPET_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current L1-11 child HP, damage, cadence, range and movement",
                                           "Air Defense priority and fallback",
                                           "spawn and death tick, position, collision and entity-ID order",
                                           "Pup creation, count, level and stats",
                                           "trap, aura, spell, Clone and Recall interactions",
                                           "battlefield art and redistribution rights"]},
                        {"content_id": "lavaloon_pup", "parent_content_id": "lavaloon_puppet_lavaloon",
                         "source": {"file": LAVALOON_PUPPET_AUDIT.name,
                                    "sha256": hashlib.sha256(LAVALOON_PUPPET_AUDIT.read_bytes()).hexdigest()},
                         "relationship": "candidate_unverified_for_permanent_equipment_variant",
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["permanent variant parent relationship and creation condition",
                                           "Pup count, level, spawn tick and position",
                                           "current HP, damage, movement, targeting and lifetime",
                                           "trap, aura and spell interactions", "battlefield art rights"]},
                        {"content_id": "henchmen_puppet_henchman", "parent_content_id": "henchmen_puppet",
                         "source": {"file": HENCHMEN_PUPPET_AUDIT.name,
                                    "sha256": hashlib.sha256(HENCHMEN_PUPPET_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child HP, DPS, cadence, range and movement",
                                           "spawn position, collision and entity-ID order",
                                           "targeting, trap, aura and lifetime interactions",
                                           "battlefield art and redistribution rights"]},
                        {"content_id": "action_figure_giant_giant", "parent_content_id": "action_figure",
                         "source": {"file": ACTION_FIGURE_AUDIT.name,
                                    "sha256": hashlib.sha256(ACTION_FIGURE_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child stats and temporary-event relation",
                                           "spawn position, collision and entity-ID order",
                                           "attack, target priority, rage and splash timing",
                                           "Wall, trap, Pet and aura interactions",
                                           "lifetime, Queen defeat and battlefield art rights"]},
                        {"content_id": "magic_mirror_clone", "parent_content_id": "magic_mirror",
                         "source": {"file": MAGIC_MIRROR_AUDIT.name,
                                    "sha256": hashlib.sha256(MAGIC_MIRROR_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child HP/DPS and duration",
                                           "spawn position, collision and entity-ID order",
                                           "attack, target selection, damage and expiry tick order",
                                           "Equipment bonus exclusions and other modifiers",
                                           "traps, Pets, auras, spell effects and art rights"]},
                        {"content_id": "healer_puppet_healer", "parent_content_id": "healer_puppet",
                         "source": {"file": HEALER_PUPPET_AUDIT.name,
                                    "sha256": hashlib.sha256(HEALER_PUPPET_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child stats and ordinary Healer inheritance",
                                           "spawn formation, position and entity-ID tick order",
                                           "first heal pulse, cadence and target priority",
                                           "ground-only healing and Hero/Pet/clone eligibility",
                                           "traps, post-Queen-death behavior and art rights"]},
                        {"content_id": "archer_puppet_archer", "parent_content_id": "archer_puppet",
                         "source": {"file": ARCHER_PUPPET_AUDIT.name,
                                    "sha256": hashlib.sha256(ARCHER_PUPPET_AUDIT.read_bytes()).hexdigest()},
                         "official_invisibility_spawn_rate": {"file": ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY.name,
                                                               "sha256": hashlib.sha256(ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY.read_bytes()).hexdigest(),
                                                               "units_per_batch": 5, "interval_ms": 1000,
                                                               "status": "historical_primary_confirmed_lifecycle_partial"},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["child level and ordinary Archer stat relation",
                                           "spawn groups, formation, placement and entity-ID order",
                                           "invisibility timing and target acquisition",
                                           "projectile, splash and trap interactions",
                                           "battlefield art and redistribution rights"]},
                        {"content_id": "snake_bracelet_snake", "parent_content_id": "snake_bracelet",
                         "source": {"file": SNAKE_BRACELET_AUDIT.name,
                                    "sha256": hashlib.sha256(SNAKE_BRACELET_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child HP, DPS, cadence, range and movement",
                                           "spawn placement, lifetime and entity-ID order",
                                           "target acquisition, Wall crossing and retargeting",
                                           "trap and aura interactions", "battlefield art rights"]},
                        {"content_id": "barbarian_puppet_barbarian", "parent_content_id": "barbarian_puppet",
                         "source": {"file": BARBARIAN_PUPPET_AUDIT.name,
                                    "sha256": hashlib.sha256(BARBARIAN_PUPPET_AUDIT.read_bytes()).hexdigest()},
                         "official_spawn_rate": {"file": BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN.name,
                                                 "sha256": hashlib.sha256(BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN.read_bytes()).hexdigest(),
                                                 "units_per_batch": 5, "interval_ms": 500,
                                                 "rage_duration_seconds": 20,
                                                 "status": "historical_primary_confirmed_lifecycle_partial"},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current spawned unit level and stats, total count and final batch",
                                           "first batch phase, spawn placement and entity-ID tick order",
                                           "Rage boundary and damage/speed stacking",
                                           "targeting, traps, collision and art rights"]},
                        {"content_id": "frostmite", "parent_content_id": "frosty",
                         "source": {"file": FROSTY_AUDIT.name,
                                    "sha256": hashlib.sha256(FROSTY_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child values and lifetime",
                                           "spawn position and tick order", "targeting and slow contract",
                                           "collision and battlefield art rights"]},
                        {"content_id": "booger", "parent_content_id": "sneezy",
                         "source": {"file": SNEEZY_AUDIT.name,
                                    "sha256": hashlib.sha256(SNEEZY_AUDIT.read_bytes()).hexdigest()},
                         "core": "not_materialized", "headless": "not_exposed",
                         "viewer": "not_exposed", "rl": "not_exposed",
                         "snapshot": "not_tested", "replay": "not_tested", "fidelity": "partial",
                         "uncertainties": ["current child HP, DPS, cadence and lifetime",
                                           "spawn position, cap and replenishment tick order",
                                           "targeting and collision without trap triggering",
                                           "Grand Warden follow weighting", "battlefield art rights"]}]
    for item in items:
        item["attachment_contract"] = "source_backed_slot_cardinality_combat_partial"
        item["prebattle_loadout"] = {
            "status": "core_headless_viewer_rl_value_selection_no_combat_effect",
            "snapshot_replay_hash": "snapshot_binds_selection; logical_hash_excludes_inert_selection; verified_161_test_suite",
            "viewer_control": "all_12_pet_and_42_equipment_picker_mouse_clicks_verified_with_core_validation; verified_162_test_suite",
            "viewer_selection_render": "all_12_pets_and_42_equipment_at_catalogued_max_level_rendered_and_core_accepted; verified_162_test_suite",
            "viewer_playback": "selected_pet_replay_matches_direct_core_after_two_ticks; verified_162_test_suite",
            "headless_rl_per_item": "all_12_pets_and_42_equipment_max_level_records_and_loadouts_verified_through_core_adapters; headless_22_fixed_tick_replays_match_simulation; combat_partial",
        }
        if item["family"] in ("pet", "equipment"):
            item["source_level_record"] = "immutable_core_headless_viewer_rl_provenance_values; all_1073_core_levels_and_161_test_suite_verified; combat_partial"
        if item["content_id"] in {"royal_gem", "earthquake_boots", "vampstache", "barbarian_puppet", "archer_puppet", "electro_boots"}:
            item["primary_level_provenance"] = {
                "file": SIX_EQUIPMENT_PRIMARY_LEVEL_PROVENANCE.name,
                "sha256": hashlib.sha256(SIX_EQUIPMENT_PRIMARY_LEVEL_PROVENANCE.read_bytes()).hexdigest(),
                "status": "official_level_values_in_shared_immutable_core; current_client_and_combat_partial",
            }
    return {"schema_version": 1, "inventory_id": "home-village-th18-hero-pet-equipment-coverage-2026-09-23",
            "scope_reference": {"file": SCOPE.name, "sha256": hashlib.sha256(SCOPE.read_bytes()).hexdigest()},
            "shared_attachment_audit": {"file": SHARED_ATTACHMENT_AUDIT.name,
                                        "sha256": hashlib.sha256(SHARED_ATTACHMENT_AUDIT.read_bytes()).hexdigest()},
            "frozen_reference": {"file": REFERENCE.name, "sha256": hashlib.sha256(REFERENCE.read_bytes()).hexdigest()},
            "counts": {"heroes": 6, "pets": 12, "equipment": 42},
            "status_rule": "Catalogue and prebattle loadout values are not combat integration. Each combat axis advances only after sourced Core, adapter, serialization and focused verification.",
            "items": items, "spawned_subunits": spawned_subunits}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    expected = json.dumps(build(), indent=2, ensure_ascii=False) + "\n"
    if args.check:
        assert OUTPUT.read_text(encoding="utf-8") == expected, "Hero goal coverage matrix is stale"
        print("Hero goal coverage passed: 6 Heroes, 12 Pets, 42 Equipment items")
    else:
        OUTPUT.write_text(expected, encoding="utf-8")


if __name__ == "__main__":
    main()
