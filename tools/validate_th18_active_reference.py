#!/usr/bin/env python3
"""Validate the additive active TH18 reference and its imported delta level."""
from __future__ import annotations

import hashlib
import json
import pathlib
from decimal import Decimal

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
X_BOW_FOOTPRINT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-x-bow-footprint.json"
ELECTRO_DRAGON_CHAIN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-electro-dragon-chain.json"
BOWLER_BOUNCE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-bowler-bounce.json"
ICE_GOLEM_DEATH_FREEZE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-ice-golem-death-freeze.json"
APPRENTICE_WARDEN_LIFE_AURA_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-apprentice-warden-life-aura.json"
SUPER_HOG_RIDER_SPLIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-hog-rider-split.json"
SUPER_MINER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-miner.json"
SUPER_VALKYRIE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-valkyrie.json"
SUPER_YETI_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-yeti-baseline.json"
SUPER_WITCH_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-super-witch-baseline.json"
ICE_HOUND_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-ice-hound-ice-pup.json"
YETI_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-yeti-yetimite.json"
WITCH_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-witch-skeleton-baseline.json"
LAVA_HOUND_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-lava-hound-lava-pup-baseline.json"
HEADHUNTER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-headhunter-baseline.json"
DRUID_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-druid-bear-baseline.json"
FURNACE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-furnace-firemite-baseline.json"
METEOR_GOLEM_DELTA = ROOT / "data" / "reference" / "th18-2026-09-18-meteor-golem-meteormite-baseline.json"
RUIN_WITCH_KNIGHT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-ruin-witch-knight-contract.json"
BABY_DRAGON_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-baby-dragon-tantrum-contract.json"
PEKKA_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-pekka-baseline.json"
GOBLIN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-goblin-baseline.json"
MINION_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-minion-baseline.json"
HOG_RIDER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-hog-rider-baseline.json"
VALKYRIE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-valkyrie-baseline.json"
DRAGON_RIDER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-dragon-rider-baseline.json"
MINER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-miner-baseline.json"
ELECTRO_DRAGON_CURRENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-electro-dragon-current-baseline.json"
RUIN_WITCH_KNIGHT_CURRENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-ruin-witch-knight-current-evidence.json"
DRAGON_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-dragon-baseline.json"
BALLOON_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-balloon-baseline.json"
GOLEM_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-golem-golemite-baseline.json"
WIZARD_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-wizard-baseline.json"
GIANT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-giant-baseline.json"
ARCHER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-archer-baseline.json"
BARBARIAN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-barbarian-baseline.json"
HEALER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-healer-baseline.json"
WALL_BREAKER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-wall-breaker-baseline.json"
ELECTRO_TITAN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-electro-titan-baseline.json"
THROWER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-thrower-baseline.json"
ROOT_RIDER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-root-rider-baseline.json"
SNEAKY_GOBLIN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-sneaky-goblin-baseline.json"
AIR_SWEEPER_CURRENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-air-sweeper-current-evidence.json"
SPELL_TOWER_CURRENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-spell-tower-current-evidence.json"
FIRESPITTER_CURRENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-firespitter-current-evidence.json"
BUILDERS_HUT_CURRENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-builders-hut-current-evidence.json"
RICOCHET_CANNON_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-ricochet-cannon-baseline.json"
SUPER_WIZARD_TOWER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-super-wizard-tower-baseline.json"
REVENGE_TOWER_CURRENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-revenge-tower-current-evidence.json"
MULTI_ARCHER_TOWER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-multi-archer-tower-baseline.json"
MULTI_GEAR_TOWER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-multi-gear-tower-baseline.json"
SCATTERSHOT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-scattershot-baseline.json"
EAGLE_ARTILLERY_SCOPE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-eagle-artillery-th18-scope.json"
TOWN_HALL_PASSIVE_SCOPE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-town-hall-th18-passive-scope.json"
BOMB_TOWER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-bomb-tower-baseline.json"
X_BOW_DELTA = ROOT / "data" / "reference" / "th18-2026-09-19-x-bow-baseline.json"
INFERNO_TOWER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-inferno-tower-baseline.json"
RUIN_WITCH_KNIGHT_FOLLOWUP_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-ruin-witch-knight-followup.json"
FURNACE_FIREMITE_FOLLOWUP_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-furnace-firemite-followup.json"
METEOR_GOLEM_METEORMITE_FOLLOWUP_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-meteor-golem-meteormite-followup.json"
REVENGE_TOWER_FOLLOWUP_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-revenge-tower-followup.json"
CRAFTING_STATION_PASSIVE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-crafting-station-passive-baseline.json"
ELECTROMITE_FOLLOWUP_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-electromite-followup.json"
BIG_BOY_FOLLOWUP_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-big-boy-followup.json"
LAVA_HOUND_LAVA_PUP_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-lava-hound-lava-pup-baseline.json"
RUIN_WITCH_MOVEMENT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-ruin-witch-movement-followup.json"
AIR_SWEEPER_RECHECK_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-air-sweeper-mechanics-recheck.json"
BUILDERS_HUT_MULTI_BUILDER_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-builders-hut-multi-builder-followup.json"
FURNACE_FIREMITE_LIFECYCLE_RECHECK_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-furnace-firemite-lifecycle-recheck.json"
SPAWNED_SUBUNIT_ASSETS_RECHECK_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-spawned-subunit-assets-recheck.json"
GRAND_WARDEN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-grand-warden-baseline.json"
ROYAL_CHAMPION_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-royal-champion-baseline.json"
MINION_PRINCE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-minion-prince-baseline.json"
DRAGON_DUKE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-dragon-duke-current-evidence.json"
DRAGON_DUKE_BASE_ATTACK_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-dragon-duke-base-attack.json"
ROYAL_CHAMPION_WALL_CROSSING_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-royal-champion-wall-crossing.json"
HERO_SCOPE_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-hero-scope-audit.json"
HERO_VISUAL_ICONS_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-hero-visual-icons.json"
HERO_BANNER_DEFENSE_RECHECK_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-hero-banner-defense-recheck.json"
FLYING_HERO_DEFENSE_TARGETABILITY_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-flying-hero-defense-targetability.json"
GRAND_WARDEN_AIR_MODE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-grand-warden-air-mode.json"
HERO_BANNER_VISUAL_VARIANTS_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-hero-banner-visual-variants.json"
HERO_BANNER_ASSIGNMENT_OFFICIAL_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-hero-banner-assignment-official.json"
HERO_BANNER_TH18_QUANTITY_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-hero-banner-th18-quantity.json"
ATTACKING_HERO_SLOTS_DELTA = ROOT / "data" / "reference" / "th18-2026-09-20-attacking-hero-slots.json"
HERO_BATTLEFIELD_ASSET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-21-hero-battlefield-asset-audit.json"
HERO_ASSET_RIGHTS_POLICY_DELTA = ROOT / "data" / "reference" / "th18-2026-09-21-hero-asset-rights-policy.json"
HERO_PROJECTILE_CONTRACT_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-21-hero-projectile-contract-audit.json"
DRAGON_DUKE_TRAP_MITIGATION_DELTA = ROOT / "data" / "reference" / "th18-2026-09-21-dragon-duke-trap-mitigation.json"
HERO_FRIENDLY_CHALLENGE_MODIFIER_SCOPE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-21-hero-friendly-challenge-modifier-scope.json"
HERO_BANNER_DEFENSE_BEHAVIOR_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-21-hero-banner-defense-behavior-audit.json"
HERO_COLLISION_GEOMETRY_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-21-hero-collision-geometry-audit.json"
HERO_BATTLEFIELD_ART_RIGHTS_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-hero-battlefield-art-rights-audit.json"
ROYAL_GEM_OFFICIAL_LEVEL_BONUS_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-royal-gem-official-level-bonus-table.json"
EARTHQUAKE_BOOTS_OFFICIAL_RADIUS_DAMAGE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-earthquake-boots-official-radius-and-damage-table.json"
VAMPSTACHE_OFFICIAL_DPS_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-vampstache-official-dps-table.json"
BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-barbarian-puppet-official-bonus-and-spawn-rate.json"
ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-archer-puppet-official-bonus-and-invisibility-table.json"
ELECTRO_BOOTS_OFFICIAL_AURA_HEALING_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-electro-boots-official-aura-and-healing-table.json"
HEALING_TOME_OFFICIAL_DURATION_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-healing-tome-official-duration-table.json"
HEALING_TOME_PRIMARY_LEVEL_PROVENANCE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-healing-tome-primary-level-provenance.json"
SIX_EQUIPMENT_PRIMARY_LEVEL_PROVENANCE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-six-equipment-primary-level-provenance.json"
ROYAL_GEM_ACTIVE_HEAL_RECHECK_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-royal-gem-active-heal-source-recheck.json"
MONOLITH_ARROW_PASSIVE_TYPE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-monolith-arrow-passive-type-override.json"
HERO_DEFENSE_EVIDENCE_CLOSURE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-hero-defense-evidence-closure.json"
METEOR_GOLEM_METEORMITE_EVIDENCE_RECHECK_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-meteor-golem-meteormite-evidence-recheck.json"
HERO_PET_EQUIPMENT_SCOPE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-hero-pet-equipment-scope.json"
LASSI_COMBAT_CONTRACT_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-lassi-combat-contract-audit.json"
ELECTRO_OWL_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-electro-owl-level-and-behavior-audit.json"
MIGHTY_YAK_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-mighty-yak-level-and-wall-contract-audit.json"
UNICORN_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-unicorn-healing-and-lifecycle-audit.json"
FROSTY_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-frosty-frostmite-contract-audit.json"
DIGGY_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-diggy-current-level-and-behavior-audit.json"
POISON_LIZARD_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-poison-lizard-level-and-effect-audit.json"
POISON_LIZARD_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "poison-lizard.json"
PHOENIX_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-phoenix-revival-and-interaction-audit.json"
PHOENIX_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "phoenix.json"
SPIRIT_FOX_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-spirit-fox-invisibility-contract-audit.json"
SPIRIT_FOX_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "spirit-fox.json"
ANGRY_JELLY_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-angry-jelly-brainwash-contract-audit.json"
ANGRY_JELLY_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "angry-jelly.json"
SNEEZY_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-sneezy-booger-contract-audit.json"
SNEEZY_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "sneezy.json"
GREEDY_RAVEN_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-greedy-raven-resource-target-audit.json"
GREEDY_RAVEN_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "greedy-raven.json"
REVENGE_DECK_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json"
REVENGE_DECK_OBSERVATION = ROOT / "data" / "reference" / "th18-2026-09-23-revenge-deck-l1-l27-observation.json"
BARBARIAN_PUPPET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-barbarian-puppet-summon-contract-audit.json"
BARBARIAN_PUPPET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "barbarian-puppet.json"
RAGE_VIAL_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-rage-vial-effect-order-audit.json"
RAGE_VIAL_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "rage-vial.json"
EARTHQUAKE_BOOTS_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-earthquake-boots-ground-target-audit.json"
EARTHQUAKE_BOOTS_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "earthquake-boots.json"
VAMPSTACHE_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-vampstache-heal-event-audit.json"
VAMPSTACHE_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "vampstache.json"
GIANT_GAUNTLET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-giant-gauntlet-current-effect-audit.json"
GIANT_GAUNTLET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "giant-gauntlet.json"
SPIKY_BALL_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-spiky-ball-bounce-and-count-audit.json"
SPIKY_BALL_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "spiky-ball.json"
SNAKE_BRACELET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-snake-bracelet-spawn-contract-audit.json"
SNAKE_BRACELET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "snake-bracelet.json"
STICK_HORSE_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-stick-horse-deployment-and-wall-audit.json"
STICK_HORSE_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "stick-horse.json"
ARCHER_PUPPET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-archer-puppet-summon-contract-audit.json"
ARCHER_PUPPET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "archer-puppet.json"
INVISIBILITY_VIAL_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-invisibility-vial-damage-and-target-audit.json"
INVISIBILITY_VIAL_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "invisibility-vial.json"
GIANT_ARROW_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-giant-arrow-air-defense-damage-audit.json"
GIANT_ARROW_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "giant-arrow.json"
HEALER_PUPPET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-healer-puppet-summon-and-healing-audit.json"
HEALER_PUPPET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "healer-puppet.json"
FROZEN_ARROW_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-frozen-arrow-slow-and-impact-audit.json"
FROZEN_ARROW_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "frozen-arrow.json"
MAGIC_MIRROR_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-magic-mirror-clone-and-invisibility-audit.json"
MAGIC_MIRROR_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "magic-mirror.json"
ACTION_FIGURE_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-action-figure-giant-giant-contract-audit.json"
ACTION_FIGURE_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "action-figure.json"
MONOLITH_ARROW_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-monolith-arrow-housing-and-projectile-audit.json"
MONOLITH_ARROW_OBSERVATION = ROOT / "data" / "reference" / "monolith-arrow-observation-2026-09-17.json"
HENCHMEN_PUPPET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-henchmen-puppet-and-child-contract-audit.json"
HENCHMEN_PUPPET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "henchmen-puppet.json"
DARK_ORB_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-dark-orb-projectile-and-slow-audit.json"
DARK_ORB_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "dark-orb.json"
METAL_PANTS_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-metal-pants-barrier-and-recovery-audit.json"
METAL_PANTS_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "metal-pants.json"
NOBLE_IRON_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-noble-iron-opening-shots-audit.json"
NOBLE_IRON_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "noble-iron.json"
DARK_CROWN_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-dark-crown-defeat-threshold-audit.json"
DARK_CROWN_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "dark-crown.json"
METEOR_STAFF_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-meteor-staff-target-and-cadence-audit.json"
METEOR_STAFF_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "meteor-staff.json"
ETERNAL_TOME_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-eternal-tome-aura-immunity-audit.json"
ETERNAL_TOME_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "eternal-tome.json"
LIFE_GEM_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-life-gem-balance-and-aura-audit.json"
LIFE_GEM_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "life-gem.json"
RAGE_GEM_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-rage-gem-aura-and-stacking-audit.json"
RAGE_GEM_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "rage-gem.json"
HEALING_TOME_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-healing-tome-aura-and-pulse-audit.json"
HEALING_TOME_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "healing-tome.json"
FIREBALL_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-fireball-target-size-and-splash-audit.json"
FIREBALL_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "fireball.json"
LAVALOON_PUPPET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-lavaloon-puppet-and-child-contract-audit.json"
LAVALOON_PUPPET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "lavaloon-puppet.json"
HEROIC_TORCH_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-heroic-torch-balance-and-wall-audit.json"
HEROIC_TORCH_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "heroic-torch.json"
ROYAL_GEM_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-royal-gem-recovery-and-bonus-audit.json"
ROYAL_GEM_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "royal-gem.json"
SEEKING_SHIELD_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-seeking-shield-target-and-bounce-audit.json"
SEEKING_SHIELD_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "seeking-shield.json"
HOG_RIDER_PUPPET_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-hog-rider-puppet-and-child-audit.json"
HOG_RIDER_PUPPET_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "hog-rider-puppet.json"
HASTE_VIAL_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-haste-vial-cadence-and-speed-audit.json"
HASTE_VIAL_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "haste-vial.json"
ROCKET_SPEAR_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-rocket-spear-charged-shots-audit.json"
ROCKET_SPEAR_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "rocket-spear.json"
ELECTRO_BOOTS_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-electro-boots-aura-and-balance-audit.json"
ELECTRO_BOOTS_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "electro-boots.json"
FROST_FLAKE_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-frost-flake-freeze-and-target-audit.json"
FROST_FLAKE_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "frost-flake.json"
FIRE_HEART_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-fire-heart-balance-and-death-burst-audit.json"
FIRE_HEART_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "fire-heart.json"
FLAME_BLOWER_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-flame-blower-direction-and-activation-audit.json"
FLAME_BLOWER_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "flame-blower.json"
STUN_BLASTER_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-stun-blaster-shockwave-and-target-audit.json"
STUN_BLASTER_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "stun-blaster.json"
ELECTRO_FANGS_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-electro-fangs-chain-and-target-audit.json"
ELECTRO_FANGS_OBSERVATION = ROOT / "data" / "reference" / "electro-fangs-observation-2026-09-17.json"
ROCKET_BACKPACK_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-rocket-backpack-dash-and-balance-audit.json"
HERO_ATTACHMENT_AUDIT_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-hero-pet-equipment-attachment-contract-audit.json"
ELECTRO_FANGS_PASSIVE_DELTA = ROOT / "data" / "reference" / "th18-2026-09-23-electro-fangs-passive-type-override.json"
ROCKET_BACKPACK_RAW = ROOT / "data" / "raw" / "62b019df868f9decb49e97a0ff5ae15fb27af9ba" / "rocket-backpack.json"
DIGGY_L11_L15_OBSERVATION = ROOT / "data" / "reference" / "th18-2026-09-23-diggy-l11-l15-observation.json"
OBSERVATION = ROOT / "data" / "reference" / "monolith-l5-observation-2026-09-18.json"
HEADHUNTER_OBSERVATION = ROOT / "data" / "reference" / "headhunter-l4-observation-2026-09-18.json"
DRUID_OBSERVATION = ROOT / "data" / "reference" / "druid-l6-observation-2026-09-18.json"
DRUID_BEAR_OBSERVATION = ROOT / "data" / "reference" / "druid-bear-observation-2026-09-18.json"
FIREMITE_OBSERVATION = ROOT / "data" / "reference" / "firemite-observation-2026-09-18.json"
SNEAKY_GOBLIN_OBSERVATION = ROOT / "data" / "reference" / "sneaky-goblin-l10-observation-2026-09-19.json"
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
    x_bow_footprint_bytes = X_BOW_FOOTPRINT_DELTA.read_bytes()
    x_bow_footprint_delta = json.loads(x_bow_footprint_bytes)
    electro_dragon_chain_bytes = ELECTRO_DRAGON_CHAIN_DELTA.read_bytes()
    electro_dragon_chain_delta = json.loads(electro_dragon_chain_bytes)
    bowler_bounce_bytes = BOWLER_BOUNCE_DELTA.read_bytes()
    bowler_bounce_delta = json.loads(bowler_bounce_bytes)
    ice_golem_death_freeze_bytes = ICE_GOLEM_DEATH_FREEZE_DELTA.read_bytes()
    ice_golem_death_freeze_delta = json.loads(ice_golem_death_freeze_bytes)
    apprentice_warden_life_aura_bytes = APPRENTICE_WARDEN_LIFE_AURA_DELTA.read_bytes()
    apprentice_warden_life_aura_delta = json.loads(apprentice_warden_life_aura_bytes)
    super_hog_rider_split_bytes = SUPER_HOG_RIDER_SPLIT_DELTA.read_bytes()
    super_hog_rider_split_delta = json.loads(super_hog_rider_split_bytes)
    super_miner_bytes = SUPER_MINER_DELTA.read_bytes()
    super_miner_delta = json.loads(super_miner_bytes)
    super_valkyrie_bytes = SUPER_VALKYRIE_DELTA.read_bytes()
    super_valkyrie_delta = json.loads(super_valkyrie_bytes)
    super_yeti_bytes = SUPER_YETI_DELTA.read_bytes()
    super_yeti_delta = json.loads(super_yeti_bytes)
    super_witch_bytes = SUPER_WITCH_DELTA.read_bytes()
    super_witch_delta = json.loads(super_witch_bytes)
    ice_hound_bytes = ICE_HOUND_DELTA.read_bytes()
    ice_hound_delta = json.loads(ice_hound_bytes)
    yeti_bytes = YETI_DELTA.read_bytes()
    yeti_delta = json.loads(yeti_bytes)
    witch_bytes = WITCH_DELTA.read_bytes()
    witch_delta = json.loads(witch_bytes)
    lava_hound_bytes = LAVA_HOUND_DELTA.read_bytes()
    lava_hound_delta = json.loads(lava_hound_bytes)
    headhunter_bytes = HEADHUNTER_DELTA.read_bytes()
    headhunter_delta = json.loads(headhunter_bytes)
    druid_bytes = DRUID_DELTA.read_bytes()
    druid_delta = json.loads(druid_bytes)
    furnace_bytes = FURNACE_DELTA.read_bytes()
    furnace_delta = json.loads(furnace_bytes)
    meteor_golem_bytes = METEOR_GOLEM_DELTA.read_bytes()
    meteor_golem_delta = json.loads(meteor_golem_bytes)
    ruin_witch_knight_bytes = RUIN_WITCH_KNIGHT_DELTA.read_bytes()
    ruin_witch_knight_delta = json.loads(ruin_witch_knight_bytes)
    baby_dragon_bytes = BABY_DRAGON_DELTA.read_bytes()
    baby_dragon_delta = json.loads(baby_dragon_bytes)
    pekka_bytes = PEKKA_DELTA.read_bytes()
    pekka_delta = json.loads(pekka_bytes)
    goblin_bytes = GOBLIN_DELTA.read_bytes()
    goblin_delta = json.loads(goblin_bytes)
    minion_bytes = MINION_DELTA.read_bytes()
    minion_delta = json.loads(minion_bytes)
    hog_rider_bytes = HOG_RIDER_DELTA.read_bytes()
    hog_rider_delta = json.loads(hog_rider_bytes)
    valkyrie_bytes = VALKYRIE_DELTA.read_bytes()
    valkyrie_delta = json.loads(valkyrie_bytes)
    dragon_rider_bytes = DRAGON_RIDER_DELTA.read_bytes()
    dragon_rider_delta = json.loads(dragon_rider_bytes)
    miner_bytes = MINER_DELTA.read_bytes()
    miner_delta = json.loads(miner_bytes)
    electro_dragon_current_bytes = ELECTRO_DRAGON_CURRENT_DELTA.read_bytes()
    electro_dragon_current_delta = json.loads(electro_dragon_current_bytes)
    ruin_witch_knight_current_bytes = RUIN_WITCH_KNIGHT_CURRENT_DELTA.read_bytes()
    ruin_witch_knight_current_delta = json.loads(ruin_witch_knight_current_bytes)
    dragon_bytes = DRAGON_DELTA.read_bytes()
    dragon_delta = json.loads(dragon_bytes)
    balloon_bytes = BALLOON_DELTA.read_bytes()
    balloon_delta = json.loads(balloon_bytes)
    golem_bytes = GOLEM_DELTA.read_bytes()
    golem_delta = json.loads(golem_bytes)
    wizard_bytes = WIZARD_DELTA.read_bytes()
    wizard_delta = json.loads(wizard_bytes)
    giant_bytes = GIANT_DELTA.read_bytes()
    giant_delta = json.loads(giant_bytes)
    archer_bytes = ARCHER_DELTA.read_bytes()
    archer_delta = json.loads(archer_bytes)
    barbarian_bytes = BARBARIAN_DELTA.read_bytes()
    barbarian_delta = json.loads(barbarian_bytes)
    healer_bytes = HEALER_DELTA.read_bytes()
    healer_delta = json.loads(healer_bytes)
    wall_breaker_bytes = WALL_BREAKER_DELTA.read_bytes()
    wall_breaker_delta = json.loads(wall_breaker_bytes)
    electro_titan_bytes = ELECTRO_TITAN_DELTA.read_bytes()
    electro_titan_delta = json.loads(electro_titan_bytes)
    thrower_bytes = THROWER_DELTA.read_bytes()
    thrower_delta = json.loads(thrower_bytes)
    root_rider_bytes = ROOT_RIDER_DELTA.read_bytes()
    root_rider_delta = json.loads(root_rider_bytes)
    sneaky_goblin_bytes = SNEAKY_GOBLIN_DELTA.read_bytes()
    sneaky_goblin_delta = json.loads(sneaky_goblin_bytes)
    air_sweeper_current_bytes = AIR_SWEEPER_CURRENT_DELTA.read_bytes()
    air_sweeper_current_delta = json.loads(air_sweeper_current_bytes)
    spell_tower_current_bytes = SPELL_TOWER_CURRENT_DELTA.read_bytes()
    spell_tower_current_delta = json.loads(spell_tower_current_bytes)
    firespitter_current_bytes = FIRESPITTER_CURRENT_DELTA.read_bytes()
    firespitter_current_delta = json.loads(firespitter_current_bytes)
    builders_hut_current_bytes = BUILDERS_HUT_CURRENT_DELTA.read_bytes()
    builders_hut_current_delta = json.loads(builders_hut_current_bytes)
    ricochet_cannon_bytes = RICOCHET_CANNON_DELTA.read_bytes()
    ricochet_cannon_delta = json.loads(ricochet_cannon_bytes)
    super_wizard_tower_bytes = SUPER_WIZARD_TOWER_DELTA.read_bytes()
    super_wizard_tower_delta = json.loads(super_wizard_tower_bytes)
    revenge_tower_current_bytes = REVENGE_TOWER_CURRENT_DELTA.read_bytes()
    revenge_tower_current_delta = json.loads(revenge_tower_current_bytes)
    multi_archer_tower_bytes = MULTI_ARCHER_TOWER_DELTA.read_bytes()
    multi_archer_tower_delta = json.loads(multi_archer_tower_bytes)
    multi_gear_tower_bytes = MULTI_GEAR_TOWER_DELTA.read_bytes()
    multi_gear_tower_delta = json.loads(multi_gear_tower_bytes)
    scattershot_bytes = SCATTERSHOT_DELTA.read_bytes()
    scattershot_delta = json.loads(scattershot_bytes)
    eagle_artillery_scope_bytes = EAGLE_ARTILLERY_SCOPE_DELTA.read_bytes()
    eagle_artillery_scope_delta = json.loads(eagle_artillery_scope_bytes)
    town_hall_passive_scope_bytes = TOWN_HALL_PASSIVE_SCOPE_DELTA.read_bytes()
    town_hall_passive_scope_delta = json.loads(town_hall_passive_scope_bytes)
    bomb_tower_bytes = BOMB_TOWER_DELTA.read_bytes()
    bomb_tower_delta = json.loads(bomb_tower_bytes)
    x_bow_bytes = X_BOW_DELTA.read_bytes()
    x_bow_delta = json.loads(x_bow_bytes)
    inferno_tower_bytes = INFERNO_TOWER_DELTA.read_bytes()
    inferno_tower_delta = json.loads(inferno_tower_bytes)
    ruin_witch_knight_followup_bytes = RUIN_WITCH_KNIGHT_FOLLOWUP_DELTA.read_bytes()
    ruin_witch_knight_followup_delta = json.loads(ruin_witch_knight_followup_bytes)
    furnace_firemite_followup_bytes = FURNACE_FIREMITE_FOLLOWUP_DELTA.read_bytes()
    furnace_firemite_followup_delta = json.loads(furnace_firemite_followup_bytes)
    meteor_golem_meteormite_followup_bytes = METEOR_GOLEM_METEORMITE_FOLLOWUP_DELTA.read_bytes()
    meteor_golem_meteormite_followup_delta = json.loads(meteor_golem_meteormite_followup_bytes)
    revenge_tower_followup_bytes = REVENGE_TOWER_FOLLOWUP_DELTA.read_bytes()
    revenge_tower_followup_delta = json.loads(revenge_tower_followup_bytes)
    crafting_station_passive_bytes = CRAFTING_STATION_PASSIVE_DELTA.read_bytes()
    crafting_station_passive_delta = json.loads(crafting_station_passive_bytes)
    electromite_followup_bytes = ELECTROMITE_FOLLOWUP_DELTA.read_bytes()
    electromite_followup_delta = json.loads(electromite_followup_bytes)
    big_boy_followup_bytes = BIG_BOY_FOLLOWUP_DELTA.read_bytes()
    big_boy_followup_delta = json.loads(big_boy_followup_bytes)
    lava_hound_lava_pup_bytes = LAVA_HOUND_LAVA_PUP_DELTA.read_bytes()
    lava_hound_lava_pup_delta = json.loads(lava_hound_lava_pup_bytes)
    ruin_witch_movement_bytes = RUIN_WITCH_MOVEMENT_DELTA.read_bytes()
    ruin_witch_movement_delta = json.loads(ruin_witch_movement_bytes)
    air_sweeper_recheck_bytes = AIR_SWEEPER_RECHECK_DELTA.read_bytes()
    air_sweeper_recheck_delta = json.loads(air_sweeper_recheck_bytes)
    builders_hut_multi_builder_bytes = BUILDERS_HUT_MULTI_BUILDER_DELTA.read_bytes()
    builders_hut_multi_builder_delta = json.loads(builders_hut_multi_builder_bytes)
    furnace_firemite_lifecycle_recheck_bytes = FURNACE_FIREMITE_LIFECYCLE_RECHECK_DELTA.read_bytes()
    furnace_firemite_lifecycle_recheck_delta = json.loads(furnace_firemite_lifecycle_recheck_bytes)
    spawned_subunit_assets_recheck_bytes = SPAWNED_SUBUNIT_ASSETS_RECHECK_DELTA.read_bytes()
    spawned_subunit_assets_recheck_delta = json.loads(spawned_subunit_assets_recheck_bytes)
    grand_warden_bytes = GRAND_WARDEN_DELTA.read_bytes()
    grand_warden_delta = json.loads(grand_warden_bytes)
    royal_champion_bytes = ROYAL_CHAMPION_DELTA.read_bytes()
    royal_champion_delta = json.loads(royal_champion_bytes)
    minion_prince_bytes = MINION_PRINCE_DELTA.read_bytes()
    minion_prince_delta = json.loads(minion_prince_bytes)
    dragon_duke_bytes = DRAGON_DUKE_DELTA.read_bytes()
    dragon_duke_delta = json.loads(dragon_duke_bytes)
    dragon_duke_base_attack_bytes = DRAGON_DUKE_BASE_ATTACK_DELTA.read_bytes()
    dragon_duke_base_attack_delta = json.loads(dragon_duke_base_attack_bytes)
    royal_champion_wall_crossing_bytes = ROYAL_CHAMPION_WALL_CROSSING_DELTA.read_bytes()
    royal_champion_wall_crossing_delta = json.loads(royal_champion_wall_crossing_bytes)
    hero_scope_audit_bytes = HERO_SCOPE_AUDIT_DELTA.read_bytes()
    hero_scope_audit_delta = json.loads(hero_scope_audit_bytes)
    hero_visual_icons_bytes = HERO_VISUAL_ICONS_DELTA.read_bytes()
    hero_visual_icons_delta = json.loads(hero_visual_icons_bytes)
    hero_banner_defense_recheck_bytes = HERO_BANNER_DEFENSE_RECHECK_DELTA.read_bytes()
    hero_banner_defense_recheck_delta = json.loads(hero_banner_defense_recheck_bytes)
    flying_hero_defense_targetability_bytes = FLYING_HERO_DEFENSE_TARGETABILITY_DELTA.read_bytes()
    flying_hero_defense_targetability_delta = json.loads(flying_hero_defense_targetability_bytes)
    grand_warden_air_mode_bytes = GRAND_WARDEN_AIR_MODE_DELTA.read_bytes()
    grand_warden_air_mode_delta = json.loads(grand_warden_air_mode_bytes)
    hero_banner_visual_variants_bytes = HERO_BANNER_VISUAL_VARIANTS_DELTA.read_bytes()
    hero_banner_visual_variants_delta = json.loads(hero_banner_visual_variants_bytes)
    hero_banner_assignment_official_bytes = HERO_BANNER_ASSIGNMENT_OFFICIAL_DELTA.read_bytes()
    hero_banner_assignment_official_delta = json.loads(hero_banner_assignment_official_bytes)
    hero_banner_th18_quantity_bytes = HERO_BANNER_TH18_QUANTITY_DELTA.read_bytes()
    hero_banner_th18_quantity_delta = json.loads(hero_banner_th18_quantity_bytes)
    attacking_hero_slots_bytes = ATTACKING_HERO_SLOTS_DELTA.read_bytes()
    attacking_hero_slots_delta = json.loads(attacking_hero_slots_bytes)
    hero_battlefield_asset_audit_bytes = HERO_BATTLEFIELD_ASSET_AUDIT_DELTA.read_bytes()
    hero_battlefield_asset_audit_delta = json.loads(hero_battlefield_asset_audit_bytes)
    hero_asset_rights_policy_bytes = HERO_ASSET_RIGHTS_POLICY_DELTA.read_bytes()
    hero_asset_rights_policy_delta = json.loads(hero_asset_rights_policy_bytes)
    hero_projectile_contract_audit_bytes = HERO_PROJECTILE_CONTRACT_AUDIT_DELTA.read_bytes()
    hero_projectile_contract_audit_delta = json.loads(hero_projectile_contract_audit_bytes)
    dragon_duke_trap_mitigation_bytes = DRAGON_DUKE_TRAP_MITIGATION_DELTA.read_bytes()
    dragon_duke_trap_mitigation_delta = json.loads(dragon_duke_trap_mitigation_bytes)
    hero_friendly_challenge_modifier_scope_bytes = HERO_FRIENDLY_CHALLENGE_MODIFIER_SCOPE_DELTA.read_bytes()
    hero_friendly_challenge_modifier_scope_delta = json.loads(hero_friendly_challenge_modifier_scope_bytes)
    hero_banner_defense_behavior_audit_bytes = HERO_BANNER_DEFENSE_BEHAVIOR_AUDIT_DELTA.read_bytes()
    hero_banner_defense_behavior_audit_delta = json.loads(hero_banner_defense_behavior_audit_bytes)
    hero_collision_geometry_audit_bytes = HERO_COLLISION_GEOMETRY_AUDIT_DELTA.read_bytes()
    hero_collision_geometry_audit_delta = json.loads(hero_collision_geometry_audit_bytes)
    hero_defense_evidence_closure_bytes = HERO_DEFENSE_EVIDENCE_CLOSURE_DELTA.read_bytes()
    hero_defense_evidence_closure_delta = json.loads(hero_defense_evidence_closure_bytes)
    meteor_golem_meteormite_evidence_recheck_bytes = METEOR_GOLEM_METEORMITE_EVIDENCE_RECHECK_DELTA.read_bytes()
    meteor_golem_meteormite_evidence_recheck_delta = json.loads(meteor_golem_meteormite_evidence_recheck_bytes)
    hero_pet_equipment_scope_bytes = HERO_PET_EQUIPMENT_SCOPE_DELTA.read_bytes()
    hero_pet_equipment_scope_delta = json.loads(hero_pet_equipment_scope_bytes)
    lassi_combat_contract_audit_bytes = LASSI_COMBAT_CONTRACT_AUDIT_DELTA.read_bytes()
    lassi_combat_contract_audit_delta = json.loads(lassi_combat_contract_audit_bytes)
    electro_owl_audit_bytes = ELECTRO_OWL_AUDIT_DELTA.read_bytes()
    electro_owl_audit_delta = json.loads(electro_owl_audit_bytes)
    mighty_yak_audit_bytes = MIGHTY_YAK_AUDIT_DELTA.read_bytes()
    mighty_yak_audit_delta = json.loads(mighty_yak_audit_bytes)
    unicorn_audit_bytes = UNICORN_AUDIT_DELTA.read_bytes()
    unicorn_audit_delta = json.loads(unicorn_audit_bytes)
    frosty_audit_bytes = FROSTY_AUDIT_DELTA.read_bytes()
    frosty_audit_delta = json.loads(frosty_audit_bytes)
    diggy_audit_bytes = DIGGY_AUDIT_DELTA.read_bytes()
    diggy_audit_delta = json.loads(diggy_audit_bytes)
    poison_lizard_audit_bytes = POISON_LIZARD_AUDIT_DELTA.read_bytes()
    poison_lizard_audit_delta = json.loads(poison_lizard_audit_bytes)
    phoenix_audit_bytes = PHOENIX_AUDIT_DELTA.read_bytes()
    phoenix_audit_delta = json.loads(phoenix_audit_bytes)
    spirit_fox_audit_bytes = SPIRIT_FOX_AUDIT_DELTA.read_bytes()
    spirit_fox_audit_delta = json.loads(spirit_fox_audit_bytes)
    angry_jelly_audit_bytes = ANGRY_JELLY_AUDIT_DELTA.read_bytes()
    angry_jelly_audit_delta = json.loads(angry_jelly_audit_bytes)
    sneezy_audit_bytes = SNEEZY_AUDIT_DELTA.read_bytes()
    sneezy_audit_delta = json.loads(sneezy_audit_bytes)
    greedy_raven_audit_bytes = GREEDY_RAVEN_AUDIT_DELTA.read_bytes()
    greedy_raven_audit_delta = json.loads(greedy_raven_audit_bytes)
    revenge_deck_audit_bytes = REVENGE_DECK_AUDIT_DELTA.read_bytes()
    revenge_deck_audit_delta = json.loads(revenge_deck_audit_bytes)
    barbarian_puppet_audit_bytes = BARBARIAN_PUPPET_AUDIT_DELTA.read_bytes()
    barbarian_puppet_audit_delta = json.loads(barbarian_puppet_audit_bytes)
    rage_vial_audit_bytes = RAGE_VIAL_AUDIT_DELTA.read_bytes()
    rage_vial_audit_delta = json.loads(rage_vial_audit_bytes)
    earthquake_boots_audit_bytes = EARTHQUAKE_BOOTS_AUDIT_DELTA.read_bytes()
    earthquake_boots_audit_delta = json.loads(earthquake_boots_audit_bytes)
    vampstache_audit_bytes = VAMPSTACHE_AUDIT_DELTA.read_bytes()
    vampstache_audit_delta = json.loads(vampstache_audit_bytes)
    giant_gauntlet_audit_bytes = GIANT_GAUNTLET_AUDIT_DELTA.read_bytes()
    giant_gauntlet_audit_delta = json.loads(giant_gauntlet_audit_bytes)
    spiky_ball_audit_bytes = SPIKY_BALL_AUDIT_DELTA.read_bytes()
    spiky_ball_audit_delta = json.loads(spiky_ball_audit_bytes)
    snake_bracelet_audit_bytes = SNAKE_BRACELET_AUDIT_DELTA.read_bytes()
    snake_bracelet_audit_delta = json.loads(snake_bracelet_audit_bytes)
    stick_horse_audit_bytes = STICK_HORSE_AUDIT_DELTA.read_bytes()
    stick_horse_audit_delta = json.loads(stick_horse_audit_bytes)
    archer_puppet_audit_bytes = ARCHER_PUPPET_AUDIT_DELTA.read_bytes()
    archer_puppet_audit_delta = json.loads(archer_puppet_audit_bytes)
    invisibility_vial_audit_bytes = INVISIBILITY_VIAL_AUDIT_DELTA.read_bytes()
    invisibility_vial_audit_delta = json.loads(invisibility_vial_audit_bytes)
    giant_arrow_audit_bytes = GIANT_ARROW_AUDIT_DELTA.read_bytes()
    giant_arrow_audit_delta = json.loads(giant_arrow_audit_bytes)
    healer_puppet_audit_bytes = HEALER_PUPPET_AUDIT_DELTA.read_bytes()
    healer_puppet_audit_delta = json.loads(healer_puppet_audit_bytes)
    frozen_arrow_audit_bytes = FROZEN_ARROW_AUDIT_DELTA.read_bytes()
    frozen_arrow_audit_delta = json.loads(frozen_arrow_audit_bytes)
    magic_mirror_audit_bytes = MAGIC_MIRROR_AUDIT_DELTA.read_bytes()
    magic_mirror_audit_delta = json.loads(magic_mirror_audit_bytes)
    action_figure_audit_bytes = ACTION_FIGURE_AUDIT_DELTA.read_bytes()
    action_figure_audit_delta = json.loads(action_figure_audit_bytes)
    monolith_arrow_audit_bytes = MONOLITH_ARROW_AUDIT_DELTA.read_bytes()
    monolith_arrow_audit_delta = json.loads(monolith_arrow_audit_bytes)
    henchmen_puppet_audit_bytes = HENCHMEN_PUPPET_AUDIT_DELTA.read_bytes()
    henchmen_puppet_audit_delta = json.loads(henchmen_puppet_audit_bytes)
    dark_orb_audit_bytes = DARK_ORB_AUDIT_DELTA.read_bytes()
    dark_orb_audit_delta = json.loads(dark_orb_audit_bytes)
    metal_pants_audit_bytes = METAL_PANTS_AUDIT_DELTA.read_bytes()
    metal_pants_audit_delta = json.loads(metal_pants_audit_bytes)
    noble_iron_audit_bytes = NOBLE_IRON_AUDIT_DELTA.read_bytes()
    noble_iron_audit_delta = json.loads(noble_iron_audit_bytes)
    dark_crown_audit_bytes = DARK_CROWN_AUDIT_DELTA.read_bytes()
    dark_crown_audit_delta = json.loads(dark_crown_audit_bytes)
    meteor_staff_audit_bytes = METEOR_STAFF_AUDIT_DELTA.read_bytes()
    meteor_staff_audit_delta = json.loads(meteor_staff_audit_bytes)
    eternal_tome_audit_bytes = ETERNAL_TOME_AUDIT_DELTA.read_bytes()
    eternal_tome_audit_delta = json.loads(eternal_tome_audit_bytes)
    life_gem_audit_bytes = LIFE_GEM_AUDIT_DELTA.read_bytes()
    life_gem_audit_delta = json.loads(life_gem_audit_bytes)
    rage_gem_audit_bytes = RAGE_GEM_AUDIT_DELTA.read_bytes()
    rage_gem_audit_delta = json.loads(rage_gem_audit_bytes)
    healing_tome_audit_bytes = HEALING_TOME_AUDIT_DELTA.read_bytes()
    healing_tome_audit_delta = json.loads(healing_tome_audit_bytes)
    fireball_audit_bytes = FIREBALL_AUDIT_DELTA.read_bytes()
    fireball_audit_delta = json.loads(fireball_audit_bytes)
    lavaloon_puppet_audit_bytes = LAVALOON_PUPPET_AUDIT_DELTA.read_bytes()
    lavaloon_puppet_audit_delta = json.loads(lavaloon_puppet_audit_bytes)
    heroic_torch_audit_bytes = HEROIC_TORCH_AUDIT_DELTA.read_bytes()
    heroic_torch_audit_delta = json.loads(heroic_torch_audit_bytes)
    royal_gem_audit_bytes = ROYAL_GEM_AUDIT_DELTA.read_bytes()
    royal_gem_audit_delta = json.loads(royal_gem_audit_bytes)
    seeking_shield_audit_bytes = SEEKING_SHIELD_AUDIT_DELTA.read_bytes()
    seeking_shield_audit_delta = json.loads(seeking_shield_audit_bytes)
    hog_rider_puppet_audit_bytes = HOG_RIDER_PUPPET_AUDIT_DELTA.read_bytes()
    hog_rider_puppet_audit_delta = json.loads(hog_rider_puppet_audit_bytes)
    haste_vial_audit_bytes = HASTE_VIAL_AUDIT_DELTA.read_bytes()
    haste_vial_audit_delta = json.loads(haste_vial_audit_bytes)
    rocket_spear_audit_bytes = ROCKET_SPEAR_AUDIT_DELTA.read_bytes()
    rocket_spear_audit_delta = json.loads(rocket_spear_audit_bytes)
    electro_boots_audit_bytes = ELECTRO_BOOTS_AUDIT_DELTA.read_bytes()
    electro_boots_audit_delta = json.loads(electro_boots_audit_bytes)
    frost_flake_audit_bytes = FROST_FLAKE_AUDIT_DELTA.read_bytes()
    frost_flake_audit_delta = json.loads(frost_flake_audit_bytes)
    fire_heart_audit_bytes = FIRE_HEART_AUDIT_DELTA.read_bytes()
    fire_heart_audit_delta = json.loads(fire_heart_audit_bytes)
    flame_blower_audit_bytes = FLAME_BLOWER_AUDIT_DELTA.read_bytes()
    flame_blower_audit_delta = json.loads(flame_blower_audit_bytes)
    stun_blaster_audit_bytes = STUN_BLASTER_AUDIT_DELTA.read_bytes()
    stun_blaster_audit_delta = json.loads(stun_blaster_audit_bytes)
    electro_fangs_audit_bytes = ELECTRO_FANGS_AUDIT_DELTA.read_bytes()
    electro_fangs_audit_delta = json.loads(electro_fangs_audit_bytes)
    rocket_backpack_audit_bytes = ROCKET_BACKPACK_AUDIT_DELTA.read_bytes()
    rocket_backpack_audit_delta = json.loads(rocket_backpack_audit_bytes)
    hero_attachment_audit_bytes = HERO_ATTACHMENT_AUDIT_DELTA.read_bytes()
    hero_attachment_audit_delta = json.loads(hero_attachment_audit_bytes)
    electro_fangs_passive_bytes = ELECTRO_FANGS_PASSIVE_DELTA.read_bytes()
    electro_fangs_passive_delta = json.loads(electro_fangs_passive_bytes)
    hero_battlefield_art_rights_audit_bytes = HERO_BATTLEFIELD_ART_RIGHTS_AUDIT_DELTA.read_bytes()
    hero_battlefield_art_rights_audit_delta = json.loads(hero_battlefield_art_rights_audit_bytes)
    royal_gem_official_level_bonus_bytes = ROYAL_GEM_OFFICIAL_LEVEL_BONUS_DELTA.read_bytes()
    royal_gem_official_level_bonus_delta = json.loads(royal_gem_official_level_bonus_bytes)
    earthquake_boots_official_radius_damage_bytes = EARTHQUAKE_BOOTS_OFFICIAL_RADIUS_DAMAGE_DELTA.read_bytes()
    earthquake_boots_official_radius_damage_delta = json.loads(earthquake_boots_official_radius_damage_bytes)
    vampstache_official_dps_bytes = VAMPSTACHE_OFFICIAL_DPS_DELTA.read_bytes()
    vampstache_official_dps_delta = json.loads(vampstache_official_dps_bytes)
    barbarian_puppet_official_bonus_spawn_bytes = BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN_DELTA.read_bytes()
    barbarian_puppet_official_bonus_spawn_delta = json.loads(barbarian_puppet_official_bonus_spawn_bytes)
    archer_puppet_official_bonus_invisibility_bytes = ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY_DELTA.read_bytes()
    archer_puppet_official_bonus_invisibility_delta = json.loads(archer_puppet_official_bonus_invisibility_bytes)
    electro_boots_official_aura_healing_bytes = ELECTRO_BOOTS_OFFICIAL_AURA_HEALING_DELTA.read_bytes()
    electro_boots_official_aura_healing_delta = json.loads(electro_boots_official_aura_healing_bytes)
    healing_tome_official_duration_bytes = HEALING_TOME_OFFICIAL_DURATION_DELTA.read_bytes()
    healing_tome_official_duration_delta = json.loads(healing_tome_official_duration_bytes)
    healing_tome_primary_level_provenance_bytes = HEALING_TOME_PRIMARY_LEVEL_PROVENANCE_DELTA.read_bytes()
    healing_tome_primary_level_provenance_delta = json.loads(healing_tome_primary_level_provenance_bytes)
    six_equipment_primary_level_provenance_bytes = SIX_EQUIPMENT_PRIMARY_LEVEL_PROVENANCE_DELTA.read_bytes()
    six_equipment_primary_level_provenance_delta = json.loads(six_equipment_primary_level_provenance_bytes)
    royal_gem_active_heal_recheck_bytes = ROYAL_GEM_ACTIVE_HEAL_RECHECK_DELTA.read_bytes()
    royal_gem_active_heal_recheck_delta = json.loads(royal_gem_active_heal_recheck_bytes)
    monolith_arrow_passive_type_bytes = MONOLITH_ARROW_PASSIVE_TYPE_DELTA.read_bytes()
    monolith_arrow_passive_type_delta = json.loads(monolith_arrow_passive_type_bytes)
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
    assert x_bow_footprint_delta["parent_reference"] == {"file": SUPER_DRAGON_DELTA.name, "reference_id": super_dragon_delta["reference_id"], "sha256": hashlib.sha256(super_dragon_bytes).hexdigest()}
    assert electro_dragon_chain_delta["parent_reference"] == {"file": X_BOW_FOOTPRINT_DELTA.name, "reference_id": x_bow_footprint_delta["reference_id"], "sha256": hashlib.sha256(x_bow_footprint_bytes).hexdigest()}
    assert bowler_bounce_delta["parent_reference"] == {"file": ELECTRO_DRAGON_CHAIN_DELTA.name, "reference_id": electro_dragon_chain_delta["reference_id"], "sha256": hashlib.sha256(electro_dragon_chain_bytes).hexdigest()}
    assert ice_golem_death_freeze_delta["parent_reference"] == {"file": BOWLER_BOUNCE_DELTA.name, "reference_id": bowler_bounce_delta["reference_id"], "sha256": hashlib.sha256(bowler_bounce_bytes).hexdigest()}
    assert apprentice_warden_life_aura_delta["parent_reference"] == {"file": ICE_GOLEM_DEATH_FREEZE_DELTA.name, "reference_id": ice_golem_death_freeze_delta["reference_id"], "sha256": hashlib.sha256(ice_golem_death_freeze_bytes).hexdigest()}
    assert super_hog_rider_split_delta["parent_reference"] == {"file": APPRENTICE_WARDEN_LIFE_AURA_DELTA.name, "reference_id": apprentice_warden_life_aura_delta["reference_id"], "sha256": hashlib.sha256(apprentice_warden_life_aura_bytes).hexdigest()}
    assert super_miner_delta["parent_reference"] == {"file": SUPER_HOG_RIDER_SPLIT_DELTA.name, "reference_id": super_hog_rider_split_delta["reference_id"], "sha256": hashlib.sha256(super_hog_rider_split_bytes).hexdigest()}
    assert super_valkyrie_delta["parent_reference"] == {"file": SUPER_MINER_DELTA.name, "reference_id": super_miner_delta["reference_id"], "sha256": hashlib.sha256(super_miner_bytes).hexdigest()}
    assert super_yeti_delta["parent_reference"] == {"file": SUPER_VALKYRIE_DELTA.name, "reference_id": super_valkyrie_delta["reference_id"], "sha256": hashlib.sha256(super_valkyrie_bytes).hexdigest()}
    assert super_witch_delta["parent_reference"] == {"file": SUPER_YETI_DELTA.name, "reference_id": super_yeti_delta["reference_id"], "sha256": hashlib.sha256(super_yeti_bytes).hexdigest()}
    assert ice_hound_delta["parent_reference"] == {"file": SUPER_WITCH_DELTA.name, "reference_id": super_witch_delta["reference_id"], "sha256": hashlib.sha256(super_witch_bytes).hexdigest()}
    assert yeti_delta["parent_reference"] == {"file": ICE_HOUND_DELTA.name, "reference_id": ice_hound_delta["reference_id"], "sha256": hashlib.sha256(ice_hound_bytes).hexdigest()}
    assert witch_delta["parent_reference"] == {"file": YETI_DELTA.name, "reference_id": yeti_delta["reference_id"], "sha256": hashlib.sha256(yeti_bytes).hexdigest()}
    assert lava_hound_delta["parent_reference"] == {"file": WITCH_DELTA.name, "reference_id": witch_delta["reference_id"], "sha256": hashlib.sha256(witch_bytes).hexdigest()}
    assert headhunter_delta["parent_reference"] == {"file": LAVA_HOUND_DELTA.name, "reference_id": lava_hound_delta["reference_id"], "sha256": hashlib.sha256(lava_hound_bytes).hexdigest()}
    assert druid_delta["parent_reference"] == {"file": HEADHUNTER_DELTA.name, "reference_id": headhunter_delta["reference_id"], "sha256": hashlib.sha256(headhunter_bytes).hexdigest()}
    assert furnace_delta["parent_reference"] == {"file": DRUID_DELTA.name, "reference_id": druid_delta["reference_id"], "sha256": hashlib.sha256(druid_bytes).hexdigest()}
    assert meteor_golem_delta["parent_reference"] == {"file": FURNACE_DELTA.name, "reference_id": furnace_delta["reference_id"], "sha256": hashlib.sha256(furnace_bytes).hexdigest()}
    assert ruin_witch_knight_delta["parent_reference"] == {"file": METEOR_GOLEM_DELTA.name, "reference_id": meteor_golem_delta["reference_id"], "sha256": hashlib.sha256(meteor_golem_bytes).hexdigest()}
    assert baby_dragon_delta["parent_reference"] == {"file": RUIN_WITCH_KNIGHT_DELTA.name, "reference_id": ruin_witch_knight_delta["reference_id"], "sha256": hashlib.sha256(ruin_witch_knight_bytes).hexdigest()}
    assert pekka_delta["parent_reference"] == {"file": BABY_DRAGON_DELTA.name, "reference_id": baby_dragon_delta["reference_id"], "sha256": hashlib.sha256(baby_dragon_bytes).hexdigest()}
    assert goblin_delta["parent_reference"] == {"file": PEKKA_DELTA.name, "reference_id": pekka_delta["reference_id"], "sha256": hashlib.sha256(pekka_bytes).hexdigest()}
    assert minion_delta["parent_reference"] == {"file": GOBLIN_DELTA.name, "reference_id": goblin_delta["reference_id"], "sha256": hashlib.sha256(goblin_bytes).hexdigest()}
    assert hog_rider_delta["parent_reference"] == {"file": MINION_DELTA.name, "reference_id": minion_delta["reference_id"], "sha256": hashlib.sha256(minion_bytes).hexdigest()}
    assert valkyrie_delta["parent_reference"] == {"file": HOG_RIDER_DELTA.name, "reference_id": hog_rider_delta["reference_id"], "sha256": hashlib.sha256(hog_rider_bytes).hexdigest()}
    assert dragon_rider_delta["parent_reference"] == {"file": VALKYRIE_DELTA.name, "reference_id": valkyrie_delta["reference_id"], "sha256": hashlib.sha256(valkyrie_bytes).hexdigest()}
    assert miner_delta["parent_reference"] == {"file": DRAGON_RIDER_DELTA.name, "reference_id": dragon_rider_delta["reference_id"], "sha256": hashlib.sha256(dragon_rider_bytes).hexdigest()}
    assert electro_dragon_current_delta["parent_reference"] == {"file": MINER_DELTA.name, "reference_id": miner_delta["reference_id"], "sha256": hashlib.sha256(miner_bytes).hexdigest()}
    assert ruin_witch_knight_current_delta["parent_reference"] == {"file": ELECTRO_DRAGON_CURRENT_DELTA.name, "reference_id": electro_dragon_current_delta["reference_id"], "sha256": hashlib.sha256(electro_dragon_current_bytes).hexdigest()}
    assert dragon_delta["parent_reference"] == {"file": RUIN_WITCH_KNIGHT_CURRENT_DELTA.name, "reference_id": ruin_witch_knight_current_delta["reference_id"], "sha256": hashlib.sha256(ruin_witch_knight_current_bytes).hexdigest()}
    assert balloon_delta["parent_reference"] == {"file": DRAGON_DELTA.name, "reference_id": dragon_delta["reference_id"], "sha256": hashlib.sha256(dragon_bytes).hexdigest()}
    assert golem_delta["parent_reference"] == {"file": BALLOON_DELTA.name, "reference_id": balloon_delta["reference_id"], "sha256": hashlib.sha256(balloon_bytes).hexdigest()}
    assert wizard_delta["parent_reference"] == {"file": GOLEM_DELTA.name, "reference_id": golem_delta["reference_id"], "sha256": hashlib.sha256(golem_bytes).hexdigest()}
    assert giant_delta["parent_reference"] == {"file": WIZARD_DELTA.name, "reference_id": wizard_delta["reference_id"], "sha256": hashlib.sha256(wizard_bytes).hexdigest()}
    assert archer_delta["parent_reference"] == {"file": GIANT_DELTA.name, "reference_id": giant_delta["reference_id"], "sha256": hashlib.sha256(giant_bytes).hexdigest()}
    assert barbarian_delta["parent_reference"] == {"file": ARCHER_DELTA.name, "reference_id": archer_delta["reference_id"], "sha256": hashlib.sha256(archer_bytes).hexdigest()}
    assert healer_delta["parent_reference"] == {"file": BARBARIAN_DELTA.name, "reference_id": barbarian_delta["reference_id"], "sha256": hashlib.sha256(barbarian_bytes).hexdigest()}
    assert wall_breaker_delta["parent_reference"] == {"file": HEALER_DELTA.name, "reference_id": healer_delta["reference_id"], "sha256": hashlib.sha256(healer_bytes).hexdigest()}
    assert electro_titan_delta["parent_reference"] == {"file": WALL_BREAKER_DELTA.name, "reference_id": wall_breaker_delta["reference_id"], "sha256": hashlib.sha256(wall_breaker_bytes).hexdigest()}
    assert thrower_delta["parent_reference"] == {"file": ELECTRO_TITAN_DELTA.name, "reference_id": electro_titan_delta["reference_id"], "sha256": hashlib.sha256(electro_titan_bytes).hexdigest()}
    assert root_rider_delta["parent_reference"] == {"file": THROWER_DELTA.name, "reference_id": thrower_delta["reference_id"], "sha256": hashlib.sha256(thrower_bytes).hexdigest()}
    assert sneaky_goblin_delta["parent_reference"] == {"file": ROOT_RIDER_DELTA.name, "reference_id": root_rider_delta["reference_id"], "sha256": hashlib.sha256(root_rider_bytes).hexdigest()}
    assert air_sweeper_current_delta["parent_reference"] == {"file": SNEAKY_GOBLIN_DELTA.name, "reference_id": sneaky_goblin_delta["reference_id"], "sha256": hashlib.sha256(sneaky_goblin_bytes).hexdigest()}
    assert spell_tower_current_delta["parent_reference"] == {"file": AIR_SWEEPER_CURRENT_DELTA.name, "reference_id": air_sweeper_current_delta["reference_id"], "sha256": hashlib.sha256(air_sweeper_current_bytes).hexdigest()}
    assert firespitter_current_delta["parent_reference"] == {"file": SPELL_TOWER_CURRENT_DELTA.name, "reference_id": spell_tower_current_delta["reference_id"], "sha256": hashlib.sha256(spell_tower_current_bytes).hexdigest()}
    assert builders_hut_current_delta["parent_reference"] == {"file": FIRESPITTER_CURRENT_DELTA.name, "reference_id": firespitter_current_delta["reference_id"], "sha256": hashlib.sha256(firespitter_current_bytes).hexdigest()}
    assert ricochet_cannon_delta["parent_reference"] == {"file": BUILDERS_HUT_CURRENT_DELTA.name, "reference_id": builders_hut_current_delta["reference_id"], "sha256": hashlib.sha256(builders_hut_current_bytes).hexdigest()}
    assert super_wizard_tower_delta["parent_reference"] == {"file": RICOCHET_CANNON_DELTA.name, "reference_id": ricochet_cannon_delta["reference_id"], "sha256": hashlib.sha256(ricochet_cannon_bytes).hexdigest()}
    assert revenge_tower_current_delta["parent_reference"] == {"file": SUPER_WIZARD_TOWER_DELTA.name, "reference_id": super_wizard_tower_delta["reference_id"], "sha256": hashlib.sha256(super_wizard_tower_bytes).hexdigest()}
    assert multi_archer_tower_delta["parent_reference"] == {"file": REVENGE_TOWER_CURRENT_DELTA.name, "reference_id": revenge_tower_current_delta["reference_id"], "sha256": hashlib.sha256(revenge_tower_current_bytes).hexdigest()}
    assert multi_gear_tower_delta["parent_reference"] == {"file": MULTI_ARCHER_TOWER_DELTA.name, "reference_id": multi_archer_tower_delta["reference_id"], "sha256": hashlib.sha256(multi_archer_tower_bytes).hexdigest()}
    assert scattershot_delta["parent_reference"] == {"file": MULTI_GEAR_TOWER_DELTA.name, "reference_id": multi_gear_tower_delta["reference_id"], "sha256": hashlib.sha256(multi_gear_tower_bytes).hexdigest()}
    assert eagle_artillery_scope_delta["parent_reference"] == {"file": SCATTERSHOT_DELTA.name, "reference_id": scattershot_delta["reference_id"], "sha256": hashlib.sha256(scattershot_bytes).hexdigest()}
    assert town_hall_passive_scope_delta["parent_reference"] == {"file": EAGLE_ARTILLERY_SCOPE_DELTA.name, "reference_id": eagle_artillery_scope_delta["reference_id"], "sha256": hashlib.sha256(eagle_artillery_scope_bytes).hexdigest()}
    assert bomb_tower_delta["parent_reference"] == {"file": TOWN_HALL_PASSIVE_SCOPE_DELTA.name, "reference_id": town_hall_passive_scope_delta["reference_id"], "sha256": hashlib.sha256(town_hall_passive_scope_bytes).hexdigest()}
    assert x_bow_delta["parent_reference"] == {"file": BOMB_TOWER_DELTA.name, "reference_id": bomb_tower_delta["reference_id"], "sha256": hashlib.sha256(bomb_tower_bytes).hexdigest()}
    assert inferno_tower_delta["parent_reference"] == {"file": X_BOW_DELTA.name, "reference_id": x_bow_delta["reference_id"], "sha256": hashlib.sha256(x_bow_bytes).hexdigest()}
    assert ruin_witch_knight_followup_delta["parent_reference"] == {"file": INFERNO_TOWER_DELTA.name, "reference_id": inferno_tower_delta["reference_id"], "sha256": hashlib.sha256(inferno_tower_bytes).hexdigest()}
    assert furnace_firemite_followup_delta["parent_reference"] == {"file": RUIN_WITCH_KNIGHT_FOLLOWUP_DELTA.name, "reference_id": ruin_witch_knight_followup_delta["reference_id"], "sha256": hashlib.sha256(ruin_witch_knight_followup_bytes).hexdigest()}
    assert super_barbarian_delta["game_version"] == (
        "The immutable level payload is source commit 62b019df868f9decb49e97a0ff5ae15fb27af9ba. "
        "Neither the official launch note nor the community modifier table identifies a current game/client build; "
        "that version remains explicitly unknown rather than inferred."
    )
    super_barbarian_sources = {source["id"]: source for source in super_barbarian_delta["sources"]}
    assert super_barbarian_sources["supercell-2020-03-30-super-barbarian-rage"]["version"] == "not published by the official launch note"
    assert super_barbarian_sources["fandom-2026-09-18-super-barbarian"]["version"] == "not published by the community table"
    assert meteor_golem_meteormite_followup_delta["parent_reference"] == {"file": FURNACE_FIREMITE_FOLLOWUP_DELTA.name, "reference_id": furnace_firemite_followup_delta["reference_id"], "sha256": hashlib.sha256(furnace_firemite_followup_bytes).hexdigest()}
    assert revenge_tower_followup_delta["parent_reference"] == {"file": METEOR_GOLEM_METEORMITE_FOLLOWUP_DELTA.name, "reference_id": meteor_golem_meteormite_followup_delta["reference_id"], "sha256": hashlib.sha256(meteor_golem_meteormite_followup_bytes).hexdigest()}
    assert crafting_station_passive_delta["parent_reference"] == {"file": REVENGE_TOWER_FOLLOWUP_DELTA.name, "reference_id": revenge_tower_followup_delta["reference_id"], "sha256": hashlib.sha256(revenge_tower_followup_bytes).hexdigest()}
    assert electromite_followup_delta["parent_reference"] == {"file": CRAFTING_STATION_PASSIVE_DELTA.name, "reference_id": crafting_station_passive_delta["reference_id"], "sha256": hashlib.sha256(crafting_station_passive_bytes).hexdigest()}
    assert big_boy_followup_delta["parent_reference"] == {"file": ELECTROMITE_FOLLOWUP_DELTA.name, "reference_id": electromite_followup_delta["reference_id"], "sha256": hashlib.sha256(electromite_followup_bytes).hexdigest()}
    assert lava_hound_lava_pup_delta["parent_reference"] == {"file": BIG_BOY_FOLLOWUP_DELTA.name, "reference_id": big_boy_followup_delta["reference_id"], "sha256": hashlib.sha256(big_boy_followup_bytes).hexdigest()}
    assert ruin_witch_movement_delta["parent_reference"] == {"file": LAVA_HOUND_LAVA_PUP_DELTA.name, "reference_id": lava_hound_lava_pup_delta["reference_id"], "sha256": hashlib.sha256(lava_hound_lava_pup_bytes).hexdigest()}
    assert air_sweeper_recheck_delta["parent_reference"] == {"file": RUIN_WITCH_MOVEMENT_DELTA.name, "reference_id": ruin_witch_movement_delta["reference_id"], "sha256": hashlib.sha256(ruin_witch_movement_bytes).hexdigest()}
    assert builders_hut_multi_builder_delta["parent_reference"] == {"file": AIR_SWEEPER_RECHECK_DELTA.name, "reference_id": air_sweeper_recheck_delta["reference_id"], "sha256": hashlib.sha256(air_sweeper_recheck_bytes).hexdigest()}
    assert furnace_firemite_lifecycle_recheck_delta["parent_reference"] == {"file": BUILDERS_HUT_MULTI_BUILDER_DELTA.name, "reference_id": builders_hut_multi_builder_delta["reference_id"], "sha256": hashlib.sha256(builders_hut_multi_builder_bytes).hexdigest()}
    assert furnace_firemite_lifecycle_recheck_delta["mechanics_evidence"]["published_lifecycle"]["firemite_caps_by_furnace_level"] == [19, 20, 21, 22]
    assert spawned_subunit_assets_recheck_delta["parent_reference"] == {"file": FURNACE_FIREMITE_LIFECYCLE_RECHECK_DELTA.name, "reference_id": furnace_firemite_lifecycle_recheck_delta["reference_id"], "sha256": hashlib.sha256(furnace_firemite_lifecycle_recheck_bytes).hexdigest()}
    assert set(spawned_subunit_assets_recheck_delta["asset_evidence"]["public_visual_tiers_only"]) == {"druid_bear", "lava_pup", "yetimite", "super_rider"}
    assert grand_warden_delta["parent_reference"]["file"] == "th18-2026-09-20-archer-queen-baseline.json"
    assert grand_warden_delta["parent_reference"]["reference_id"] == "home-village-th18-2026-09-20-archer-queen-baseline"
    assert len(grand_warden_delta["parent_reference"]["sha256"]) == 64
    assert royal_champion_delta["parent_reference"] == {"file": GRAND_WARDEN_DELTA.name, "reference_id": grand_warden_delta["reference_id"], "sha256": hashlib.sha256(grand_warden_bytes).hexdigest()}
    assert minion_prince_delta["parent_reference"] == {"file": ROYAL_CHAMPION_DELTA.name, "reference_id": royal_champion_delta["reference_id"], "sha256": hashlib.sha256(royal_champion_bytes).hexdigest()}
    assert dragon_duke_delta["parent_reference"] == {"file": MINION_PRINCE_DELTA.name, "reference_id": minion_prince_delta["reference_id"], "sha256": hashlib.sha256(minion_prince_bytes).hexdigest()}
    assert dragon_duke_base_attack_delta["parent_reference"] == {"file": DRAGON_DUKE_DELTA.name, "reference_id": dragon_duke_delta["reference_id"], "sha256": hashlib.sha256(dragon_duke_bytes).hexdigest()}
    assert royal_champion_wall_crossing_delta["parent_reference"] == {"file": DRAGON_DUKE_BASE_ATTACK_DELTA.name, "reference_id": dragon_duke_base_attack_delta["reference_id"], "sha256": hashlib.sha256(dragon_duke_base_attack_bytes).hexdigest()}
    assert hero_scope_audit_delta["parent_reference"] == {"file": ROYAL_CHAMPION_WALL_CROSSING_DELTA.name, "reference_id": royal_champion_wall_crossing_delta["reference_id"], "sha256": hashlib.sha256(royal_champion_wall_crossing_bytes).hexdigest()}
    assert hero_visual_icons_delta["parent_reference"] == {"file": HERO_SCOPE_AUDIT_DELTA.name, "reference_id": hero_scope_audit_delta["reference_id"], "sha256": hashlib.sha256(hero_scope_audit_bytes).hexdigest()}
    assert hero_banner_defense_recheck_delta["parent_reference"] == {"file": HERO_VISUAL_ICONS_DELTA.name, "reference_id": hero_visual_icons_delta["reference_id"], "sha256": hashlib.sha256(hero_visual_icons_bytes).hexdigest()}
    assert flying_hero_defense_targetability_delta["parent_reference"] == {"file": HERO_BANNER_DEFENSE_RECHECK_DELTA.name, "reference_id": hero_banner_defense_recheck_delta["reference_id"], "sha256": hashlib.sha256(hero_banner_defense_recheck_bytes).hexdigest()}
    assert grand_warden_air_mode_delta["parent_reference"] == {"file": FLYING_HERO_DEFENSE_TARGETABILITY_DELTA.name, "reference_id": flying_hero_defense_targetability_delta["reference_id"], "sha256": hashlib.sha256(flying_hero_defense_targetability_bytes).hexdigest()}
    assert hero_banner_visual_variants_delta["parent_reference"] == {"file": GRAND_WARDEN_AIR_MODE_DELTA.name, "reference_id": grand_warden_air_mode_delta["reference_id"], "sha256": hashlib.sha256(grand_warden_air_mode_bytes).hexdigest()}
    assert hero_banner_assignment_official_delta["parent_reference"] == {"file": HERO_BANNER_VISUAL_VARIANTS_DELTA.name, "reference_id": hero_banner_visual_variants_delta["reference_id"], "sha256": hashlib.sha256(hero_banner_visual_variants_bytes).hexdigest()}
    assert hero_banner_th18_quantity_delta["parent_reference"] == {"file": HERO_BANNER_ASSIGNMENT_OFFICIAL_DELTA.name, "reference_id": hero_banner_assignment_official_delta["reference_id"], "sha256": hashlib.sha256(hero_banner_assignment_official_bytes).hexdigest()}
    assert attacking_hero_slots_delta["parent_reference"] == {"file": HERO_BANNER_TH18_QUANTITY_DELTA.name, "reference_id": hero_banner_th18_quantity_delta["reference_id"], "sha256": hashlib.sha256(hero_banner_th18_quantity_bytes).hexdigest()}
    assert hero_battlefield_asset_audit_delta["parent_reference"] == {"file": ATTACKING_HERO_SLOTS_DELTA.name, "reference_id": attacking_hero_slots_delta["reference_id"], "sha256": hashlib.sha256(attacking_hero_slots_bytes).hexdigest()}
    assert hero_asset_rights_policy_delta["parent_reference"] == {"file": HERO_BATTLEFIELD_ASSET_AUDIT_DELTA.name, "reference_id": hero_battlefield_asset_audit_delta["reference_id"], "sha256": hashlib.sha256(hero_battlefield_asset_audit_bytes).hexdigest()}
    assert hero_projectile_contract_audit_delta["parent_reference"] == {"file": HERO_ASSET_RIGHTS_POLICY_DELTA.name, "reference_id": hero_asset_rights_policy_delta["reference_id"], "sha256": hashlib.sha256(hero_asset_rights_policy_bytes).hexdigest()}
    assert dragon_duke_trap_mitigation_delta["parent_reference"] == {"file": HERO_PROJECTILE_CONTRACT_AUDIT_DELTA.name, "reference_id": hero_projectile_contract_audit_delta["reference_id"], "sha256": hashlib.sha256(hero_projectile_contract_audit_bytes).hexdigest()}
    assert hero_friendly_challenge_modifier_scope_delta["parent_reference"] == {"file": DRAGON_DUKE_TRAP_MITIGATION_DELTA.name, "reference_id": dragon_duke_trap_mitigation_delta["reference_id"], "sha256": hashlib.sha256(dragon_duke_trap_mitigation_bytes).hexdigest()}
    assert hero_banner_defense_behavior_audit_delta["parent_reference"] == {"file": HERO_FRIENDLY_CHALLENGE_MODIFIER_SCOPE_DELTA.name, "reference_id": hero_friendly_challenge_modifier_scope_delta["reference_id"], "sha256": hashlib.sha256(hero_friendly_challenge_modifier_scope_bytes).hexdigest()}
    assert hero_collision_geometry_audit_delta["parent_reference"] == {"file": HERO_BANNER_DEFENSE_BEHAVIOR_AUDIT_DELTA.name, "reference_id": hero_banner_defense_behavior_audit_delta["reference_id"], "sha256": hashlib.sha256(hero_banner_defense_behavior_audit_bytes).hexdigest()}
    assert hero_defense_evidence_closure_delta["parent_reference"] == {"file": HERO_COLLISION_GEOMETRY_AUDIT_DELTA.name, "reference_id": hero_collision_geometry_audit_delta["reference_id"], "sha256": hashlib.sha256(hero_collision_geometry_audit_bytes).hexdigest()}
    assert meteor_golem_meteormite_evidence_recheck_delta["parent_reference"] == {"file": HERO_DEFENSE_EVIDENCE_CLOSURE_DELTA.name, "reference_id": hero_defense_evidence_closure_delta["reference_id"], "sha256": hashlib.sha256(hero_defense_evidence_closure_bytes).hexdigest()}
    assert hero_pet_equipment_scope_delta["parent_reference"] == {"file": METEOR_GOLEM_METEORMITE_EVIDENCE_RECHECK_DELTA.name, "reference_id": meteor_golem_meteormite_evidence_recheck_delta["reference_id"], "sha256": hashlib.sha256(meteor_golem_meteormite_evidence_recheck_bytes).hexdigest()}
    assert lassi_combat_contract_audit_delta["parent_reference"] == {"file": HERO_PET_EQUIPMENT_SCOPE_DELTA.name, "reference_id": hero_pet_equipment_scope_delta["reference_id"], "sha256": hashlib.sha256(hero_pet_equipment_scope_bytes).hexdigest()}
    assert electro_owl_audit_delta["parent_reference"] == {"file": LASSI_COMBAT_CONTRACT_AUDIT_DELTA.name, "reference_id": lassi_combat_contract_audit_delta["reference_id"], "sha256": hashlib.sha256(lassi_combat_contract_audit_bytes).hexdigest()}
    assert mighty_yak_audit_delta["parent_reference"] == {"file": ELECTRO_OWL_AUDIT_DELTA.name, "reference_id": electro_owl_audit_delta["reference_id"], "sha256": hashlib.sha256(electro_owl_audit_bytes).hexdigest()}
    assert unicorn_audit_delta["parent_reference"] == {"file": MIGHTY_YAK_AUDIT_DELTA.name, "reference_id": mighty_yak_audit_delta["reference_id"], "sha256": hashlib.sha256(mighty_yak_audit_bytes).hexdigest()}
    assert frosty_audit_delta["parent_reference"] == {"file": UNICORN_AUDIT_DELTA.name, "reference_id": unicorn_audit_delta["reference_id"], "sha256": hashlib.sha256(unicorn_audit_bytes).hexdigest()}
    assert diggy_audit_delta["parent_reference"] == {"file": FROSTY_AUDIT_DELTA.name, "reference_id": frosty_audit_delta["reference_id"], "sha256": hashlib.sha256(frosty_audit_bytes).hexdigest()}
    assert poison_lizard_audit_delta["parent_reference"] == {"file": DIGGY_AUDIT_DELTA.name, "reference_id": diggy_audit_delta["reference_id"], "sha256": hashlib.sha256(diggy_audit_bytes).hexdigest()}
    assert phoenix_audit_delta["parent_reference"] == {"file": POISON_LIZARD_AUDIT_DELTA.name, "reference_id": poison_lizard_audit_delta["reference_id"], "sha256": hashlib.sha256(poison_lizard_audit_bytes).hexdigest()}
    assert spirit_fox_audit_delta["parent_reference"] == {"file": PHOENIX_AUDIT_DELTA.name, "reference_id": phoenix_audit_delta["reference_id"], "sha256": hashlib.sha256(phoenix_audit_bytes).hexdigest()}
    assert angry_jelly_audit_delta["parent_reference"] == {"file": SPIRIT_FOX_AUDIT_DELTA.name, "reference_id": spirit_fox_audit_delta["reference_id"], "sha256": hashlib.sha256(spirit_fox_audit_bytes).hexdigest()}
    assert sneezy_audit_delta["parent_reference"] == {"file": ANGRY_JELLY_AUDIT_DELTA.name, "reference_id": angry_jelly_audit_delta["reference_id"], "sha256": hashlib.sha256(angry_jelly_audit_bytes).hexdigest()}
    assert greedy_raven_audit_delta["parent_reference"] == {"file": SNEEZY_AUDIT_DELTA.name, "reference_id": sneezy_audit_delta["reference_id"], "sha256": hashlib.sha256(sneezy_audit_bytes).hexdigest()}
    assert revenge_deck_audit_delta["parent_reference"] == {"file": GREEDY_RAVEN_AUDIT_DELTA.name, "reference_id": greedy_raven_audit_delta["reference_id"], "sha256": hashlib.sha256(greedy_raven_audit_bytes).hexdigest()}
    assert barbarian_puppet_audit_delta["parent_reference"] == {"file": REVENGE_DECK_AUDIT_DELTA.name, "reference_id": revenge_deck_audit_delta["reference_id"], "sha256": hashlib.sha256(revenge_deck_audit_bytes).hexdigest()}
    assert rage_vial_audit_delta["parent_reference"] == {"file": BARBARIAN_PUPPET_AUDIT_DELTA.name, "reference_id": barbarian_puppet_audit_delta["reference_id"], "sha256": hashlib.sha256(barbarian_puppet_audit_bytes).hexdigest()}
    assert earthquake_boots_audit_delta["parent_reference"] == {"file": RAGE_VIAL_AUDIT_DELTA.name, "reference_id": rage_vial_audit_delta["reference_id"], "sha256": hashlib.sha256(rage_vial_audit_bytes).hexdigest()}
    assert vampstache_audit_delta["parent_reference"] == {"file": EARTHQUAKE_BOOTS_AUDIT_DELTA.name, "reference_id": earthquake_boots_audit_delta["reference_id"], "sha256": hashlib.sha256(earthquake_boots_audit_bytes).hexdigest()}
    assert giant_gauntlet_audit_delta["parent_reference"] == {"file": VAMPSTACHE_AUDIT_DELTA.name, "reference_id": vampstache_audit_delta["reference_id"], "sha256": hashlib.sha256(vampstache_audit_bytes).hexdigest()}
    assert spiky_ball_audit_delta["parent_reference"] == {"file": GIANT_GAUNTLET_AUDIT_DELTA.name, "reference_id": giant_gauntlet_audit_delta["reference_id"], "sha256": hashlib.sha256(giant_gauntlet_audit_bytes).hexdigest()}
    assert snake_bracelet_audit_delta["parent_reference"] == {"file": SPIKY_BALL_AUDIT_DELTA.name, "reference_id": spiky_ball_audit_delta["reference_id"], "sha256": hashlib.sha256(spiky_ball_audit_bytes).hexdigest()}
    assert stick_horse_audit_delta["parent_reference"] == {"file": SNAKE_BRACELET_AUDIT_DELTA.name, "reference_id": snake_bracelet_audit_delta["reference_id"], "sha256": hashlib.sha256(snake_bracelet_audit_bytes).hexdigest()}
    assert archer_puppet_audit_delta["parent_reference"] == {"file": STICK_HORSE_AUDIT_DELTA.name, "reference_id": stick_horse_audit_delta["reference_id"], "sha256": hashlib.sha256(stick_horse_audit_bytes).hexdigest()}
    assert invisibility_vial_audit_delta["parent_reference"] == {"file": ARCHER_PUPPET_AUDIT_DELTA.name, "reference_id": archer_puppet_audit_delta["reference_id"], "sha256": hashlib.sha256(archer_puppet_audit_bytes).hexdigest()}
    assert giant_arrow_audit_delta["parent_reference"] == {"file": INVISIBILITY_VIAL_AUDIT_DELTA.name, "reference_id": invisibility_vial_audit_delta["reference_id"], "sha256": hashlib.sha256(invisibility_vial_audit_bytes).hexdigest()}
    assert healer_puppet_audit_delta["parent_reference"] == {"file": GIANT_ARROW_AUDIT_DELTA.name, "reference_id": giant_arrow_audit_delta["reference_id"], "sha256": hashlib.sha256(giant_arrow_audit_bytes).hexdigest()}
    assert frozen_arrow_audit_delta["parent_reference"] == {"file": HEALER_PUPPET_AUDIT_DELTA.name, "reference_id": healer_puppet_audit_delta["reference_id"], "sha256": hashlib.sha256(healer_puppet_audit_bytes).hexdigest()}
    assert magic_mirror_audit_delta["parent_reference"] == {"file": FROZEN_ARROW_AUDIT_DELTA.name, "reference_id": frozen_arrow_audit_delta["reference_id"], "sha256": hashlib.sha256(frozen_arrow_audit_bytes).hexdigest()}
    assert action_figure_audit_delta["parent_reference"] == {"file": MAGIC_MIRROR_AUDIT_DELTA.name, "reference_id": magic_mirror_audit_delta["reference_id"], "sha256": hashlib.sha256(magic_mirror_audit_bytes).hexdigest()}
    assert monolith_arrow_audit_delta["parent_reference"] == {"file": ACTION_FIGURE_AUDIT_DELTA.name, "reference_id": action_figure_audit_delta["reference_id"], "sha256": hashlib.sha256(action_figure_audit_bytes).hexdigest()}
    assert henchmen_puppet_audit_delta["parent_reference"] == {"file": MONOLITH_ARROW_AUDIT_DELTA.name, "reference_id": monolith_arrow_audit_delta["reference_id"], "sha256": hashlib.sha256(monolith_arrow_audit_bytes).hexdigest()}
    assert dark_orb_audit_delta["parent_reference"] == {"file": HENCHMEN_PUPPET_AUDIT_DELTA.name, "reference_id": henchmen_puppet_audit_delta["reference_id"], "sha256": hashlib.sha256(henchmen_puppet_audit_bytes).hexdigest()}
    assert metal_pants_audit_delta["parent_reference"] == {"file": DARK_ORB_AUDIT_DELTA.name, "reference_id": dark_orb_audit_delta["reference_id"], "sha256": hashlib.sha256(dark_orb_audit_bytes).hexdigest()}
    assert noble_iron_audit_delta["parent_reference"] == {"file": METAL_PANTS_AUDIT_DELTA.name, "reference_id": metal_pants_audit_delta["reference_id"], "sha256": hashlib.sha256(metal_pants_audit_bytes).hexdigest()}
    assert dark_crown_audit_delta["parent_reference"] == {"file": NOBLE_IRON_AUDIT_DELTA.name, "reference_id": noble_iron_audit_delta["reference_id"], "sha256": hashlib.sha256(noble_iron_audit_bytes).hexdigest()}
    assert meteor_staff_audit_delta["parent_reference"] == {"file": DARK_CROWN_AUDIT_DELTA.name, "reference_id": dark_crown_audit_delta["reference_id"], "sha256": hashlib.sha256(dark_crown_audit_bytes).hexdigest()}
    assert eternal_tome_audit_delta["parent_reference"] == {"file": METEOR_STAFF_AUDIT_DELTA.name, "reference_id": meteor_staff_audit_delta["reference_id"], "sha256": hashlib.sha256(meteor_staff_audit_bytes).hexdigest()}
    assert life_gem_audit_delta["parent_reference"] == {"file": ETERNAL_TOME_AUDIT_DELTA.name, "reference_id": eternal_tome_audit_delta["reference_id"], "sha256": hashlib.sha256(eternal_tome_audit_bytes).hexdigest()}
    assert rage_gem_audit_delta["parent_reference"] == {"file": LIFE_GEM_AUDIT_DELTA.name, "reference_id": life_gem_audit_delta["reference_id"], "sha256": hashlib.sha256(life_gem_audit_bytes).hexdigest()}
    assert healing_tome_audit_delta["parent_reference"] == {"file": RAGE_GEM_AUDIT_DELTA.name, "reference_id": rage_gem_audit_delta["reference_id"], "sha256": hashlib.sha256(rage_gem_audit_bytes).hexdigest()}
    assert fireball_audit_delta["parent_reference"] == {"file": HEALING_TOME_AUDIT_DELTA.name, "reference_id": healing_tome_audit_delta["reference_id"], "sha256": hashlib.sha256(healing_tome_audit_bytes).hexdigest()}
    assert lavaloon_puppet_audit_delta["parent_reference"] == {"file": FIREBALL_AUDIT_DELTA.name, "reference_id": fireball_audit_delta["reference_id"], "sha256": hashlib.sha256(fireball_audit_bytes).hexdigest()}
    assert heroic_torch_audit_delta["parent_reference"] == {"file": LAVALOON_PUPPET_AUDIT_DELTA.name, "reference_id": lavaloon_puppet_audit_delta["reference_id"], "sha256": hashlib.sha256(lavaloon_puppet_audit_bytes).hexdigest()}
    assert royal_gem_audit_delta["parent_reference"] == {"file": HEROIC_TORCH_AUDIT_DELTA.name, "reference_id": heroic_torch_audit_delta["reference_id"], "sha256": hashlib.sha256(heroic_torch_audit_bytes).hexdigest()}
    assert seeking_shield_audit_delta["parent_reference"] == {"file": ROYAL_GEM_AUDIT_DELTA.name, "reference_id": royal_gem_audit_delta["reference_id"], "sha256": hashlib.sha256(royal_gem_audit_bytes).hexdigest()}
    assert hog_rider_puppet_audit_delta["parent_reference"] == {"file": SEEKING_SHIELD_AUDIT_DELTA.name, "reference_id": seeking_shield_audit_delta["reference_id"], "sha256": hashlib.sha256(seeking_shield_audit_bytes).hexdigest()}
    assert haste_vial_audit_delta["parent_reference"] == {"file": HOG_RIDER_PUPPET_AUDIT_DELTA.name, "reference_id": hog_rider_puppet_audit_delta["reference_id"], "sha256": hashlib.sha256(hog_rider_puppet_audit_bytes).hexdigest()}
    assert rocket_spear_audit_delta["parent_reference"] == {"file": HASTE_VIAL_AUDIT_DELTA.name, "reference_id": haste_vial_audit_delta["reference_id"], "sha256": hashlib.sha256(haste_vial_audit_bytes).hexdigest()}
    assert electro_boots_audit_delta["parent_reference"] == {"file": ROCKET_SPEAR_AUDIT_DELTA.name, "reference_id": rocket_spear_audit_delta["reference_id"], "sha256": hashlib.sha256(rocket_spear_audit_bytes).hexdigest()}
    assert frost_flake_audit_delta["parent_reference"] == {"file": ELECTRO_BOOTS_AUDIT_DELTA.name, "reference_id": electro_boots_audit_delta["reference_id"], "sha256": hashlib.sha256(electro_boots_audit_bytes).hexdigest()}
    assert fire_heart_audit_delta["parent_reference"] == {"file": FROST_FLAKE_AUDIT_DELTA.name, "reference_id": frost_flake_audit_delta["reference_id"], "sha256": hashlib.sha256(frost_flake_audit_bytes).hexdigest()}
    assert flame_blower_audit_delta["parent_reference"] == {"file": FIRE_HEART_AUDIT_DELTA.name, "reference_id": fire_heart_audit_delta["reference_id"], "sha256": hashlib.sha256(fire_heart_audit_bytes).hexdigest()}
    assert stun_blaster_audit_delta["parent_reference"] == {"file": FLAME_BLOWER_AUDIT_DELTA.name, "reference_id": flame_blower_audit_delta["reference_id"], "sha256": hashlib.sha256(flame_blower_audit_bytes).hexdigest()}
    assert electro_fangs_audit_delta["parent_reference"] == {"file": STUN_BLASTER_AUDIT_DELTA.name, "reference_id": stun_blaster_audit_delta["reference_id"], "sha256": hashlib.sha256(stun_blaster_audit_bytes).hexdigest()}
    assert rocket_backpack_audit_delta["parent_reference"] == {"file": ELECTRO_FANGS_AUDIT_DELTA.name, "reference_id": electro_fangs_audit_delta["reference_id"], "sha256": hashlib.sha256(electro_fangs_audit_bytes).hexdigest()}
    assert hero_attachment_audit_delta["parent_reference"] == {"file": ROCKET_BACKPACK_AUDIT_DELTA.name, "reference_id": rocket_backpack_audit_delta["reference_id"], "sha256": hashlib.sha256(rocket_backpack_audit_bytes).hexdigest()}
    assert electro_fangs_passive_delta["parent_reference"] == {"file": HERO_ATTACHMENT_AUDIT_DELTA.name, "reference_id": hero_attachment_audit_delta["reference_id"], "sha256": hashlib.sha256(hero_attachment_audit_bytes).hexdigest()}
    assert hero_battlefield_art_rights_audit_delta["parent_reference"] == {"file": ELECTRO_FANGS_PASSIVE_DELTA.name, "reference_id": electro_fangs_passive_delta["reference_id"], "sha256": hashlib.sha256(electro_fangs_passive_bytes).hexdigest()}
    assert royal_gem_official_level_bonus_delta["parent_reference"] == {"file": HERO_BATTLEFIELD_ART_RIGHTS_AUDIT_DELTA.name, "reference_id": hero_battlefield_art_rights_audit_delta["reference_id"], "sha256": hashlib.sha256(hero_battlefield_art_rights_audit_bytes).hexdigest()}
    assert earthquake_boots_official_radius_damage_delta["parent_reference"] == {"file": ROYAL_GEM_OFFICIAL_LEVEL_BONUS_DELTA.name, "reference_id": royal_gem_official_level_bonus_delta["reference_id"], "sha256": hashlib.sha256(royal_gem_official_level_bonus_bytes).hexdigest()}
    assert vampstache_official_dps_delta["parent_reference"] == {"file": EARTHQUAKE_BOOTS_OFFICIAL_RADIUS_DAMAGE_DELTA.name, "reference_id": earthquake_boots_official_radius_damage_delta["reference_id"], "sha256": hashlib.sha256(earthquake_boots_official_radius_damage_bytes).hexdigest()}
    assert barbarian_puppet_official_bonus_spawn_delta["parent_reference"] == {"file": VAMPSTACHE_OFFICIAL_DPS_DELTA.name, "reference_id": vampstache_official_dps_delta["reference_id"], "sha256": hashlib.sha256(vampstache_official_dps_bytes).hexdigest()}
    assert archer_puppet_official_bonus_invisibility_delta["parent_reference"] == {"file": BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN_DELTA.name, "reference_id": barbarian_puppet_official_bonus_spawn_delta["reference_id"], "sha256": hashlib.sha256(barbarian_puppet_official_bonus_spawn_bytes).hexdigest()}
    assert electro_boots_official_aura_healing_delta["parent_reference"] == {"file": ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY_DELTA.name, "reference_id": archer_puppet_official_bonus_invisibility_delta["reference_id"], "sha256": hashlib.sha256(archer_puppet_official_bonus_invisibility_bytes).hexdigest()}
    assert healing_tome_official_duration_delta["parent_reference"] == {"file": ELECTRO_BOOTS_OFFICIAL_AURA_HEALING_DELTA.name, "reference_id": electro_boots_official_aura_healing_delta["reference_id"], "sha256": hashlib.sha256(electro_boots_official_aura_healing_bytes).hexdigest()}
    assert healing_tome_primary_level_provenance_delta["parent_reference"] == {"file": HEALING_TOME_OFFICIAL_DURATION_DELTA.name, "reference_id": healing_tome_official_duration_delta["reference_id"], "sha256": hashlib.sha256(healing_tome_official_duration_bytes).hexdigest()}
    assert six_equipment_primary_level_provenance_delta["parent_reference"] == {"file": HEALING_TOME_PRIMARY_LEVEL_PROVENANCE_DELTA.name, "reference_id": healing_tome_primary_level_provenance_delta["reference_id"], "sha256": hashlib.sha256(healing_tome_primary_level_provenance_bytes).hexdigest()}
    assert royal_gem_active_heal_recheck_delta["parent_reference"] == {"file": SIX_EQUIPMENT_PRIMARY_LEVEL_PROVENANCE_DELTA.name, "reference_id": six_equipment_primary_level_provenance_delta["reference_id"], "sha256": hashlib.sha256(six_equipment_primary_level_provenance_bytes).hexdigest()}
    assert monolith_arrow_passive_type_delta["parent_reference"] == {"file": ROYAL_GEM_ACTIVE_HEAL_RECHECK_DELTA.name, "reference_id": royal_gem_active_heal_recheck_delta["reference_id"], "sha256": hashlib.sha256(royal_gem_active_heal_recheck_bytes).hexdigest()}
    assert FROZEN_REFERENCE["reference_id"] == monolith_arrow_passive_type_delta["reference_id"]
    monolith_class = monolith_arrow_passive_type_delta["catalogue_content_overrides"][0]
    assert monolith_class["content_id"] == "monolith_arrow"
    assert monolith_class["fields"]["equipment_activation_type"]["value"] == "Passive"
    assert monolith_class["fields"]["equipment_activation_type"]["source"] == "clash-ninja-2026-06-03-monolith-arrow-passive"
    assert [source["tier"] for source in monolith_arrow_passive_type_delta["sources"]] == ["primary", "secondary-version-unknown", "secondary-version-unknown"]
    assert monolith_arrow_passive_type_delta["open_uncertainties"] and monolith_arrow_passive_type_delta["rejected_inferences"]
    assert len(royal_gem_active_heal_recheck_delta["sources"]) == 5
    assert all(source["content_id"] == "royal_gem" and source["levels"] == "1-18"
               and source["url"] and source["title"] and source["consulted_on"]
               and source["confidence"] and source["limitations"]
               for source in royal_gem_active_heal_recheck_delta["sources"])
    assert royal_gem_active_heal_recheck_delta["open_uncertainties"]
    assert royal_gem_active_heal_recheck_delta["rejected_inferences"]
    assert len(hero_battlefield_art_rights_audit_delta["sources"]) == 3
    assert all(source["url"] and source["title"] and source["consulted_on"] and source["raw_sha256"] and source["content_ids"] and source["levels"] and source["mode"] and source["confidence"] for source in hero_battlefield_art_rights_audit_delta["sources"])
    assert [source["tier"] for source in hero_battlefield_art_rights_audit_delta["sources"]] == ["primary", "secondary", "secondary"]
    assert hero_battlefield_art_rights_audit_delta["local_context"]["sha256"] == hashlib.sha256((ROOT / hero_battlefield_art_rights_audit_delta["local_context"]["file"]).read_bytes()).hexdigest()
    assert hero_battlefield_art_rights_audit_delta["open_uncertainties"]
    official_bonus_sources = royal_gem_official_level_bonus_delta["sources"]
    assert [source["tier"] for source in official_bonus_sources] == ["primary", "secondary-dated-game-files"]
    assert all(source["url"] and source["title"] and source["consulted_on"] and source["raw_sha256"] and source["content_id"] == "royal_gem" and source["levels"] == "1-18" for source in official_bonus_sources)
    official_bonus = royal_gem_official_level_bonus_delta["observations"][0]
    bonus_catalogue = json.loads(CATALOGUE.read_text(encoding="utf-8"))
    official_bonus_rows = next(content for content in bonus_catalogue["contents"] if content["id"] == "royal_gem")["levels"]
    assert len(official_bonus_rows) == 18
    for index, row in enumerate(official_bonus_rows):
        assert row["level"] == index + 1
        assert row["source_fields"]["hitpointIncrease"] == official_bonus["official_post_buff_hp_bonus_by_level"][index]
        assert row["source_fields"]["stats"]["dpsIncrease"] == official_bonus["official_post_buff_dps_bonus_by_level"][index]
    boots_sources = earthquake_boots_official_radius_damage_delta["sources"]
    assert len(boots_sources) == 3 and all(source["tier"] == "primary" for source in boots_sources)
    assert all(source["url"] and source["title"] and source["published_on"] and source["consulted_on"] and source["content_id"] == "earthquake_boots" and source["levels"] and source["mode"] and source["confidence"] for source in boots_sources)
    assert all(source["raw_sha256"] for source in boots_sources)
    boots_rows = next(content for content in bonus_catalogue["contents"] if content["id"] == "earthquake_boots")
    assert len(boots_rows["levels"]) == 18
    assert boots_rows["source_fields"]["ability"]["abilityRadius"] == earthquake_boots_official_radius_damage_delta["observations"][0]["official_post_2024_radius_tiles"] == 8
    for index, row in enumerate(boots_rows["levels"][8:]):
        assert row["level"] == index + 9
        assert row["source_fields"]["stats"]["buildingDamagePercent"] == f'{earthquake_boots_official_radius_damage_delta["observations"][1]["official_post_2025_building_damage_percent_by_level"][index]}%'
    assert earthquake_boots_official_radius_damage_delta["open_uncertainties"] and earthquake_boots_official_radius_damage_delta["rejected_inferences"]
    vamp_sources = vampstache_official_dps_delta["sources"]
    assert [source["tier"] for source in vamp_sources] == ["primary", "secondary-dated-game-files"]
    assert all(source["url"] and source["title"] and source["consulted_on"] and source["raw_sha256"] and source["content_id"] == "vampstache" and source["levels"] == "1-18" and source["mode"] and source["confidence"] for source in vamp_sources)
    vamp_rows = next(content for content in bonus_catalogue["contents"] if content["id"] == "vampstache")["levels"]
    assert len(vamp_rows) == 18
    for index, row in enumerate(vamp_rows):
        assert row["level"] == index + 1
        assert row["source_fields"]["stats"]["dpsIncrease"] == vampstache_official_dps_delta["observations"][0]["official_post_buff_dps_bonus_by_level"][index]
    assert vampstache_official_dps_delta["open_uncertainties"] and vampstache_official_dps_delta["rejected_inferences"]
    puppet_sources = barbarian_puppet_official_bonus_spawn_delta["sources"]
    assert len(puppet_sources) == 2 and all(source["tier"] == "primary" and source["url"] and source["title"] and source["published_on"] and source["consulted_on"] and source["raw_sha256"] and source["content_id"] == "barbarian_puppet" and source["levels"] == "1-18" and source["mode"] and source["confidence"] for source in puppet_sources)
    puppet_rows = next(content for content in bonus_catalogue["contents"] if content["id"] == "barbarian_puppet")
    puppet_rage = barbarian_puppet_official_bonus_spawn_delta["observations"][0]
    puppet_bonus = barbarian_puppet_official_bonus_spawn_delta["observations"][1]
    assert len(puppet_rows["levels"]) == 18
    assert puppet_rows["source_fields"]["ability"]["barbarianRageDuration"] == puppet_rage["official_post_2024_rage_duration_seconds"] == 20
    assert puppet_rage["official_post_2024_spawn_units_per_batch"] == 5
    assert puppet_rage["official_post_2024_spawn_interval_ms"] == 500
    for index, row in enumerate(puppet_rows["levels"]):
        assert row["level"] == index + 1
        assert row["source_fields"]["hitpointIncrease"] == puppet_bonus["official_post_buff_hp_bonus_by_level"][index]
        assert row["source_fields"]["hpRecoveryIncrease"] == puppet_bonus["official_post_buff_hp_recovery_by_level"][index]
    assert barbarian_puppet_official_bonus_spawn_delta["open_uncertainties"] and barbarian_puppet_official_bonus_spawn_delta["rejected_inferences"]
    archer_puppet_sources = archer_puppet_official_bonus_invisibility_delta["sources"]
    assert len(archer_puppet_sources) == 2 and all(source["tier"] == "primary" and source["url"] and source["title"] and source["published_on"] and source["consulted_on"] and source["raw_sha256"] and source["content_id"] == "archer_puppet" and source["levels"] == "1-18" and source["mode"] and source["confidence"] for source in archer_puppet_sources)
    archer_puppet_rows = next(content for content in bonus_catalogue["contents"] if content["id"] == "archer_puppet")["levels"]
    archer_invisibility = archer_puppet_official_bonus_invisibility_delta["observations"][0]
    archer_bonus = archer_puppet_official_bonus_invisibility_delta["observations"][1]
    assert len(archer_puppet_rows) == 18
    assert archer_invisibility["official_post_2024_spawn_units_per_batch"] == 5
    assert archer_invisibility["official_post_2024_spawn_interval_ms"] == 1000
    for index, row in enumerate(archer_puppet_rows):
        assert row["level"] == index + 1
        assert row["source_fields"]["stats"]["archerInvisibilityDuration"] == f'{archer_invisibility["official_post_2024_invisibility_seconds_by_level"][index]:.1f}s'
        assert row["source_fields"]["stats"]["dpsIncrease"] == archer_bonus["official_post_buff_dps_bonus_by_level"][index]
        assert row["source_fields"]["hpRecoveryIncrease"] == archer_bonus["official_post_buff_hp_recovery_by_level"][index]
    assert archer_puppet_official_bonus_invisibility_delta["open_uncertainties"] and archer_puppet_official_bonus_invisibility_delta["rejected_inferences"]
    electro_boots_sources = electro_boots_official_aura_healing_delta["sources"]
    assert len(electro_boots_sources) == 1 and all(source["tier"] == "primary" and source["url"] and source["title"] and source["published_on"] and source["consulted_on"] and source["raw_sha256"] and source["content_id"] == "electro_boots" and source["levels"] == "1-27" and source["mode"] and source["confidence"] for source in electro_boots_sources)
    electro_boots_rows = next(content for content in bonus_catalogue["contents"] if content["id"] == "electro_boots")["levels"]
    electro_boots_official = electro_boots_official_aura_healing_delta["observations"][0]
    assert len(electro_boots_rows) == 27
    for index, row in enumerate(electro_boots_rows):
        assert row["level"] == index + 1
        assert row["source_fields"]["stats"]["auraDps"] == electro_boots_official["official_post_nerf_aura_dps_by_level"][index]
        assert row["source_fields"]["stats"]["selfHealingPerSecond"] == electro_boots_official["official_post_nerf_self_heal_per_second_by_level"][index]
    assert electro_boots_official_aura_healing_delta["open_uncertainties"] and electro_boots_official_aura_healing_delta["rejected_inferences"]
    tome_sources = healing_tome_official_duration_delta["sources"]
    assert len(tome_sources) == 1 and all(source["tier"] == "primary" and source["url"] and source["title"] and source["published_on"] and source["consulted_on"] and source["raw_sha256"] and source["content_id"] == "healing_tome" and source["levels"] == "1-18" and source["mode"] and source["confidence"] for source in tome_sources)
    tome_rows = next(content for content in bonus_catalogue["contents"] if content["id"] == "healing_tome")["levels"]
    tome_durations = healing_tome_official_duration_delta["observations"][0]["official_post_2024_duration_ms_by_level"]
    assert len(tome_rows) == len(tome_durations) == 18
    for index, row in enumerate(tome_rows):
        assert row["level"] == index + 1
        assert row["source_fields"]["stats"]["abilityDuration"] == f'{tome_durations[index] / 1000:.1f}s'
        assert tome_durations[index] % 10 == 0
        evidence = row["official_duration_evidence"]
        assert evidence == {"duration_ms": tome_durations[index],
                            "source_id": tome_sources[0]["id"],
                            "source_delta_sha256": hashlib.sha256(healing_tome_official_duration_bytes).hexdigest(),
                            "evidence_status": "announced_post_2024_not_current_combat_contract"}
        assert "official_duration_evidence" in row["provenance"]["overridden_fields"]
    assert healing_tome_official_duration_delta["open_uncertainties"] and healing_tome_official_duration_delta["rejected_inferences"]
    assert len(healing_tome_primary_level_provenance_delta["catalogue_overrides"]) == 18
    assert healing_tome_primary_level_provenance_delta["sources"][0] == tome_sources[0]
    batch = six_equipment_primary_level_provenance_delta
    assert len(batch["catalogue_overrides"]) == 117
    source_ids = {source["id"] for source in batch["sources"]}
    assert len(source_ids) == len(batch["sources"]) == 10
    source_delta_paths = {
        "royal_gem": ROYAL_GEM_OFFICIAL_LEVEL_BONUS_DELTA,
        "earthquake_boots": EARTHQUAKE_BOOTS_OFFICIAL_RADIUS_DAMAGE_DELTA,
        "vampstache": VAMPSTACHE_OFFICIAL_DPS_DELTA,
        "barbarian_puppet": BARBARIAN_PUPPET_OFFICIAL_BONUS_SPAWN_DELTA,
        "archer_puppet": ARCHER_PUPPET_OFFICIAL_BONUS_INVISIBILITY_DELTA,
        "electro_boots": ELECTRO_BOOTS_OFFICIAL_AURA_HEALING_DELTA,
    }
    source_delta_bytes = {content_id: path.read_bytes() for content_id, path in source_delta_paths.items()}
    source_delta_documents = {content_id: json.loads(raw) for content_id, raw in source_delta_bytes.items()}
    actual_batch_rows = {}
    for entry in batch["catalogue_overrides"]:
        content_id, level = entry["content_id"], entry["level"]
        assert (content_id, level) not in actual_batch_rows
        evidence = entry["fields"]["primary_level_evidence"]["value"]
        assert evidence["source_delta_sha256"] == hashlib.sha256(source_delta_bytes[content_id]).hexdigest()
        assert set(evidence["values"]) == set(evidence["value_sources"])
        assert set(evidence["value_sources"].values()) <= source_ids
        normalized_row = next(row for row in next(content for content in bonus_catalogue["contents"] if content["id"] == content_id)["levels"] if row["level"] == level)
        assert normalized_row["primary_level_evidence"] == evidence
        assert "primary_level_evidence" in normalized_row["provenance"]["overridden_fields"]
        actual_batch_rows[(content_id, level)] = evidence
    assert {content_id for content_id, _ in actual_batch_rows} == set(source_delta_bytes)
    for content_id, document in source_delta_documents.items():
        source_link = batch["source_deltas"][content_id]
        assert source_link == {"file": source_delta_paths[content_id].name,
                               "reference_id": document["reference_id"],
                               "sha256": hashlib.sha256(source_delta_bytes[content_id]).hexdigest()}
        for numeric_observation in document["observations"]:
            for field, value in numeric_observation.items():
                if not field.startswith("official_"):
                    continue
                if field.endswith("_by_level"):
                    start = 9 if content_id == "earthquake_boots" else 1
                    for index, reported in enumerate(value):
                        output_key = {
                            "official_post_buff_hp_bonus_by_level": "wielder_hp_bonus",
                            "official_post_buff_dps_bonus_by_level": "wielder_dps_bonus",
                            "official_post_buff_hp_recovery_by_level": "active_hp_recovery",
                            "official_post_2025_building_damage_percent_by_level": "building_damage_percent",
                            "official_post_2024_invisibility_seconds_by_level": "spawned_archer_invisibility_ms",
                            "official_post_nerf_aura_dps_by_level": "aura_dps",
                            "official_post_nerf_self_heal_per_second_by_level": "self_heal_per_second",
                        }[field]
                        assert numeric_observation["source"] == actual_batch_rows[(content_id, start + index)]["value_sources"][output_key]
                        expected = int(reported * 1000) if field == "official_post_2024_invisibility_seconds_by_level" else reported
                        assert actual_batch_rows[(content_id, start + index)]["values"][output_key] == expected
                elif field == "official_post_2024_radius_tiles":
                    assert all(actual_batch_rows[(content_id, level)]["values"]["radius_tiles"] == value for level in range(1, 19))
    assert len(hero_attachment_audit_delta["sources"]) == 3
    assert {source["tier"] for source in hero_attachment_audit_delta["sources"]} == {"primary"}
    assert all(source["url"] and source["title"] and source["consulted_on"] and
               source["content_id"] and source["levels"] and source["mode"] and
               source["confidence"] for source in hero_attachment_audit_delta["sources"])
    assert len(hero_attachment_audit_delta["derivations"]) == 2
    assert len(hero_attachment_audit_delta["rejected_inferences"]) == 4
    passive_override = electro_fangs_passive_delta["catalogue_content_overrides"][0]
    assert passive_override["content_id"] == "electro_fangs"
    assert passive_override["fields"]["equipment_activation_type"] == {
        "value": "Passive", "source": "supercell-2026-04-27-electro-fangs-passive-type",
        "note": "Primary release note explicitly labels the ability Passive; supplemental raw level fields remain unchanged."}
    assert electro_fangs_passive_delta["sources"][0]["tier"] == "primary"
    legacy_chain = FROZEN_REFERENCE["reference_chain"]["ids"][:-4]
    assert FROZEN_REFERENCE["reference_chain"]["ids"][-4:] == [healing_tome_primary_level_provenance_delta["reference_id"], six_equipment_primary_level_provenance_delta["reference_id"], royal_gem_active_heal_recheck_delta["reference_id"], monolith_arrow_passive_type_delta["reference_id"]]
    assert legacy_chain[-58:-24] == [poison_lizard_audit_delta["reference_id"], phoenix_audit_delta["reference_id"], spirit_fox_audit_delta["reference_id"], angry_jelly_audit_delta["reference_id"], sneezy_audit_delta["reference_id"], greedy_raven_audit_delta["reference_id"], revenge_deck_audit_delta["reference_id"], barbarian_puppet_audit_delta["reference_id"], rage_vial_audit_delta["reference_id"], earthquake_boots_audit_delta["reference_id"], vampstache_audit_delta["reference_id"], giant_gauntlet_audit_delta["reference_id"], spiky_ball_audit_delta["reference_id"], snake_bracelet_audit_delta["reference_id"], stick_horse_audit_delta["reference_id"], archer_puppet_audit_delta["reference_id"], invisibility_vial_audit_delta["reference_id"], giant_arrow_audit_delta["reference_id"], healer_puppet_audit_delta["reference_id"], frozen_arrow_audit_delta["reference_id"], magic_mirror_audit_delta["reference_id"], action_figure_audit_delta["reference_id"], monolith_arrow_audit_delta["reference_id"], henchmen_puppet_audit_delta["reference_id"], dark_orb_audit_delta["reference_id"], metal_pants_audit_delta["reference_id"], noble_iron_audit_delta["reference_id"], dark_crown_audit_delta["reference_id"], meteor_staff_audit_delta["reference_id"], eternal_tome_audit_delta["reference_id"], life_gem_audit_delta["reference_id"], rage_gem_audit_delta["reference_id"], healing_tome_audit_delta["reference_id"], fireball_audit_delta["reference_id"]]
    assert legacy_chain[-24:] == [lavaloon_puppet_audit_delta["reference_id"], heroic_torch_audit_delta["reference_id"], royal_gem_audit_delta["reference_id"], seeking_shield_audit_delta["reference_id"], hog_rider_puppet_audit_delta["reference_id"], haste_vial_audit_delta["reference_id"], rocket_spear_audit_delta["reference_id"], electro_boots_audit_delta["reference_id"], frost_flake_audit_delta["reference_id"], fire_heart_audit_delta["reference_id"], flame_blower_audit_delta["reference_id"], stun_blaster_audit_delta["reference_id"], electro_fangs_audit_delta["reference_id"], rocket_backpack_audit_delta["reference_id"], hero_attachment_audit_delta["reference_id"], electro_fangs_passive_delta["reference_id"], hero_battlefield_art_rights_audit_delta["reference_id"], royal_gem_official_level_bonus_delta["reference_id"], earthquake_boots_official_radius_damage_delta["reference_id"], vampstache_official_dps_delta["reference_id"], barbarian_puppet_official_bonus_spawn_delta["reference_id"], archer_puppet_official_bonus_invisibility_delta["reference_id"], electro_boots_official_aura_healing_delta["reference_id"], healing_tome_official_duration_delta["reference_id"]]
    prior_chain = legacy_chain[:-58]
    assert prior_chain[-58:-33] == [
        builders_hut_current_delta["reference_id"], ricochet_cannon_delta["reference_id"], super_wizard_tower_delta["reference_id"], revenge_tower_current_delta["reference_id"], multi_archer_tower_delta["reference_id"], multi_gear_tower_delta["reference_id"], scattershot_delta["reference_id"], eagle_artillery_scope_delta["reference_id"], town_hall_passive_scope_delta["reference_id"], bomb_tower_delta["reference_id"], x_bow_delta["reference_id"], inferno_tower_delta["reference_id"], ruin_witch_knight_followup_delta["reference_id"], furnace_firemite_followup_delta["reference_id"], meteor_golem_meteormite_followup_delta["reference_id"], revenge_tower_followup_delta["reference_id"], crafting_station_passive_delta["reference_id"], electromite_followup_delta["reference_id"], big_boy_followup_delta["reference_id"], lava_hound_lava_pup_delta["reference_id"], ruin_witch_movement_delta["reference_id"], air_sweeper_recheck_delta["reference_id"], builders_hut_multi_builder_delta["reference_id"], furnace_firemite_lifecycle_recheck_delta["reference_id"], spawned_subunit_assets_recheck_delta["reference_id"]
    ]
    assert prior_chain[-31:] == [grand_warden_delta["reference_id"], royal_champion_delta["reference_id"], minion_prince_delta["reference_id"], dragon_duke_delta["reference_id"], dragon_duke_base_attack_delta["reference_id"], royal_champion_wall_crossing_delta["reference_id"], hero_scope_audit_delta["reference_id"], hero_visual_icons_delta["reference_id"], hero_banner_defense_recheck_delta["reference_id"], flying_hero_defense_targetability_delta["reference_id"], grand_warden_air_mode_delta["reference_id"], hero_banner_visual_variants_delta["reference_id"], hero_banner_assignment_official_delta["reference_id"], hero_banner_th18_quantity_delta["reference_id"], attacking_hero_slots_delta["reference_id"], hero_battlefield_asset_audit_delta["reference_id"], hero_asset_rights_policy_delta["reference_id"], hero_projectile_contract_audit_delta["reference_id"], dragon_duke_trap_mitigation_delta["reference_id"], hero_friendly_challenge_modifier_scope_delta["reference_id"], hero_banner_defense_behavior_audit_delta["reference_id"], hero_collision_geometry_audit_delta["reference_id"], hero_defense_evidence_closure_delta["reference_id"], meteor_golem_meteormite_evidence_recheck_delta["reference_id"], hero_pet_equipment_scope_delta["reference_id"], lassi_combat_contract_audit_delta["reference_id"], electro_owl_audit_delta["reference_id"], mighty_yak_audit_delta["reference_id"], unicorn_audit_delta["reference_id"], frosty_audit_delta["reference_id"], diggy_audit_delta["reference_id"]]
    assert prior_chain[:-58] == [
        parent["reference_id"], delta["reference_id"], super_wall_breaker_delta["reference_id"],
        super_barbarian_delta["reference_id"], super_giant_delta["reference_id"], super_archer_delta["reference_id"], rocket_balloon_delta["reference_id"], inferno_dragon_delta["reference_id"], super_wizard_delta["reference_id"], super_minion_delta["reference_id"], super_bowler_delta["reference_id"], super_dragon_delta["reference_id"], x_bow_footprint_delta["reference_id"], electro_dragon_chain_delta["reference_id"], bowler_bounce_delta["reference_id"], ice_golem_death_freeze_delta["reference_id"], apprentice_warden_life_aura_delta["reference_id"], super_hog_rider_split_delta["reference_id"], super_miner_delta["reference_id"], super_valkyrie_delta["reference_id"], super_yeti_delta["reference_id"], super_witch_delta["reference_id"], ice_hound_delta["reference_id"], yeti_delta["reference_id"], witch_delta["reference_id"], lava_hound_delta["reference_id"], headhunter_delta["reference_id"], druid_delta["reference_id"], furnace_delta["reference_id"], meteor_golem_delta["reference_id"], ruin_witch_knight_delta["reference_id"], baby_dragon_delta["reference_id"], pekka_delta["reference_id"], goblin_delta["reference_id"], minion_delta["reference_id"], hog_rider_delta["reference_id"], valkyrie_delta["reference_id"], dragon_rider_delta["reference_id"], miner_delta["reference_id"], electro_dragon_current_delta["reference_id"], ruin_witch_knight_current_delta["reference_id"], dragon_delta["reference_id"], balloon_delta["reference_id"], golem_delta["reference_id"], wizard_delta["reference_id"], giant_delta["reference_id"], archer_delta["reference_id"], barbarian_delta["reference_id"], healer_delta["reference_id"], wall_breaker_delta["reference_id"], electro_titan_delta["reference_id"], thrower_delta["reference_id"], root_rider_delta["reference_id"], sneaky_goblin_delta["reference_id"], air_sweeper_current_delta["reference_id"], spell_tower_current_delta["reference_id"], firespitter_current_delta["reference_id"]
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
    electro_fangs_content = next(item for item in catalogue["contents"] if item["id"] == "electro_fangs")
    assert electro_fangs_content["equipment_activation_type"] == "Passive"
    assert electro_fangs_content["field_provenance"]["equipment_activation_type"]["source"] == \
        "supercell-2026-04-27-electro-fangs-passive-type"
    owl = next(content for content in catalogue["contents"] if content["id"] == "electro_owl")
    assert owl["support"] == "catalogued_only"
    assert [level["hitpoints"] for level in owl["levels"][10:]] == [2600, 2700, 2800, 2900, 3000]
    assert [level["dps"] for level in owl["levels"][10:]] == [150, 155, 160, 165, 170]
    assert [level["damage_per_shot"] for level in owl["levels"]] == [140, 147, 154, 161, 168, 175, 182, 189, 196, 203, 210, 217, 224, 231, 238]
    assert all(set(level["provenance"]["overridden_fields"]) >= {"hitpoints", "dps"} for level in owl["levels"][10:])
    yak = next(content for content in catalogue["contents"] if content["id"] == "mighty_yak")
    assert yak["support"] == "catalogued_only" and yak["splash_radius_tiles"] == 0
    assert [level["damage_vs_walls"] for level in yak["levels"]] == [20 * level["damage_per_shot"] for level in yak["levels"]]
    assert [level["hitpoints"] for level in yak["levels"][10:]] == [5700, 5850, 6000, 6150, 6300]
    assert [level["dps"] for level in yak["levels"][10:]] == [100, 104, 108, 112, 116]
    assert all(set(level["provenance"]["overridden_fields"]) >= {"hitpoints", "dps"} for level in yak["levels"][10:])
    unicorn = next(content for content in catalogue["contents"] if content["id"] == "unicorn")
    assert unicorn["support"] == "catalogued_only"
    assert [level["healing_per_second"] for level in unicorn["levels"][3:10]] == [58, 60, 62, 64, 66, 68, 70]
    assert [level["healing_per_pulse"] for level in unicorn["levels"]] == [level["healing_per_second"] for level in unicorn["levels"]]
    assert all("healing_per_second" in level["provenance"]["overridden_fields"] for level in unicorn["levels"][3:10])
    frosty = next(content for content in catalogue["contents"] if content["id"] == "frosty")
    assert frosty["support"] == "catalogued_only"
    assert len(frosty["levels"]) == 15
    assert [level["frostmites_per_summon"] for level in frosty["levels"]] == [1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3]
    assert [level["max_frostmites"] for level in frosty["levels"]] == [4, 4, 4, 4, 8, 8, 8, 8, 8, 10, 10, 10, 10, 10, 12]
    diggy_observation_bytes = DIGGY_L11_L15_OBSERVATION.read_bytes()
    diggy_observation = json.loads(diggy_observation_bytes)
    diggy_extra = [item for item in FROZEN_REFERENCE["catalogue_extra_levels"] if item["content_id"] == "diggy"]
    assert [item["level"] for item in diggy_extra] == [11, 12, 13, 14, 15]
    assert all(item["observation_file"] == DIGGY_L11_L15_OBSERVATION.name and item["observation_sha256"] == hashlib.sha256(diggy_observation_bytes).hexdigest() and item["core_materializable"] is False for item in diggy_extra)
    assert [item["level"] for item in diggy_observation["observations"]] == [11, 12, 13, 14, 15]
    diggy = next(content for content in catalogue["contents"] if content["id"] == "diggy")
    assert diggy["support"] == "catalogued_only" and [level["level"] for level in diggy["levels"]] == list(range(1, 16))
    assert [level["hitpoints"] for level in diggy["levels"][10:]] == [5150, 5300, 5450, 5600, 5750]
    assert [level["dps"] for level in diggy["levels"][10:]] == [155, 160, 165, 170, 175]
    assert all(level["core_materializable"] is False for level in diggy["levels"][10:])
    poison_lizard = next(content for content in catalogue["contents"] if content["id"] == "poison_lizard")
    poison_raw_bytes = POISON_LIZARD_RAW.read_bytes()
    poison_source = next(source for source in poison_lizard_audit_delta["sources"] if source["id"] == "chiefpansancolt-62b019df-poison-lizard-raw-recheck")
    assert poison_source["raw_sha256"] == hashlib.sha256(poison_raw_bytes).hexdigest()
    poison_raw = json.loads(poison_raw_bytes)
    assert poison_lizard["support"] == "catalogued_only" and poison_lizard["range_tiles"] == 4.5
    assert len(poison_lizard["levels"]) == len(poison_raw["levels"]) == 15
    for level, source_level in zip(poison_lizard["levels"], poison_raw["levels"]):
        assert level["level"] == source_level["level"]
        assert level["poison_max_dps"] == source_level["poisonMaxDps"]
        assert level["poison_speed_decrease_percent"] == source_level["poisonSpeedDecreasePercent"]
        assert level["poison_attack_rate_decrease_percent"] == source_level["poisonAttackRateDecreasePercent"]
        assert {"hitpoints", "dps"} <= set(level["provenance"]["overridden_fields"])
    phoenix = next(content for content in catalogue["contents"] if content["id"] == "phoenix")
    phoenix_raw_bytes = PHOENIX_RAW.read_bytes()
    phoenix_source = next(source for source in phoenix_audit_delta["sources"] if source["id"] == "chiefpansancolt-62b019df-phoenix-raw-recheck")
    assert phoenix_source["raw_sha256"] == hashlib.sha256(phoenix_raw_bytes).hexdigest()
    phoenix_raw = json.loads(phoenix_raw_bytes)
    assert phoenix["support"] == "catalogued_only" and len(phoenix["levels"]) == len(phoenix_raw["levels"]) == 10
    for level, source_level in zip(phoenix["levels"], phoenix_raw["levels"]):
        assert level["level"] == source_level["level"]
        assert level["revive_duration_seconds"] == source_level["reviveDuration"]
    spirit_fox = next(content for content in catalogue["contents"] if content["id"] == "spirit_fox")
    fox_raw_bytes = SPIRIT_FOX_RAW.read_bytes()
    fox_source = next(source for source in spirit_fox_audit_delta["sources"] if source["id"] == "chiefpansancolt-62b019df-spirit-fox-raw-recheck")
    assert fox_source["raw_sha256"] == hashlib.sha256(fox_raw_bytes).hexdigest()
    fox_raw = json.loads(fox_raw_bytes)
    assert spirit_fox["support"] == "catalogued_only" and len(spirit_fox["levels"]) == len(fox_raw["levels"]) == 10
    for level, source_level in zip(spirit_fox["levels"], fox_raw["levels"]):
        assert level["level"] == source_level["level"]
        assert level["invisibility_duration_seconds"] == source_level["invisibilityDuration"]
    angry_jelly = next(content for content in catalogue["contents"] if content["id"] == "angry_jelly")
    jelly_raw_bytes = ANGRY_JELLY_RAW.read_bytes()
    jelly_source = next(source for source in angry_jelly_audit_delta["sources"] if source["id"] == "chiefpansancolt-62b019df-angry-jelly-raw-recheck")
    assert jelly_source["raw_sha256"] == hashlib.sha256(jelly_raw_bytes).hexdigest()
    jelly_raw = json.loads(jelly_raw_bytes)
    jelly_range_override = angry_jelly_audit_delta["catalogue_content_overrides"][0]["fields"]["range_tiles"]
    assert angry_jelly["support"] == "catalogued_only" and jelly_raw["range"] == 1.5
    assert angry_jelly["range_tiles"] == jelly_range_override["value"] == 5
    assert angry_jelly["field_provenance"]["range_tiles"]["source"] == jelly_range_override["source"]
    assert len(angry_jelly["levels"]) == len(jelly_raw["levels"]) == 10
    for level, source_level in zip(angry_jelly["levels"], jelly_raw["levels"]):
        assert level["level"] == source_level["level"]
        assert level["brainwash_duration_seconds"] == source_level["brainwashDuration"]
    sneezy = next(content for content in catalogue["contents"] if content["id"] == "sneezy")
    sneezy_raw_bytes = SNEEZY_RAW.read_bytes()
    sneezy_source = next(source for source in sneezy_audit_delta["sources"] if source["id"] == "chiefpansancolt-62b019df-sneezy-raw-recheck")
    assert sneezy_source["raw_sha256"] == hashlib.sha256(sneezy_raw_bytes).hexdigest()
    sneezy_raw = json.loads(sneezy_raw_bytes)
    assert sneezy["support"] == "catalogued_only" and len(sneezy["levels"]) == len(sneezy_raw["levels"]) == 10
    for name, raw_name in (("max_boogers_summoned", "maxBoogersSummoned"),
                           ("rage_duration_seconds", "rageDuration"),
                           ("rage_damage_increase_percent_raw", "rageDamageIncreasePercent")):
        evidence = sneezy_audit_delta["catalogue_content_overrides"][0]["fields"][name]
        assert sneezy[name] == evidence["value"] == sneezy_raw[raw_name]
        assert sneezy["field_provenance"][name]["source"] == evidence["source"]
    greedy_raven = next(content for content in catalogue["contents"] if content["id"] == "greedy_raven")
    raven_raw_bytes = GREEDY_RAVEN_RAW.read_bytes()
    raven_source = next(source for source in greedy_raven_audit_delta["sources"] if source["id"] == "chiefpansancolt-62b019df-greedy-raven-raw-recheck")
    assert raven_source["raw_sha256"] == hashlib.sha256(raven_raw_bytes).hexdigest()
    raven_raw = json.loads(raven_raw_bytes)
    assert greedy_raven["support"] == "catalogued_only" and greedy_raven["range_tiles"] == 8
    assert len(greedy_raven["levels"]) == len(raven_raw["levels"]) == 10
    for level, source_level in zip(greedy_raven["levels"], raven_raw["levels"]):
        assert level["level"] == source_level["level"]
        assert level["dps_on_resource_buildings"] == source_level["dpsOnResourceBuildings"] == 5 * source_level["damagePerSecond"]
    equipment_ids = next(entry["ids"] for entry in FROZEN_REFERENCE["secondary_inventory_indices"]
                         if entry["family"] == "equipment")
    assert len(equipment_ids) == 42 and equipment_ids[-1] == "revenge_deck"
    assert "portal_pendant" not in equipment_ids
    supplement = revenge_deck_audit_delta["catalogue_supplemental_contents"][0]
    observation_bytes = REVENGE_DECK_OBSERVATION.read_bytes()
    observation_content = json.loads(observation_bytes)["content"]
    assert supplement["observation_sha256"] == hashlib.sha256(observation_bytes).hexdigest()
    revenge_deck = next(content for content in catalogue["contents"] if content["id"] == "revenge_deck")
    assert revenge_deck["category"] == "hero-equipment" and revenge_deck["support"] == "catalogued_only"
    assert revenge_deck["source_fields"] == {"hero": "dragon-duke", "rarity": "Epic", "abilityType": "Passive"}
    assert len(revenge_deck["levels"]) == len(observation_content["levels"]) == 27
    for imported_level, observed_level in zip(revenge_deck["levels"], observation_content["levels"]):
        assert imported_level["level"] == observed_level["level"]
        assert imported_level["source_fields"] == observed_level["source_fields"]
        assert imported_level["provenance"]["source"]["raw_sha256"] == supplement["observation_sha256"]
    assert [sum(level["source_fields"][key] for level in revenge_deck["levels"])
            for key in ("upgradeShinyOre", "upgradeGlowingOre", "upgradeStarryOre")] == [56060, 3720, 480]
    puppet_raw_bytes = BARBARIAN_PUPPET_RAW.read_bytes()
    puppet_source = next(source for source in barbarian_puppet_audit_delta["sources"]
                         if source["id"] == "chiefpansancolt-62b019df-barbarian-puppet-raw-recheck")
    assert puppet_source["raw_sha256"] == hashlib.sha256(puppet_raw_bytes).hexdigest()
    puppet_raw = json.loads(puppet_raw_bytes)
    puppet = next(content for content in catalogue["contents"] if content["id"] == "barbarian_puppet")
    assert puppet["support"] == "catalogued_only" and len(puppet["levels"]) == len(puppet_raw["levels"]) == 18
    assert puppet["source_fields"]["ability"]["barbarianRageDuration"] == puppet_raw["ability"]["barbarianRageDuration"]
    for imported_level, raw_level in zip(puppet["levels"], puppet_raw["levels"]):
        assert imported_level["level"] == raw_level["level"]
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert imported_level["source_fields"]["hpRecoveryIncrease"] == raw_level["hpRecoveryIncrease"]
    vial_raw_bytes = RAGE_VIAL_RAW.read_bytes()
    vial_source = next(source for source in rage_vial_audit_delta["sources"]
                       if source["id"] == "chiefpansancolt-62b019df-rage-vial-raw-recheck")
    assert vial_source["raw_sha256"] == hashlib.sha256(vial_raw_bytes).hexdigest()
    vial_raw = json.loads(vial_raw_bytes)
    vial = next(content for content in catalogue["contents"] if content["id"] == "rage_vial")
    assert vial["support"] == "catalogued_only" and len(vial["levels"]) == len(vial_raw["levels"]) == 18
    assert vial["source_fields"]["ability"]["abilityDuration"] == vial_raw["ability"]["abilityDuration"] == 10
    for imported_level, raw_level in zip(vial["levels"], vial_raw["levels"]):
        assert imported_level["level"] == raw_level["level"]
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hpRecoveryIncrease"] == raw_level["hpRecoveryIncrease"]
    boots_raw_bytes = EARTHQUAKE_BOOTS_RAW.read_bytes()
    boots_source = next(source for source in earthquake_boots_audit_delta["sources"]
                        if source["id"] == "chiefpansancolt-62b019df-earthquake-boots-raw-recheck")
    assert boots_source["raw_sha256"] == hashlib.sha256(boots_raw_bytes).hexdigest()
    boots_raw = json.loads(boots_raw_bytes)
    boots = next(content for content in catalogue["contents"] if content["id"] == "earthquake_boots")
    assert boots["support"] == "catalogued_only" and len(boots["levels"]) == len(boots_raw["levels"]) == 18
    assert boots["source_fields"]["ability"] == boots_raw["ability"]
    for imported_level, raw_level in zip(boots["levels"], boots_raw["levels"]):
        assert imported_level["level"] == raw_level["level"]
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
    vamp_raw_bytes = VAMPSTACHE_RAW.read_bytes()
    vamp_source = next(source for source in vampstache_audit_delta["sources"]
                       if source["id"] == "chiefpansancolt-62b019df-vampstache-raw-recheck")
    assert vamp_source["raw_sha256"] == hashlib.sha256(vamp_raw_bytes).hexdigest()
    vamp_raw = json.loads(vamp_raw_bytes)
    vamp = next(content for content in catalogue["contents"] if content["id"] == "vampstache")
    assert vamp["support"] == "catalogued_only" and len(vamp["levels"]) == len(vamp_raw["levels"]) == 18
    for imported_level, raw_level in zip(vamp["levels"], vamp_raw["levels"]):
        assert imported_level["level"] == raw_level["level"]
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
    gauntlet_raw_bytes = GIANT_GAUNTLET_RAW.read_bytes()
    gauntlet_source = next(source for source in giant_gauntlet_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-giant-gauntlet-raw-recheck")
    assert gauntlet_source["raw_sha256"] == hashlib.sha256(gauntlet_raw_bytes).hexdigest()
    gauntlet_raw = json.loads(gauntlet_raw_bytes)
    gauntlet = next(content for content in catalogue["contents"] if content["id"] == "giant_gauntlet")
    assert gauntlet["support"] == "catalogued_only" and len(gauntlet["levels"]) == len(gauntlet_raw["levels"]) == 27
    assert gauntlet["source_fields"]["ability"] == gauntlet_raw["ability"]
    for imported_level, raw_level in zip(gauntlet["levels"], gauntlet_raw["levels"]):
        assert imported_level["level"] == raw_level["level"]
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
    ball_raw_bytes = SPIKY_BALL_RAW.read_bytes()
    ball_source = next(source for source in spiky_ball_audit_delta["sources"]
                       if source["id"] == "chiefpansancolt-62b019df-spiky-ball-raw-recheck")
    assert ball_source["raw_sha256"] == hashlib.sha256(ball_raw_bytes).hexdigest()
    ball_raw = json.loads(ball_raw_bytes)
    ball = next(content for content in catalogue["contents"] if content["id"] == "spiky_ball")
    assert ball["support"] == "catalogued_only" and len(ball["levels"]) == len(ball_raw["levels"]) == 27
    launch_counts = spiky_ball_audit_delta["observations"][0]["launch_number_of_targets"]
    pinned_counts = spiky_ball_audit_delta["observations"][0]["pinned_number_of_targets"]
    assert len(launch_counts) == len(pinned_counts) == 27
    for index, (imported_level, raw_level) in enumerate(zip(ball["levels"], ball_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert pinned_counts[index] == raw_level["stats"]["numberOfTargets"] < launch_counts[index]
    bracelet_raw_bytes = SNAKE_BRACELET_RAW.read_bytes()
    bracelet_source = next(source for source in snake_bracelet_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-snake-bracelet-raw-recheck")
    assert bracelet_source["raw_sha256"] == hashlib.sha256(bracelet_raw_bytes).hexdigest()
    bracelet_raw = json.loads(bracelet_raw_bytes)
    bracelet = next(content for content in catalogue["contents"] if content["id"] == "snake_bracelet")
    assert bracelet["support"] == "catalogued_only" and len(bracelet["levels"]) == len(bracelet_raw["levels"]) == 27
    assert bracelet["source_fields"]["abilityType"] == bracelet_raw["abilityType"] == "Passive"
    caps = snake_bracelet_audit_delta["observations"][0]["max_summoned_snakes_by_level"]
    child_levels = snake_bracelet_audit_delta["observations"][0]["snake_level_by_equipment_level"]
    assert len(caps) == len(child_levels) == 27 and child_levels[-1] == 10
    assert len(snake_bracelet_audit_delta["observations"][1]["reported_hitpoints"]) == 10
    assert len(snake_bracelet_audit_delta["observations"][1]["reported_dps"]) == 10
    for index, (imported_level, raw_level) in enumerate(zip(bracelet["levels"], bracelet_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert caps[index] == raw_level["stats"]["maxSummonedSnakes"]
        assert child_levels[index] == raw_level["stats"]["snakeLevel"]
    horse_raw_bytes = STICK_HORSE_RAW.read_bytes()
    horse_source = next(source for source in stick_horse_audit_delta["sources"]
                        if source["id"] == "chiefpansancolt-62b019df-stick-horse-raw-recheck")
    assert horse_source["raw_sha256"] == hashlib.sha256(horse_raw_bytes).hexdigest()
    horse_raw = json.loads(horse_raw_bytes)
    horse = next(content for content in catalogue["contents"] if content["id"] == "stick_horse")
    assert horse["support"] == "catalogued_only" and len(horse["levels"]) == len(horse_raw["levels"]) == 27
    assert horse["source_fields"]["abilityType"] == horse_raw["abilityType"] == "Passive"
    for index, (imported_level, raw_level) in enumerate(zip(horse["levels"], horse_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
    assert horse_raw["levels"][0]["stats"]["abilityDuration"] == "12s"
    assert horse_raw["levels"][-1]["stats"]["abilityDuration"] == "30s"
    archer_raw_bytes = ARCHER_PUPPET_RAW.read_bytes()
    archer_source = next(source for source in archer_puppet_audit_delta["sources"]
                         if source["id"] == "chiefpansancolt-62b019df-archer-puppet-raw-recheck")
    assert archer_source["raw_sha256"] == hashlib.sha256(archer_raw_bytes).hexdigest()
    archer_raw = json.loads(archer_raw_bytes)
    archer = next(content for content in catalogue["contents"] if content["id"] == "archer_puppet")
    assert archer["support"] == "catalogued_only" and len(archer["levels"]) == len(archer_raw["levels"]) == 18
    assert archer["source_fields"]["abilityType"] == archer_raw["abilityType"] == "Active"
    summoned = archer_puppet_audit_delta["observations"][0]["summoned_archers_by_level"]
    invisible_seconds = archer_puppet_audit_delta["observations"][0]["invisibility_seconds_by_level"]
    assert len(summoned) == len(invisible_seconds) == 18
    for index, (imported_level, raw_level) in enumerate(zip(archer["levels"], archer_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hpRecoveryIncrease"] == raw_level["hpRecoveryIncrease"]
        assert summoned[index] == raw_level["stats"]["summonedArchers"]
        assert invisible_seconds[index] == float(raw_level["stats"]["archerInvisibilityDuration"].removesuffix("s"))
    invis_raw_bytes = INVISIBILITY_VIAL_RAW.read_bytes()
    invis_source = next(source for source in invisibility_vial_audit_delta["sources"]
                        if source["id"] == "chiefpansancolt-62b019df-invisibility-vial-raw-recheck")
    assert invis_source["raw_sha256"] == hashlib.sha256(invis_raw_bytes).hexdigest()
    invis_raw = json.loads(invis_raw_bytes)
    invis = next(content for content in catalogue["contents"] if content["id"] == "invisibility_vial")
    assert invis["support"] == "catalogued_only" and len(invis["levels"]) == len(invis_raw["levels"]) == 18
    assert invis["source_fields"]["abilityType"] == invis_raw["abilityType"] == "Active"
    durations = invisibility_vial_audit_delta["observations"][0]["duration_seconds_by_level"]
    extra_shots = invisibility_vial_audit_delta["observations"][0]["extra_damage_per_shot_by_level"]
    assert len(durations) == len(extra_shots) == 18
    for index, (imported_level, raw_level) in enumerate(zip(invis["levels"], invis_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert durations[index] == float(raw_level["stats"]["abilityDuration"].removesuffix("s"))
        assert extra_shots[index] == raw_level["stats"]["damagePerShotIncrease"]
    arrow_raw_bytes = GIANT_ARROW_RAW.read_bytes()
    arrow_source = next(source for source in giant_arrow_audit_delta["sources"]
                        if source["id"] == "chiefpansancolt-62b019df-giant-arrow-raw-recheck")
    assert arrow_source["raw_sha256"] == hashlib.sha256(arrow_raw_bytes).hexdigest()
    arrow_raw = json.loads(arrow_raw_bytes)
    arrow = next(content for content in catalogue["contents"] if content["id"] == "giant_arrow")
    assert arrow["support"] == "catalogued_only" and len(arrow["levels"]) == len(arrow_raw["levels"]) == 18
    assert arrow["source_fields"]["hero"] == arrow_raw["hero"] == "archer-queen"
    assert arrow["source_fields"]["abilityType"] == arrow_raw["abilityType"] == "Active"
    assert arrow["source_fields"]["ability"] == arrow_raw["ability"] == {"damageRadius": "1 tile"}
    pinned_damage = giant_arrow_audit_delta["observations"][0]["pinned_projectile_damage_by_level"]
    for index, (imported_level, raw_level) in enumerate(zip(arrow["levels"], arrow_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert pinned_damage[index] == raw_level["stats"]["projectileDamage"]
    known_new = {"9": 1100, "12": 1200, "15": 1350, "18": 1500}
    assert giant_arrow_audit_delta["observations"][1]["published_new_damage_by_level"] == known_new
    assert arrow["official_2026_projectile_damage_by_level"] == known_new
    assert arrow["official_2026_air_defense_damage_multiplier"] == 2
    assert {arrow["field_provenance"][field]["source"] for field in (
        "official_2026_projectile_damage_by_level",
        "official_2026_air_defense_damage_multiplier")} == {"supercell-2026-05-22-giant-arrow-balance"}
    healer_puppet_raw_bytes = HEALER_PUPPET_RAW.read_bytes()
    healer_puppet_source = next(source for source in healer_puppet_audit_delta["sources"]
                                if source["id"] == "chiefpansancolt-62b019df-healer-puppet-raw-recheck")
    assert healer_puppet_source["raw_sha256"] == hashlib.sha256(healer_puppet_raw_bytes).hexdigest()
    healer_puppet_raw = json.loads(healer_puppet_raw_bytes)
    healer_puppet = next(content for content in catalogue["contents"] if content["id"] == "healer_puppet")
    assert healer_puppet["support"] == "catalogued_only" and len(healer_puppet["levels"]) == 18
    assert healer_puppet["source_fields"]["hero"] == healer_puppet_raw["hero"] == "archer-queen"
    assert healer_puppet["source_fields"]["abilityType"] == healer_puppet_raw["abilityType"] == "Active"
    healer_observation = healer_puppet_audit_delta["observations"][0]
    for index, (imported_level, raw_level) in enumerate(zip(healer_puppet["levels"], healer_puppet_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert healer_observation["summoned_healers_by_level"][index] == raw_level["stats"]["summonedHealers"]
        assert healer_observation["healer_level_by_equipment_level"][index] == raw_level["stats"]["healerLevel"]
        assert healer_observation["self_healing_per_second_by_level"][index] == raw_level["stats"]["selfHealingPerSecond"]
    frozen_arrow_raw_bytes = FROZEN_ARROW_RAW.read_bytes()
    frozen_arrow_source = next(source for source in frozen_arrow_audit_delta["sources"]
                               if source["id"] == "chiefpansancolt-62b019df-frozen-arrow-raw-recheck")
    assert frozen_arrow_source["raw_sha256"] == hashlib.sha256(frozen_arrow_raw_bytes).hexdigest()
    frozen_arrow_raw = json.loads(frozen_arrow_raw_bytes)
    frozen_arrow = next(content for content in catalogue["contents"] if content["id"] == "frozen_arrow")
    assert frozen_arrow["support"] == "catalogued_only" and len(frozen_arrow["levels"]) == 27
    assert frozen_arrow["source_fields"]["hero"] == frozen_arrow_raw["hero"] == "archer-queen"
    assert frozen_arrow["source_fields"]["abilityType"] == frozen_arrow_raw["abilityType"] == "Passive"
    frozen_observation = frozen_arrow_audit_delta["observations"][0]
    for index, (imported_level, raw_level) in enumerate(zip(frozen_arrow["levels"], frozen_arrow_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert frozen_observation["pinned_post_2024_slow_percent_by_level"][index] == int(raw_level["stats"]["slowDown"].removesuffix("%"))
        assert frozen_observation["pinned_slow_duration_seconds_by_level"][index] == float(raw_level["stats"]["slowDownDuration"].removesuffix("s"))
    mirror_raw_bytes = MAGIC_MIRROR_RAW.read_bytes()
    mirror_source = next(source for source in magic_mirror_audit_delta["sources"]
                         if source["id"] == "chiefpansancolt-62b019df-magic-mirror-raw-recheck")
    assert mirror_source["raw_sha256"] == hashlib.sha256(mirror_raw_bytes).hexdigest()
    mirror_raw = json.loads(mirror_raw_bytes)
    mirror = next(content for content in catalogue["contents"] if content["id"] == "magic_mirror")
    assert mirror["support"] == "catalogued_only" and len(mirror["levels"]) == 27
    assert mirror["source_fields"]["hero"] == mirror_raw["hero"] == "archer-queen"
    assert mirror["source_fields"]["abilityType"] == mirror_raw["abilityType"] == "Active"
    assert mirror["source_fields"]["ability"] == mirror_raw["ability"] == {"invisibilityDuration": "1s"}
    mirror_observation = magic_mirror_audit_delta["observations"][0]
    for index, (imported_level, raw_level) in enumerate(zip(mirror["levels"], mirror_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert imported_level["source_fields"]["hpRecoveryIncrease"] == raw_level["hpRecoveryIncrease"]
        for observation_field, raw_field in (("pinned_clone_count_by_level", "clones"),
                                             ("pinned_clone_dps_by_level", "cloneDps"),
                                             ("pinned_clone_hp_by_level", "cloneHealth")):
            assert mirror_observation[observation_field][index] == raw_level["stats"][raw_field]
        assert mirror_observation["pinned_clone_duration_seconds_by_level"][index] == int(raw_level["stats"]["cloneDuration"].removesuffix("s"))
    figure_raw_bytes = ACTION_FIGURE_RAW.read_bytes()
    figure_source = next(source for source in action_figure_audit_delta["sources"]
                         if source["id"] == "chiefpansancolt-62b019df-action-figure-raw-recheck")
    assert figure_source["raw_sha256"] == hashlib.sha256(figure_raw_bytes).hexdigest()
    figure_raw = json.loads(figure_raw_bytes)
    figure = next(content for content in catalogue["contents"] if content["id"] == "action_figure")
    assert figure["support"] == "catalogued_only" and len(figure["levels"]) == 27
    assert figure["source_fields"]["hero"] == figure_raw["hero"] == "archer-queen"
    assert figure["source_fields"]["abilityType"] == figure_raw["abilityType"] == "Active"
    assert figure["source_fields"]["ability"] == figure_raw["ability"] == {"invisibilityDuration": "1s"}
    figure_observation = action_figure_audit_delta["observations"][0]
    for index, (imported_level, raw_level) in enumerate(zip(figure["levels"], figure_raw["levels"])):
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["stats"] == raw_level["stats"]
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"]
        assert figure_observation["pinned_giant_giant_level_by_equipment_level"][index] == raw_level["stats"]["giantGiantLevel"]
        assert figure_observation["pinned_queen_self_heal_per_second_by_level"][index] == raw_level["stats"]["selfHealingPerSecond"]
    monolith_arrow_observation_bytes = MONOLITH_ARROW_OBSERVATION.read_bytes()
    monolith_arrow_source = next(source for source in monolith_arrow_audit_delta["sources"]
                                 if source["id"] == "goblins-farm-2026-09-17-monolith-arrow-frozen-observation-recheck")
    assert monolith_arrow_source["observation_sha256"] == hashlib.sha256(monolith_arrow_observation_bytes).hexdigest()
    monolith_arrow_observation = json.loads(monolith_arrow_observation_bytes)["content"]
    monolith_arrow = next(content for content in catalogue["contents"] if content["id"] == "monolith_arrow")
    assert monolith_arrow["support"] == "catalogued_only" and len(monolith_arrow["levels"]) == 27
    assert monolith_arrow["hero"] == monolith_arrow_observation["hero"] == "archer_queen"
    assert monolith_arrow["behavior"]["damage_percent_by_deployed_housing_space"] == [
        {"maximum": 180, "percent": 14}, {"minimum": 181, "maximum": 250, "percent": 10},
        {"minimum": 251, "percent": 5}]
    assert monolith_arrow["behavior"]["damage_reduction_cap_percent"] == 10
    for index, (imported_level, observed_level) in enumerate(zip(monolith_arrow["levels"], monolith_arrow_observation["levels"])):
        assert imported_level["level"] == observed_level["level"] == index + 1
        assert imported_level["hero_hitpoints_bonus"] == observed_level["hero_hitpoints_bonus"]
        assert imported_level["blacksmith_required"] == observed_level["blacksmith_required"]
    henchmen_puppet_source = next(source for source in henchmen_puppet_audit_delta["sources"]
                                  if source["id"] == "chiefpansancolt-62b019df-henchmen-puppet-raw-recheck")
    assert henchmen_puppet_source["raw_sha256"] == hashlib.sha256(HENCHMEN_PUPPET_RAW.read_bytes()).hexdigest()
    henchmen_puppet = next(content for content in catalogue["contents"] if content["id"] == "henchmen_puppet")
    henchmen_raw = json.loads(HENCHMEN_PUPPET_RAW.read_text(encoding="utf-8"))
    assert henchmen_puppet["support"] == "catalogued_only" and len(henchmen_puppet["levels"]) == 18
    assert henchmen_puppet["source_fields"]["ability"] == {"henchmenCount": 2, "invisibilityDuration": "1s"}
    henchmen_observation = henchmen_puppet_audit_delta["observations"][0]
    for index, imported_level in enumerate(henchmen_puppet["levels"]):
        raw_level = henchmen_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert imported_level["source_fields"]["hitpointIncrease"] == raw_level["hitpointIncrease"] == 500
        assert henchmen_observation["pinned_henchmen_level_by_equipment_level"][index] == raw_level["stats"]["henchmenLevel"]
        assert henchmen_observation["pinned_hero_dps_bonus_by_level"][index] == raw_level["stats"]["dpsIncrease"]
    dark_orb_source = next(source for source in dark_orb_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-dark-orb-raw-recheck")
    assert dark_orb_source["raw_sha256"] == hashlib.sha256(DARK_ORB_RAW.read_bytes()).hexdigest()
    dark_orb = next(content for content in catalogue["contents"] if content["id"] == "dark_orb")
    dark_orb_raw = json.loads(DARK_ORB_RAW.read_text(encoding="utf-8"))
    assert dark_orb["support"] == "catalogued_only" and len(dark_orb["levels"]) == 18
    assert dark_orb["source_fields"]["abilityType"] == "Active"
    assert dark_orb["dated_2026_hero_hp_bonus_by_level"] == {"8": 690}
    assert dark_orb["field_provenance"]["dated_2026_hero_hp_bonus_by_level"]["source"] == "goblins-farm-2026-09-18-dark-orb-table"
    assert dark_orb_audit_delta["observations"][0]["dated_2026_hero_hp_level_8"] == 690
    dark_orb_observation = dark_orb_audit_delta["observations"][0]
    for index, imported_level in enumerate(dark_orb["levels"]):
        raw_level = dark_orb_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert dark_orb_observation["pinned_projectile_damage_by_level"][index] == raw_level["stats"]["projectileDamage"]
        assert dark_orb_observation["pinned_slow_percent_by_level"][index] == raw_level["stats"]["slowDownPercent"]
        assert dark_orb_observation["pinned_slow_duration_seconds_by_level"][index] == int(raw_level["stats"]["slowDownDuration"][:-1])
        assert dark_orb_observation["pinned_hero_dps_by_level"][index] == raw_level["stats"]["dpsIncrease"]
        assert dark_orb_observation["pinned_hero_hp_by_level"][index] == raw_level["hitpointIncrease"]
    metal_pants_source = next(source for source in metal_pants_audit_delta["sources"]
                              if source["id"] == "chiefpansancolt-62b019df-metal-pants-raw-recheck")
    assert metal_pants_source["raw_sha256"] == hashlib.sha256(METAL_PANTS_RAW.read_bytes()).hexdigest()
    metal_pants = next(content for content in catalogue["contents"] if content["id"] == "metal_pants")
    metal_pants_raw = json.loads(METAL_PANTS_RAW.read_text(encoding="utf-8"))
    assert metal_pants["support"] == "catalogued_only" and len(metal_pants["levels"]) == 18
    assert metal_pants["source_fields"]["abilityType"] == "Active"
    metal_pants_observation = metal_pants_audit_delta["observations"][0]
    for index, imported_level in enumerate(metal_pants["levels"]):
        raw_level = metal_pants_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert metal_pants_observation["pinned_duration_seconds_by_level"][index] == int(raw_level["stats"]["abilityDuration"][:-1])
        assert metal_pants_observation["pinned_incoming_reduction_percent_by_level"][index] == raw_level["stats"]["incomingDamageReduction"]
        assert metal_pants_observation["pinned_hero_hp_by_level"][index] == raw_level["hitpointIncrease"]
        assert metal_pants_observation["pinned_hero_recovery_by_level"][index] == raw_level["hpRecoveryIncrease"]
    noble_iron_source = next(source for source in noble_iron_audit_delta["sources"]
                             if source["id"] == "chiefpansancolt-62b019df-noble-iron-raw-recheck")
    assert noble_iron_source["raw_sha256"] == hashlib.sha256(NOBLE_IRON_RAW.read_bytes()).hexdigest()
    noble_iron = next(content for content in catalogue["contents"] if content["id"] == "noble_iron")
    noble_iron_raw = json.loads(NOBLE_IRON_RAW.read_text(encoding="utf-8"))
    assert noble_iron["support"] == "catalogued_only" and len(noble_iron["levels"]) == 18
    assert noble_iron["source_fields"]["abilityType"] == "Passive"
    noble_iron_observation = noble_iron_audit_delta["observations"][0]
    for index, imported_level in enumerate(noble_iron["levels"]):
        raw_level = noble_iron_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert noble_iron_observation["pinned_extra_damage_by_level"][index] == raw_level["stats"]["damagePerShotIncrease"]
        assert noble_iron_observation["pinned_range_tiles_by_level"][index] == raw_level["stats"]["attackRange"]
        assert noble_iron_observation["pinned_boosted_attack_count_by_level"][index] == raw_level["stats"]["numberOfAttacks"]
        assert noble_iron_observation["pinned_attack_speed_increase_percent_by_level"][index] == raw_level["stats"]["attackSpeedIncrease"]
        assert raw_level["hitpointIncrease"] == noble_iron_observation["pinned_hero_hp_bonus"]
    dark_crown_source = next(source for source in dark_crown_audit_delta["sources"]
                             if source["id"] == "chiefpansancolt-62b019df-dark-crown-raw-recheck")
    assert dark_crown_source["raw_sha256"] == hashlib.sha256(DARK_CROWN_RAW.read_bytes()).hexdigest()
    dark_crown = next(content for content in catalogue["contents"] if content["id"] == "dark_crown")
    dark_crown_raw = json.loads(DARK_CROWN_RAW.read_text(encoding="utf-8"))
    assert dark_crown["support"] == "catalogued_only" and len(dark_crown["levels"]) == 27
    assert dark_crown["source_fields"]["abilityType"] == "Passive"
    assert dark_crown_audit_delta["observations"][0]["historical_cumulative_defeated_housing_thresholds"] == [60, 120, 180]
    dark_crown_observation = dark_crown_audit_delta["observations"][1]
    for index, imported_level in enumerate(dark_crown["levels"]):
        raw_level = dark_crown_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert dark_crown_observation["pinned_max_health_increase_by_level"][index] == raw_level["stats"]["maxHealthIncrease"]
        assert dark_crown_observation["pinned_max_damage_increase_by_level"][index] == raw_level["stats"]["maxDamageIncrease"]
        assert dark_crown_observation["pinned_attack_speed_increase_by_level"][index] == raw_level["stats"]["attackSpeedIncrease"]
    meteor_staff_source = next(source for source in meteor_staff_audit_delta["sources"]
                               if source["id"] == "chiefpansancolt-62b019df-meteor-staff-raw-recheck")
    assert meteor_staff_source["raw_sha256"] == hashlib.sha256(METEOR_STAFF_RAW.read_bytes()).hexdigest()
    meteor_staff = next(content for content in catalogue["contents"] if content["id"] == "meteor_staff")
    meteor_staff_raw = json.loads(METEOR_STAFF_RAW.read_text(encoding="utf-8"))
    assert meteor_staff["support"] == "catalogued_only" and len(meteor_staff["levels"]) == 27
    assert meteor_staff["source_fields"]["abilityType"] == "Passive"
    meteor_staff_observation = meteor_staff_audit_delta["observations"][0]
    for index, imported_level in enumerate(meteor_staff["levels"]):
        raw_level = meteor_staff_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        cooldown_ms = int(Decimal(raw_level["stats"]["cooldownTime"][:-1]) * 1000)
        assert meteor_staff_observation["pinned_cooldown_milliseconds_by_level"][index] == cooldown_ms
        assert cooldown_ms % 10 == 0
        assert meteor_staff_observation["pinned_damage_per_hit_by_level"][index] == raw_level["stats"]["damagePerHit"]
    eternal_tome_source = next(source for source in eternal_tome_audit_delta["sources"]
                              if source["id"] == "chiefpansancolt-62b019df-eternal-tome-raw-recheck")
    assert eternal_tome_source["raw_sha256"] == hashlib.sha256(ETERNAL_TOME_RAW.read_bytes()).hexdigest()
    eternal_tome = next(content for content in catalogue["contents"] if content["id"] == "eternal_tome")
    eternal_tome_raw = json.loads(ETERNAL_TOME_RAW.read_text(encoding="utf-8"))
    assert eternal_tome["support"] == "catalogued_only" and len(eternal_tome["levels"]) == 18
    assert eternal_tome["source_fields"]["abilityType"] == "Active"
    eternal_tome_observation = eternal_tome_audit_delta["observations"][0]
    assert eternal_tome_observation["official_exit_grace_milliseconds"] == 1000
    for index, imported_level in enumerate(eternal_tome["levels"]):
        raw_level = eternal_tome_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        duration_ms = int(Decimal(raw_level["stats"]["abilityDuration"][:-1]) * 1000)
        assert eternal_tome_observation["pinned_duration_milliseconds_by_level"][index] == duration_ms
        assert duration_ms % 10 == 0
    life_gem_source = next(source for source in life_gem_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-life-gem-raw-recheck")
    assert life_gem_source["raw_sha256"] == hashlib.sha256(LIFE_GEM_RAW.read_bytes()).hexdigest()
    life_gem = next(content for content in catalogue["contents"] if content["id"] == "life_gem")
    life_gem_raw = json.loads(LIFE_GEM_RAW.read_text(encoding="utf-8"))
    assert life_gem["support"] == "catalogued_only" and len(life_gem["levels"]) == 18
    assert life_gem["source_fields"]["abilityType"] == "Passive"
    life_gem_observation = life_gem_audit_delta["observations"][0]
    assert life_gem_observation["official_changed_levels"] == {"extra_hp_and_cap": "1-7", "warden_hp_and_dps": "1-18"}
    for index, imported_level in enumerate(life_gem["levels"]):
        raw_level = life_gem_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert life_gem_observation["pinned_extra_hp_percent_by_level"][index] == int(raw_level["stats"]["hpIncreasePercent"][:-1])
        assert life_gem_observation["pinned_extra_hp_cap_by_level"][index] == raw_level["stats"]["maxHpIncrease"]
    rage_gem_source = next(source for source in rage_gem_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-rage-gem-raw-recheck")
    assert rage_gem_source["raw_sha256"] == hashlib.sha256(RAGE_GEM_RAW.read_bytes()).hexdigest()
    rage_gem = next(content for content in catalogue["contents"] if content["id"] == "rage_gem")
    rage_gem_raw = json.loads(RAGE_GEM_RAW.read_text(encoding="utf-8"))
    assert rage_gem["support"] == "catalogued_only" and len(rage_gem["levels"]) == 18
    assert rage_gem["source_fields"]["abilityType"] == "Passive"
    rage_gem_observation = rage_gem_audit_delta["observations"][0]
    for index, imported_level in enumerate(rage_gem["levels"]):
        raw_level = rage_gem_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert rage_gem_observation["pinned_damage_increase_percent_by_level"][index] == int(raw_level["stats"]["damageIncreasePercent"][:-1])
        assert rage_gem_observation["pinned_warden_attack_speed_increase_percent_by_level"][index] == int(raw_level["stats"]["attackSpeedIncrease"][1:-1])
    healing_tome_source = next(source for source in healing_tome_audit_delta["sources"]
                               if source["id"] == "chiefpansancolt-62b019df-healing-tome-raw-recheck")
    assert healing_tome_source["raw_sha256"] == hashlib.sha256(HEALING_TOME_RAW.read_bytes()).hexdigest()
    healing_tome = next(content for content in catalogue["contents"] if content["id"] == "healing_tome")
    healing_tome_raw = json.loads(HEALING_TOME_RAW.read_text(encoding="utf-8"))
    assert healing_tome["support"] == "catalogued_only" and len(healing_tome["levels"]) == 18
    assert healing_tome["source_fields"]["abilityType"] == "Active"
    healing_tome_observation = healing_tome_audit_delta["observations"][0]
    for index, imported_level in enumerate(healing_tome["levels"]):
        raw_level = healing_tome_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        duration_ms = int(Decimal(raw_level["stats"]["abilityDuration"][:-1]) * 1000)
        assert healing_tome_observation["pinned_duration_milliseconds_by_level"][index] == duration_ms
        assert duration_ms % 10 == 0
        assert healing_tome_observation["pinned_healing_per_second_by_level"][index] == raw_level["stats"]["healingPerSecond"]
    fireball_source = next(source for source in fireball_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-fireball-raw-recheck")
    assert fireball_source["raw_sha256"] == hashlib.sha256(FIREBALL_RAW.read_bytes()).hexdigest()
    fireball = next(content for content in catalogue["contents"] if content["id"] == "fireball")
    fireball_raw = json.loads(FIREBALL_RAW.read_text(encoding="utf-8"))
    assert fireball["support"] == "catalogued_only" and len(fireball["levels"]) == 27
    assert fireball["source_fields"]["abilityType"] == "Active"
    fireball_observation = fireball_audit_delta["observations"][0]
    for index, imported_level in enumerate(fireball["levels"]):
        raw_level = fireball_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert fireball_observation["pinned_damage_radius_tiles_by_level"][index] == int(raw_level["stats"]["damageRadius"].split()[0])
        assert fireball_observation["pinned_projectile_damage_by_level"][index] == raw_level["stats"]["projectileDamage"]
    assert fireball_observation["pinned_damage_radius_tiles_by_level"][17:24] == [5] * 6 + [6]
    lavaloon_source = next(source for source in lavaloon_puppet_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-lavaloon-puppet-raw-recheck")
    assert lavaloon_source["raw_sha256"] == hashlib.sha256(LAVALOON_PUPPET_RAW.read_bytes()).hexdigest()
    lavaloon_puppet = next(content for content in catalogue["contents"] if content["id"] == "lavaloon_puppet")
    lavaloon_raw = json.loads(LAVALOON_PUPPET_RAW.read_text(encoding="utf-8"))
    assert lavaloon_puppet["support"] == "catalogued_only" and len(lavaloon_puppet["levels"]) == 27
    assert lavaloon_puppet["source_fields"]["abilityType"] == "Active"
    lavaloon_observation = lavaloon_puppet_audit_delta["observations"][0]
    for index, imported_level in enumerate(lavaloon_puppet["levels"]):
        raw_level = lavaloon_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert lavaloon_observation["pinned_summon_count_by_level"][index] == raw_level["stats"]["summonedLavaloons"]
        assert lavaloon_observation["pinned_spawned_lavaloon_level_by_equipment_level"][index] == raw_level["stats"]["lavaloonLevel"]
    assert lavaloon_observation["pinned_summon_count_by_level"][20:] == [3] * 7
    heroic_torch_source = next(source for source in heroic_torch_audit_delta["sources"]
                               if source["id"] == "chiefpansancolt-62b019df-heroic-torch-raw-recheck")
    assert heroic_torch_source["raw_sha256"] == hashlib.sha256(HEROIC_TORCH_RAW.read_bytes()).hexdigest()
    heroic_torch = next(content for content in catalogue["contents"] if content["id"] == "heroic_torch")
    heroic_torch_raw = json.loads(HEROIC_TORCH_RAW.read_text(encoding="utf-8"))
    assert heroic_torch["support"] == "catalogued_only" and len(heroic_torch["levels"]) == 27
    assert heroic_torch["source_fields"]["abilityType"] == "Active"
    torch_observation = heroic_torch_audit_delta["observations"][0]
    for index, imported_level in enumerate(heroic_torch["levels"]):
        raw_level = heroic_torch_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        stats = raw_level["stats"]
        assert torch_observation["pinned_duration_seconds_by_level"][index] == float(stats["abilityDuration"][:-1])
        assert torch_observation["pinned_speed_increase_by_level"][index] == stats["speedIncrease"]
        assert torch_observation["pinned_incoming_damage_reduction_percent_by_level"][index] == int(stats["incomingDamageReduction"][:-1])
    royal_gem_source = next(source for source in royal_gem_audit_delta["sources"]
                            if source["id"] == "chiefpansancolt-62b019df-royal-gem-raw-recheck")
    assert royal_gem_source["raw_sha256"] == hashlib.sha256(ROYAL_GEM_RAW.read_bytes()).hexdigest()
    royal_gem = next(content for content in catalogue["contents"] if content["id"] == "royal_gem")
    royal_gem_raw = json.loads(ROYAL_GEM_RAW.read_text(encoding="utf-8"))
    assert royal_gem["support"] == "catalogued_only" and len(royal_gem["levels"]) == 18
    assert royal_gem["source_fields"]["abilityType"] == "Active"
    royal_gem_observation = royal_gem_audit_delta["observations"][0]
    for index, imported_level in enumerate(royal_gem["levels"]):
        raw_level = royal_gem_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert royal_gem_observation["pinned_health_recovery_by_level"][index] == raw_level["stats"]["healthRecovery"]
        assert royal_gem_observation["pinned_hp_bonus_by_level"][index] == raw_level["hitpointIncrease"]
        assert royal_gem_observation["pinned_dps_bonus_by_level"][index] == raw_level["stats"]["dpsIncrease"]
    seeking_shield_source = next(source for source in seeking_shield_audit_delta["sources"]
                                 if source["id"] == "chiefpansancolt-62b019df-seeking-shield-raw-recheck")
    assert seeking_shield_source["raw_sha256"] == hashlib.sha256(SEEKING_SHIELD_RAW.read_bytes()).hexdigest()
    seeking_shield = next(content for content in catalogue["contents"] if content["id"] == "seeking_shield")
    seeking_shield_raw = json.loads(SEEKING_SHIELD_RAW.read_text(encoding="utf-8"))
    assert seeking_shield["support"] == "catalogued_only" and len(seeking_shield["levels"]) == 18
    assert seeking_shield["source_fields"]["abilityType"] == "Active"
    shield_observation = seeking_shield_audit_delta["observations"][0]
    assert seeking_shield_raw["ability"]["numberOfTargets"] == shield_observation["pinned_target_count"] == 4
    for index, imported_level in enumerate(seeking_shield["levels"]):
        raw_level = seeking_shield_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert shield_observation["pinned_projectile_damage_by_level"][index] == raw_level["stats"]["projectileDamage"]
        assert shield_observation["pinned_hp_bonus_by_level"][index] == raw_level["hitpointIncrease"]
    hog_puppet_source = next(source for source in hog_rider_puppet_audit_delta["sources"]
                             if source["id"] == "chiefpansancolt-62b019df-hog-puppet-raw-recheck")
    assert hog_puppet_source["raw_sha256"] == hashlib.sha256(HOG_RIDER_PUPPET_RAW.read_bytes()).hexdigest()
    hog_puppet = next(content for content in catalogue["contents"] if content["id"] == "hog_rider_puppet")
    hog_puppet_raw = json.loads(HOG_RIDER_PUPPET_RAW.read_text(encoding="utf-8"))
    assert hog_puppet["support"] == "catalogued_only" and len(hog_puppet["levels"]) == 18
    assert hog_puppet["source_fields"]["abilityType"] == "Active"
    hog_observation = hog_rider_puppet_audit_delta["observations"][0]
    assert hog_puppet_raw["ability"]["invisibility"] == "1s"
    assert hog_observation["pinned_champion_invisibility_milliseconds"] == 1000
    for index, imported_level in enumerate(hog_puppet["levels"]):
        raw_level = hog_puppet_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert hog_observation["pinned_summon_count_by_level"][index] == raw_level["stats"]["summonedHogRiders"]
        assert hog_observation["pinned_child_level_by_equipment_level"][index] == raw_level["stats"]["hogRiderLevel"]
    haste_vial_source = next(source for source in haste_vial_audit_delta["sources"]
                             if source["id"] == "chiefpansancolt-62b019df-haste-vial-raw-recheck")
    assert haste_vial_source["raw_sha256"] == hashlib.sha256(HASTE_VIAL_RAW.read_bytes()).hexdigest()
    haste_vial = next(content for content in catalogue["contents"] if content["id"] == "haste_vial")
    haste_vial_raw = json.loads(HASTE_VIAL_RAW.read_text(encoding="utf-8"))
    assert haste_vial["support"] == "catalogued_only" and len(haste_vial["levels"]) == 18
    assert haste_vial["source_fields"]["abilityType"] == "Active"
    haste_observation = haste_vial_audit_delta["observations"][0]
    for index, imported_level in enumerate(haste_vial["levels"]):
        raw_level = haste_vial_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        stats = raw_level["stats"]
        assert haste_observation["pinned_duration_milliseconds_by_level"][index] == int(Decimal(stats["abilityDuration"][:-1]) * 1000)
        assert haste_observation["pinned_active_attack_speed_increase_percent_by_level"][index] == int(stats["abilityAttackSpeedIncrease"][1:-1])
        assert haste_observation["pinned_passive_attack_speed_increase_percent_by_level"][index] == int(stats["attackSpeedIncrease"][1:-1])
        assert Decimal(str(haste_observation["pinned_movement_speed_increase_by_level"][index])) == Decimal(str(stats["speedIncrease"]))
    assert haste_observation["pinned_passive_attack_speed_increase_percent_by_level"][8:] == [10,11,12,12,13,14,14,15,16,16]
    rocket_source = next(source for source in rocket_spear_audit_delta["sources"]
                         if source["id"] == "chiefpansancolt-62b019df-rocket-spear-raw-recheck")
    assert rocket_source["raw_sha256"] == hashlib.sha256(ROCKET_SPEAR_RAW.read_bytes()).hexdigest()
    rocket_spear = next(content for content in catalogue["contents"] if content["id"] == "rocket_spear")
    rocket_raw = json.loads(ROCKET_SPEAR_RAW.read_text(encoding="utf-8"))
    assert rocket_spear["support"] == "catalogued_only" and len(rocket_spear["levels"]) == 27
    assert rocket_spear["source_fields"]["abilityType"] == "Active"
    rocket_observation = rocket_spear_audit_delta["observations"][0]
    assert rocket_observation["pinned_range_tiles"] == int(rocket_raw["ability"]["attackRange"].split()[0])
    assert Decimal(str(rocket_observation["pinned_area_damage_radius_tiles"])) == Decimal(rocket_raw["ability"]["areaDamageRadius"].split()[0])
    for index, imported_level in enumerate(rocket_spear["levels"]):
        raw_level = rocket_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert rocket_observation["pinned_extra_damage_per_shot_by_level"][index] == raw_level["stats"]["damagePerShotIncrease"]
        assert rocket_observation["pinned_charged_attack_count_by_level"][index] == raw_level["stats"]["numberOfAttacks"]
        assert rocket_observation["pinned_hp_bonus_by_level"][index] == raw_level["hitpointIncrease"]
        assert rocket_observation["pinned_dps_bonus_by_level"][index] == raw_level["stats"]["dpsIncrease"]
    electro_source = next(source for source in electro_boots_audit_delta["sources"]
                          if source["id"] == "chiefpansancolt-62b019df-electro-boots-raw-recheck")
    assert electro_source["raw_sha256"] == hashlib.sha256(ELECTRO_BOOTS_RAW.read_bytes()).hexdigest()
    electro_boots = next(content for content in catalogue["contents"] if content["id"] == "electro_boots")
    electro_raw = json.loads(ELECTRO_BOOTS_RAW.read_text(encoding="utf-8"))
    assert electro_boots["support"] == "catalogued_only" and len(electro_boots["levels"]) == 27
    assert electro_boots["source_fields"]["abilityType"] == "Passive"
    electro_observation = electro_boots_audit_delta["observations"][0]
    assert electro_observation["pinned_aura_interval_ms"] == int(Decimal(electro_raw["ability"]["auraAttackSpeed"][:-1]) * 1000)
    assert electro_observation["pinned_radius_tiles"] == int(electro_raw["ability"]["auraRadius"].split()[0])
    for index, imported_level in enumerate(electro_boots["levels"]):
        raw_level = electro_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        for key, observation_key in (("auraDps", "pinned_aura_dps_by_level"),
                                     ("auraDamagePerHit", "pinned_aura_damage_per_hit_by_level"),
                                     ("selfHealingPerSecond", "pinned_self_heal_per_second_by_level")):
            assert Decimal(str(electro_observation[observation_key][index])) == Decimal(str(raw_level["stats"][key]))
    frost_source = next(source for source in frost_flake_audit_delta["sources"]
                        if source["id"] == "chiefpansancolt-62b019df-frost-flake-raw-recheck")
    assert frost_source["raw_sha256"] == hashlib.sha256(FROST_FLAKE_RAW.read_bytes()).hexdigest()
    frost_flake = next(content for content in catalogue["contents"] if content["id"] == "frost_flake")
    frost_raw = json.loads(FROST_FLAKE_RAW.read_text(encoding="utf-8"))
    assert frost_flake["support"] == "catalogued_only" and len(frost_flake["levels"]) == 27
    assert frost_flake["source_fields"]["abilityType"] == "Active"
    frost_observation = frost_flake_audit_delta["observations"][0]
    assert frost_observation["pinned_slow_down_percent"] == int(frost_raw["ability"]["slowDown"][:-1])
    for index, imported_level in enumerate(frost_flake["levels"]):
        raw_level = frost_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert frost_observation["pinned_projectile_damage_by_level"][index] == raw_level["stats"]["projectileDamage"]
        assert frost_observation["pinned_slow_down_duration_ms_by_level"][index] == int(Decimal(raw_level["stats"]["slowDownDuration"][:-1]) * 1000)
        assert frost_observation["pinned_target_count_by_level"][index] == raw_level["stats"]["numberOfTargets"]
    heart_source = next(source for source in fire_heart_audit_delta["sources"]
                        if source["id"] == "chiefpansancolt-62b019df-fire-heart-raw-recheck")
    assert heart_source["raw_sha256"] == hashlib.sha256(FIRE_HEART_RAW.read_bytes()).hexdigest()
    fire_heart = next(content for content in catalogue["contents"] if content["id"] == "fire_heart")
    heart_raw = json.loads(FIRE_HEART_RAW.read_text(encoding="utf-8"))
    assert fire_heart["support"] == "catalogued_only" and len(fire_heart["levels"]) == 18
    assert fire_heart["source_fields"]["abilityType"] == "Passive"
    heart_observation, current_heart = fire_heart_audit_delta["observations"]
    assert heart_observation["pinned_radius_tiles"] == int(heart_raw["ability"]["damageRadius"].split()[0])
    for index, imported_level in enumerate(fire_heart["levels"]):
        raw_level = heart_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        for key, observation_key in (("damageOnDefeat", "pinned_damage_on_defeat_by_level"),
                                     ("selfHealingPerSecond", "pinned_regeneration_per_second_by_level"),
                                     ("dpsIncrease", "pinned_dps_bonus_by_level")):
            assert heart_observation[observation_key][index] == raw_level["stats"][key]
    assert current_heart["official_new_dps_bonus_by_level"] == [10,10,12,12,12,14,14,14,16,16,16,18,18,18,20,20,20,23]
    assert current_heart["official_new_regeneration_per_second_at_levels"] == {"15": 140, "18": 150}
    blower_source = next(source for source in flame_blower_audit_delta["sources"]
                         if source["id"] == "chiefpansancolt-62b019df-flame-blower-raw-recheck")
    assert blower_source["raw_sha256"] == hashlib.sha256(FLAME_BLOWER_RAW.read_bytes()).hexdigest()
    flame_blower = next(content for content in catalogue["contents"] if content["id"] == "flame_blower")
    blower_raw = json.loads(FLAME_BLOWER_RAW.read_text(encoding="utf-8"))
    assert flame_blower["support"] == "catalogued_only" and len(flame_blower["levels"]) == 18
    assert flame_blower["source_fields"]["abilityType"] == "Active"
    blower_observation = flame_blower_audit_delta["observations"][0]
    assert blower_observation["pinned_damage_radius_tiles"] == int(blower_raw["ability"]["damageRadius"].split()[0])
    for index, imported_level in enumerate(flame_blower["levels"]):
        raw_level = blower_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert blower_observation["pinned_ability_damage_by_level"][index] == raw_level["stats"]["abilityDamage"]
        assert blower_observation["pinned_hp_recovery_increase_by_level"][index] == raw_level["hpRecoveryIncrease"]
    assert blower_observation["l15_damage_exceeds_l16_l17"] is True
    assert blower_observation["pinned_ability_damage_by_level"][14] > blower_observation["pinned_ability_damage_by_level"][15]
    blaster_source = next(source for source in stun_blaster_audit_delta["sources"]
                          if source["id"] == "chiefpansancolt-62b019df-stun-blaster-raw-recheck")
    assert blaster_source["raw_sha256"] == hashlib.sha256(STUN_BLASTER_RAW.read_bytes()).hexdigest()
    stun_blaster = next(content for content in catalogue["contents"] if content["id"] == "stun_blaster")
    blaster_raw = json.loads(STUN_BLASTER_RAW.read_text(encoding="utf-8"))
    assert stun_blaster["support"] == "catalogued_only" and len(stun_blaster["levels"]) == 18
    assert stun_blaster["source_fields"]["abilityType"] == "Active"
    blaster_observation = stun_blaster_audit_delta["observations"][0]
    assert blaster_observation["pinned_damage_radius_tiles"] == int(blaster_raw["ability"]["damageRadius"].split()[0])
    for index, imported_level in enumerate(stun_blaster["levels"]):
        raw_level = blaster_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert blaster_observation["pinned_ability_total_damage_by_level"][index] == raw_level["stats"]["abilityTotalDamage"]
        assert blaster_observation["pinned_stun_duration_ms_by_level"][index] == int(Decimal(raw_level["stats"]["stunDuration"][:-1]) * 1000)
        assert blaster_observation["pinned_dps_bonus_by_level"][index] == raw_level["stats"]["dpsIncrease"]
    fangs_source = next(source for source in electro_fangs_audit_delta["sources"]
                        if source["id"] == "frozen-2026-09-17-electro-fangs-observation-recheck")
    assert fangs_source["observation_sha256"] == hashlib.sha256(ELECTRO_FANGS_OBSERVATION.read_bytes()).hexdigest()
    electro_fangs = next(content for content in catalogue["contents"] if content["id"] == "electro_fangs")
    fangs_frozen = json.loads(ELECTRO_FANGS_OBSERVATION.read_text(encoding="utf-8"))["content"]
    assert electro_fangs["support"] == "catalogued_only" and len(electro_fangs["levels"]) == 18
    assert electro_fangs["behavior"] == fangs_frozen["behavior"]
    assert [entry["hero_hitpoints_bonus"] for entry in electro_fangs["levels"]] == [entry["hero_hitpoints_bonus"] for entry in fangs_frozen["levels"]]
    fangs_observation = electro_fangs_audit_delta["observations"][0]
    assert len(fangs_observation["community_damage_per_hit_by_level"]) == 18
    assert fangs_observation["community_damage_per_hit_by_level"][-1] == fangs_observation["primary_max_damage_per_strike"] == 400
    assert fangs_observation["community_number_of_chain_targets_by_level"][-1] == fangs_observation["primary_max_chain_targets"] == 4
    assert fangs_observation["primary_damage_decay_percent_per_jump"] == 20
    backpack_source = next(source for source in rocket_backpack_audit_delta["sources"]
                           if source["id"] == "chiefpansancolt-62b019df-rocket-backpack-raw-recheck")
    assert backpack_source["raw_sha256"] == hashlib.sha256(ROCKET_BACKPACK_RAW.read_bytes()).hexdigest()
    rocket_backpack = next(content for content in catalogue["contents"] if content["id"] == "rocket_backpack")
    backpack_raw = json.loads(ROCKET_BACKPACK_RAW.read_text(encoding="utf-8"))
    assert rocket_backpack["support"] == "catalogued_only" and len(rocket_backpack["levels"]) == 27
    assert rocket_backpack["source_fields"]["abilityType"] == "Active"
    backpack_observation, current_backpack = rocket_backpack_audit_delta["observations"]
    assert backpack_observation["pinned_damage_area_tiles"] == int(backpack_raw["ability"]["damageArea"].split()[0])
    for index, imported_level in enumerate(rocket_backpack["levels"]):
        raw_level = backpack_raw["levels"][index]
        assert imported_level["level"] == raw_level["level"] == index + 1
        assert backpack_observation["pinned_ability_damage_by_level"][index] == raw_level["stats"]["abilityDamage"]
        assert backpack_observation["pinned_hp_recovery_increase_by_level"][index] == raw_level["hpRecoveryIncrease"]
    assert current_backpack["official_new_penetrating_damage_at_levels"] == {"21": 1875, "27": 2150}
    assert backpack_observation["pinned_ability_damage_by_level"][20] == 1925
    assert backpack_observation["pinned_ability_damage_by_level"][26] == 2200
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
    x_bow = next(content for content in catalogue["contents"] if content["id"] == "x_bow")
    assert x_bow["footprint_tiles"] == "3x3"
    assert x_bow["field_provenance"]["footprint_tiles"]["source"] == "coc-guide-x-bow-footprint-2026-09-18"
    assert x_bow_footprint_delta["changed_values"] == [{
        "content_id": "x_bow",
        "previous_state": "The pinned upstream payload and normalized catalogue used a 4x4 footprint.",
        "replacement": "All X-Bow levels use a 3x3 footprint.",
        "justification": "Two independent Home Village references explicitly report 3x3. The upstream field is retained in its immutable raw cache and superseded only through this dated additive correction.",
    }]
    electro_dragon = next(content for content in catalogue["contents"] if content["id"] == "electro_dragon")
    assert (electro_dragon["support"], electro_dragon["chain_damage_multiplier"],
            electro_dragon["chain_target_count"], electro_dragon["chain_radius_tiles"]) == (
                "core_supported_primary_secondary_partial", .8, 5, 1)
    assert electro_dragon["field_provenance"]["chain_target_count"]["source"] == "supercell-2018-06-08-electro-dragon-launch"
    assert electro_dragon_chain_delta["known_uncertainties"][1].startswith("The death lightning")
    electro_dragon_levels = {level["level"]: (level["hitpoints"], level["dps"])
                             for level in electro_dragon["levels"]}
    assert electro_dragon_levels == {1: (3400, 260), 2: (3900, 290), 3: (4400, 320),
                                     4: (4700, 350), 5: (5000, 380), 6: (5400, 410),
                                     7: (5700, 440), 8: (6400, 475), 9: (6900, 510)}
    current_direct_parent = electro_dragon_current_delta["verified_values"]["direct_parent"]
    assert current_direct_parent["housing_space"] == 30
    assert current_direct_parent["attack_speed_seconds"] == 3.5
    assert current_direct_parent["range_tiles"] == 2.5
    assert current_direct_parent["target_type"] == "both"
    assert electro_dragon_current_delta["known_uncertainties"][1].startswith("The official launch text")
    bowler = next(content for content in catalogue["contents"] if content["id"] == "bowler")
    bowler_ten = next(level for level in bowler["levels"] if level["level"] == 10)
    assert (bowler["support"], bowler["bounce_impact_count"], bowler["bounce_step_tiles"],
            bowler_ten["hitpoints"], bowler_ten["damage_per_shot"], bowler_ten["dps"]) == (
                "core_supported_primary_secondary_lower_bound", 2, 3, 860, 352, 160)
    assert bowler["field_provenance"]["bounce_impact_count"]["source"] == "supercell-2021-09-25-super-bowler-comparison"
    ice_golem = next(content for content in catalogue["contents"] if content["id"] == "ice_golem")
    durations = {level["level"]: level["death_freeze_duration_seconds"] for level in ice_golem["levels"]}
    assert (ice_golem["support"], ice_golem["death_freeze_radius_tiles"],
            [durations[level] for level in range(1, 10)]) == (
                "core_supported_primary_secondary_partial", 7.5,
                [4, 4.75, 5.5, 6.25, 7, 7.5, 8, 8.5, 9])
    assert ice_golem["field_provenance"]["death_freeze_radius_tiles"]["source"] == "fandom-2026-09-18-ice-golem"
    assert ice_golem_death_freeze_delta["known_uncertainties"][0].startswith("Public sources do not publish")
    apprentice_warden = next(content for content in catalogue["contents"] if content["id"] == "apprentice_warden")
    aura_increases = {level["level"]: level["life_aura_hp_increase_percent"] for level in apprentice_warden["levels"]}
    assert (apprentice_warden["support"], apprentice_warden["aura_range_tiles"],
            [aura_increases[level] for level in range(1, 5)]) == (
                "core_supported_primary_partial", 7, [24, 26, 28, 30])
    assert apprentice_warden["field_provenance"]["aura_range_tiles"]["source"] == "supercell-2023-06-12-apprentice-warden-life-aura"
    assert apprentice_warden_life_aura_delta["known_uncertainties"][0].startswith("Supercell does not publish")
    super_hog_rider = next(content for content in catalogue["contents"] if content["id"] == "super_hog_rider")
    super_hog = next(content for content in catalogue["contents"] if content["id"] == "super_hog")
    super_rider = next(content for content in catalogue["contents"] if content["id"] == "super_rider")
    assert (super_hog_rider["support"], next(level for level in super_hog_rider["levels"] if level["level"] == 12)["hitpoints"]) == ("core_supported_primary_secondary_partial", 1700)
    assert (super_hog["support"], [level["hitpoints"] for level in super_hog["levels"]]) == ("spawned_only", [800, 900, 1000, 1050, 1100, 1200])
    assert (super_rider["support"], super_rider["wall_damage_multiplier"], [level["damage_per_shot"] for level in super_rider["levels"]]) == ("spawned_only", 2, [180, 200, 230, 250, 270, 300])
    assert {item["content_id"] for item in super_hog_rider_split_delta["catalogue_supplemental_contents"]} == {"super_hog", "super_rider"}
    assert super_hog_rider_split_delta["known_uncertainties"][1].startswith("The public documentation describes")
    super_miner = next(content for content in catalogue["contents"] if content["id"] == "super_miner")
    super_miner_l12 = next(level for level in super_miner["levels"] if level["level"] == 12)
    assert (super_miner["support"], super_miner["burrows"], super_miner["death_splash_radius_tiles"],
            super_miner_l12["hitpoints"], super_miner_l12["dps"], super_miner_l12["death_damage"],
            super_miner_l12["mode_stats"]["stage3"]["dps"]) == (
                "core_supported_primary_secondary_partial", True, 2, 5000, 265, 2400, 740)
    assert super_miner["field_provenance"]["inferno_ramp"]["source"] == "fandom-2026-09-18-super-miner"
    assert super_miner_delta["known_uncertainties"][1].startswith("Public material does not state")
    super_valkyrie = next(content for content in catalogue["contents"] if content["id"] == "super_valkyrie")
    super_valkyrie_l12 = next(level for level in super_valkyrie["levels"] if level["level"] == 12)
    assert (super_valkyrie["support"], super_valkyrie["attack_speed_seconds"], super_valkyrie_l12["hitpoints"], super_valkyrie_l12["damage_per_shot"]) == ("core_supported_primary_secondary_partial", 1.1, 4500, 445.5)
    assert super_valkyrie_delta["known_uncertainties"][0].startswith("No reliable public source")
    super_yeti = next(content for content in catalogue["contents"] if content["id"] == "super_yeti")
    super_yeti_l8 = next(level for level in super_yeti["levels"] if level["level"] == 8)
    assert (super_yeti["support"], super_yeti["attack_speed_seconds"], super_yeti_l8["hitpoints"], super_yeti_l8["damage_per_shot"]) == ("core_supported_secondary_partial", 1, 7400, 550)
    assert super_yeti_delta["known_uncertainties"][1].startswith("Public secondary pages conflict")
    super_witch = next(content for content in catalogue["contents"] if content["id"] == "super_witch")
    super_witch_l8 = next(level for level in super_witch["levels"] if level["level"] == 8)
    assert (super_witch["support"], super_witch["attack_speed_seconds"], super_witch_l8["hitpoints"], super_witch_l8["damage_per_shot"]) == ("core_supported_primary_secondary_partial", .7, 4100, 329)
    assert super_witch_delta["known_uncertainties"][0].startswith("No source gives")
    ice_hound = next(content for content in catalogue["contents"] if content["id"] == "ice_hound")
    ice_hound_l8 = next(level for level in ice_hound["levels"] if level["level"] == 8)
    ice_pup = next(content for content in catalogue["contents"] if content["id"] == "ice_pup")
    assert (ice_hound["support"], ice_hound["target_focus"], ice_hound_l8["hitpoints"], ice_hound_l8["damage_per_shot"], ice_hound_l8["spawned_units"]) == ("core_supported_primary_secondary_partial", "air_defenses", 11500, 50, 16)
    assert (ice_pup["support"], ice_pup["levels"][0]["hitpoints"], ice_pup["levels"][0]["damage_per_shot"]) == ("spawned_only", 50, 35)
    assert ice_hound_delta["known_uncertainties"][0].startswith("Neither primary source publishes")
    yeti = next(content for content in catalogue["contents"] if content["id"] == "yeti")
    yetimite = next(content for content in catalogue["contents"] if content["id"] == "yetimite")
    yeti_l8 = next(level for level in yeti["levels"] if level["level"] == 8)
    assert (yeti["support"], yeti["damage_spawn_threshold"], yeti_l8["hitpoints"], yeti_l8["spawned_units"]) == ("core_supported_primary_secondary_partial", 600, 4650, 14)
    assert (yetimite["support"], len(yetimite["levels"]), yetimite["levels"][-1]["hitpoints"], yetimite["levels"][-1]["damage_per_shot"]) == ("spawned_only", 8, 590, 92)
    assert yeti_delta["known_uncertainties"][0].startswith("No consulted source publishes")
    witch = next(content for content in catalogue["contents"] if content["id"] == "witch")
    witch_l8 = next(level for level in witch["levels"] if level["level"] == 8)
    assert (witch["support"], witch_l8["hitpoints"], witch_l8["damage_per_shot"], witch["attack_speed_seconds"]) == ("core_supported_primary_secondary_partial", 650, 182, .7)
    assert witch_delta["known_uncertainties"][0].startswith("No consulted source provides")
    lava_hound = next(content for content in catalogue["contents"] if content["id"] == "lava_hound")
    lava_hound_l8 = next(level for level in lava_hound["levels"] if level["level"] == 8)
    assert (lava_hound["support"], lava_hound["target_focus"], lava_hound_l8["hitpoints"], lava_hound_l8["damage_per_shot"]) == ("core_supported_primary_secondary_partial", "air_defenses", 9500, 48)
    assert lava_hound_delta["known_uncertainties"][0].startswith("No consulted source provides")
    headhunter_observation_bytes = HEADHUNTER_OBSERVATION.read_bytes()
    headhunter_observation = json.loads(headhunter_observation_bytes)
    headhunter_extra = next(item for item in FROZEN_REFERENCE["catalogue_extra_levels"]
                            if item["content_id"] == "headhunter" and item["level"] == 4)
    assert headhunter_extra["observation_file"] == HEADHUNTER_OBSERVATION.name
    assert headhunter_extra["observation_sha256"] == hashlib.sha256(headhunter_observation_bytes).hexdigest()
    headhunter = next(content for content in catalogue["contents"] if content["id"] == "headhunter")
    headhunter_l4 = next(level for level in headhunter["levels"] if level["level"] == 4)
    assert (headhunter["support"], headhunter["target_focus"], headhunter["hero_damage_multiplier"],
            headhunter_l4["hitpoints"], headhunter_l4["damage_per_shot"], headhunter_l4["dps"],
            headhunter_l4["hero_damage_per_shot"], headhunter_l4["poison_max_dps"],
            headhunter_l4["poison_movement_speed_reduction_percent"],
            headhunter_l4["poison_attack_speed_reduction_percent"]) == (
                "core_supported_primary_secondary_partial", "heroes", 4, 500, 81, 135, 540, 300, 46, 68)
    assert headhunter_l4["provenance"]["source"]["raw_sha256"] == headhunter_extra["observation_sha256"]
    assert headhunter_delta["known_uncertainties"][1].startswith("Heroes, pets and hero equipment")
    druid_observation_bytes = DRUID_OBSERVATION.read_bytes()
    druid_observation = json.loads(druid_observation_bytes)
    druid_extra = next(item for item in FROZEN_REFERENCE["catalogue_extra_levels"]
                       if item["content_id"] == "druid" and item["level"] == 6)
    assert druid_extra["observation_file"] == DRUID_OBSERVATION.name
    assert druid_extra["observation_sha256"] == hashlib.sha256(druid_observation_bytes).hexdigest()
    assert druid_observation["observations"][0]["fields"] == {
        "hitpoints": 1850, "healing_per_second": 105, "upgrade_operation": "research",
        "upgrade_cost": 380000, "upgrade_cost_resource": "Dark Elixir",
        "upgrade_time_seconds": 1339200, "town_hall_required": 18,
        "laboratory_required": 16, "hero_healing_per_hit": 120.75,
    }
    druid = next(content for content in catalogue["contents"] if content["id"] == "druid")
    druid_l6 = next(level for level in druid["levels"] if level["level"] == 6)
    assert (druid["support"], druid["heals"], druid["jumps_walls"],
            druid["druid_transform_after_seconds"], druid_l6["hitpoints"],
            druid_l6["healing_per_second"]) == (
                "core_supported_primary_secondary_partial", True, True, 30, 1850, 105)
    assert druid_l6["provenance"]["source"]["raw_sha256"] == druid_extra["observation_sha256"]
    druid_bear_observation_bytes = DRUID_BEAR_OBSERVATION.read_bytes()
    druid_bear_observation = json.loads(druid_bear_observation_bytes)
    druid_bear_supplement = next(item for item in FROZEN_REFERENCE["catalogue_supplemental_contents"]
                                 if item["content_id"] == "druid_bear")
    assert druid_bear_supplement["observation_file"] == DRUID_BEAR_OBSERVATION.name
    assert druid_bear_supplement["observation_sha256"] == hashlib.sha256(druid_bear_observation_bytes).hexdigest()
    assert druid_bear_observation["content"]["levels"][-1] == {
        "level": 6, "variant": "normal", "hitpoints": 2750,
        "damage_per_shot": 225, "dps": 225,
    }
    druid_bear = next(content for content in catalogue["contents"] if content["id"] == "druid_bear")
    assert (druid_bear["support"], druid_bear["target_focus"],
            druid_bear["levels"][-1]["hitpoints"], druid_bear["levels"][-1]["damage_per_shot"]) == (
                "spawned_only", "defenses", 2750, 225)
    assert druid_delta["known_uncertainties"][0].startswith("The original primary description")
    firemite_observation_bytes = FIREMITE_OBSERVATION.read_bytes()
    firemite_observation = json.loads(firemite_observation_bytes)
    firemite_supplement = next(item for item in FROZEN_REFERENCE["catalogue_supplemental_contents"]
                               if item["content_id"] == "firemite")
    assert firemite_supplement["observation_file"] == FIREMITE_OBSERVATION.name
    assert firemite_supplement["observation_sha256"] == hashlib.sha256(firemite_observation_bytes).hexdigest()
    furnace = next(content for content in catalogue["contents"] if content["id"] == "furnace")
    assert (furnace["support"], furnace["furnace_lifetime_seconds"],
            furnace["spawned_units"], furnace["spawned_unit_id"]) == (
                "data_only_primary_secondary", 60, [19, 20, 21, 22], "firemite")
    firemite = next(content for content in catalogue["contents"] if content["id"] == "firemite")
    assert (firemite["support"], firemite["range_tiles"], firemite["movement_speed"],
            firemite["levels"][-1]["hitpoints"], firemite["levels"][-1]["damage_per_shot"],
            firemite["levels"][-1]["persistent_flame_max_dps"]) == (
                "catalogued_only", 2.5, 32, 325, 80, 150)
    assert firemite_observation["content"]["levels"][0]["persistent_flame_duration_seconds"] == 10
    assert furnace_delta["known_uncertainties"][0].startswith("No consulted source publishes")
    meteor_golem = next(content for content in catalogue["contents"] if content["id"] == "meteor_golem")
    assert (meteor_golem["support"], meteor_golem["required_spawned_unit_id"],
            [level["hitpoints"] for level in meteor_golem["levels"]],
            [level["damage_per_shot"] for level in meteor_golem["levels"]]) == (
                "data_only_primary_secondary", "meteormite", [14500, 16000, 17500], [480, 530, 580])
    assert meteor_golem_delta["known_uncertainties"][1].startswith("No current build-pinned Meteormite")
    ruin_witch = next(content for content in catalogue["contents"] if content["id"] == "ruin_witch")
    ruin_knight = next(content for content in catalogue["contents"] if content["id"] == "ruin_knight")
    assert ruin_witch["partial_mechanics_contract"]["implementation_status"] == "data_only_no_rubble_actor_or_spawn_lifecycle"
    assert ruin_knight["partial_mechanics_contract"]["implementation_status"] == "data_only_requires_witch_spawn_contract"
    assert ruin_witch["field_provenance"]["partial_mechanics_contract"]["source"] == "supercell-2026-08-30-update"
    assert ruin_knight["field_provenance"]["partial_mechanics_contract"]["source"] == "supercell-2026-08-30-update"
    assert [(level["hitpoints"], level["reported_dps"])
            for level in ruin_witch_knight_current_delta["verified_values"]["ruin_witch"]["levels"]] == [(2300, 25), (2550, 26), (2800, 27), (3050, 28)]
    assert ruin_witch_knight_current_delta["verified_values"]["ruin_knight"] == {
        "required_spawned_child": True, "maximum_summoned": 10,
        "hitpoint_decay_per_second": 0, "level_mapping": "matches Ruin Witch level"}
    assert ruin_witch_knight_current_delta["known_uncertainties"][2].startswith("The displayed two-second")
    baby_dragon = next(content for content in catalogue["contents"] if content["id"] == "baby_dragon")
    assert (baby_dragon["support"], baby_dragon["isolation_radius_tiles"],
            baby_dragon["rage_damage_multiplier"], baby_dragon["rage_attack_speed_multiplier"]) == (
                "core_supported_primary_secondary_partial", 4.5, 2, 1.5)
    assert baby_dragon_delta["known_uncertainties"][0].startswith("No consulted source supplies")
    pekka = next(content for content in catalogue["contents"] if content["id"] == "pekka")
    pekka_l13 = next(level for level in pekka["levels"] if level["level"] == 13)
    assert (pekka["support"], pekka_l13["hitpoints"], pekka_l13["damage_per_shot"], pekka["attack_speed_seconds"]) == (
        "core_supported_secondary_partial", 8800, 1692, 1.8)
    assert pekka_delta["known_uncertainties"][0].startswith("No consulted source provides")
    goblin = next(content for content in catalogue["contents"] if content["id"] == "goblin")
    goblin_l10 = next(level for level in goblin["levels"] if level["level"] == 10)
    assert (goblin["support"], goblin["target_focus"], goblin["resource_damage_multiplier"], goblin_l10["hitpoints"], goblin_l10["damage_per_shot"]) == ("core_supported_secondary_partial", "resources", 2, 166, 82)
    assert goblin_delta["known_uncertainties"][0].startswith("No consulted source provides")
    minion = next(content for content in catalogue["contents"] if content["id"] == "minion")
    minion_l14 = next(level for level in minion["levels"] if level["level"] == 14)
    assert (minion["support"], minion["seeking_air_mine_immune"], minion_l14["hitpoints"], minion_l14["damage_per_shot"], minion_l14["upgrade_time_seconds"]) == ("core_supported_secondary_partial", True, 140, 92, 1209600)
    assert minion_delta["known_uncertainties"][0].startswith("The build-pinned Minion page")
    hog_rider = next(content for content in catalogue["contents"] if content["id"] == "hog_rider")
    hog_rider_l15 = next(level for level in hog_rider["levels"] if level["level"] == 15)
    assert (hog_rider["support"], hog_rider["target_focus"], hog_rider["jumps_walls"], hog_rider_l15["hitpoints"], hog_rider_l15["damage_per_shot"]) == ("core_supported_primary_secondary_partial", "defenses", True, 1750, 250)
    assert hog_rider_delta["known_uncertainties"][0].startswith("No consulted source defines")
    valkyrie = next(content for content in catalogue["contents"] if content["id"] == "valkyrie")
    valkyrie_l12 = next(level for level in valkyrie["levels"] if level["level"] == 12)
    assert (valkyrie["support"], valkyrie["splash_radius_tiles"], valkyrie_l12["hitpoints"], valkyrie_l12["damage_per_shot"]) == ("core_supported_primary_secondary_partial", 1, 2900, 459)
    assert valkyrie_delta["known_uncertainties"][1].startswith("The current build-pinned secondary L11")
    dragon_rider = next(content for content in catalogue["contents"] if content["id"] == "dragon_rider")
    dragon_rider_l6 = next(level for level in dragon_rider["levels"] if level["level"] == 6)
    assert (dragon_rider["support"], dragon_rider["target_focus"], dragon_rider["target_type"], dragon_rider_l6["hitpoints"], dragon_rider_l6["damage_per_shot"]) == ("core_supported_primary_secondary_partial", "defenses", "both", 6000, 612)
    assert dragon_rider_delta["known_uncertainties"][1].startswith("Goblins Farm reports a current L6 5,900 HP/490 DPS")
    miner = next(content for content in catalogue["contents"] if content["id"] == "miner")
    miner_l12 = next(level for level in miner["levels"] if level["level"] == 12)
    assert (miner["support"], miner["burrows"], miner_l12["hitpoints"], miner_l12["damage_per_shot"]) == ("core_supported_primary_secondary_partial", True, 2050, 331.5)
    assert miner_delta["known_uncertainties"][1].startswith("The build-pinned secondary source reports range")
    dragon = next(content for content in catalogue["contents"] if content["id"] == "dragon")
    dragon_l13 = next(level for level in dragon["levels"] if level["level"] == 13)
    assert (dragon["support"], dragon["range_tiles"], dragon_l13["hitpoints"], dragon_l13["damage_per_shot"]) == ("core_supported_secondary_partial", 2.5, 6000, 537.5)
    assert dragon_delta["known_uncertainties"][0].startswith("No consulted source gives splash radius")
    balloon = next(content for content in catalogue["contents"] if content["id"] == "balloon")
    balloon_l13 = next(level for level in balloon["levels"] if level["level"] == 13)
    assert (balloon["support"], balloon["splash_radius_tiles"], balloon_l13["hitpoints"],
            balloon_l13["damage_per_shot"], balloon_l13["death_damage"]) == (
                "core_supported_secondary_partial", 1.2, 1360, 978, 425)
    assert balloon_delta["known_uncertainties"][0].startswith("No consulted source defines bomb/drop projectile speed")
    golem = next(content for content in catalogue["contents"] if content["id"] == "golem")
    golemite = next(content for content in catalogue["contents"] if content["id"] == "golemite")
    golem_l15 = next(level for level in golem["levels"] if level["level"] == 15)
    golemite_l15 = next(level for level in golemite["levels"] if level["level"] == 15)
    assert (golem["support"], golem_l15["hitpoints"], golem_l15["damage_per_shot"],
            golem_l15["death_damage"], golemite["support"], golemite_l15["hitpoints"],
            golemite_l15["damage_per_shot"], golemite_l15["parent_spawn_count"]) == (
                "core_supported_primary_secondary_partial", 10600, 288, 1050,
                "spawned_only", 2040, 66, 4)
    assert golem_delta["known_uncertainties"][1].startswith("The current Golemite table is secondary")
    wizard = next(content for content in catalogue["contents"] if content["id"] == "wizard")
    wizard_l14 = next(level for level in wizard["levels"] if level["level"] == 14)
    assert (wizard["support"], wizard["splash_radius_tiles"], wizard_l14["hitpoints"],
            wizard_l14["damage_per_shot"], wizard_l14["dps"]) == (
                "core_supported_secondary_partial", 0.3, 330, 465, 310)
    assert wizard_delta["known_uncertainties"][0].startswith("No consulted source defines Wizard projectile speed")
    giant = next(content for content in catalogue["contents"] if content["id"] == "giant")
    giant_l14 = next(level for level in giant["levels"] if level["level"] == 14)
    assert (giant["support"], giant["target_focus"], giant_l14["hitpoints"],
            giant_l14["damage_per_shot"], giant_l14["dps"]) == (
                "core_supported_secondary_partial", "defenses", 3000, 228, 114)
    assert giant_delta["known_uncertainties"][0].startswith("No consulted source defines Giant route topology")
    archer = next(content for content in catalogue["contents"] if content["id"] == "archer")
    archer_l14 = next(level for level in archer["levels"] if level["level"] == 14)
    assert (archer["support"], archer["range_tiles"], archer_l14["hitpoints"], archer_l14["damage_per_shot"], archer_l14["dps"]) == ("core_supported_secondary_partial", 3.5, 76, 46, 46)
    assert archer_delta["known_uncertainties"][0].startswith("No consulted source defines Archer projectile speed")
    barbarian = next(content for content in catalogue["contents"] if content["id"] == "barbarian")
    barbarian_l13 = next(level for level in barbarian["levels"] if level["level"] == 13)
    assert (barbarian["support"], barbarian_l13["hitpoints"], barbarian_l13["damage_per_shot"], barbarian_l13["dps"]) == ("core_supported_primary_secondary_partial", 310, 51, 51)
    assert barbarian_delta["known_uncertainties"][0].startswith("No consulted source defines Barbarian route topology")
    healer = next(content for content in catalogue["contents"] if content["id"] == "healer")
    healer_l11 = next(level for level in healer["levels"] if level["level"] == 11)
    assert (healer["support"], healer["range_tiles"], healer_l11["hitpoints"], healer_l11["healing_per_second"]) == ("core_supported_secondary_partial", 4.5, 2100, 82)
    assert healer_delta["known_uncertainties"][0].startswith("No consulted source defines Healer target selection priority")
    wall_breaker = next(content for content in catalogue["contents"] if content["id"] == "wall_breaker")
    wall_breaker_l14 = next(level for level in wall_breaker["levels"] if level["level"] == 14)
    assert (wall_breaker["support"], wall_breaker_l14["hitpoints"], wall_breaker_l14["damage_per_shot"], wall_breaker_l14["dps"]) == ("core_supported_secondary_partial", 170, 126, 126)
    assert wall_breaker_delta["known_uncertainties"][0].startswith("No consulted source defines which Wall protects")
    electro_titan = next(content for content in catalogue["contents"] if content["id"] == "electro_titan")
    electro_titan_l5 = next(level for level in electro_titan["levels"] if level["level"] == 5)
    assert (electro_titan["support"], electro_titan_l5["hitpoints"], electro_titan_l5["damage_per_shot"], electro_titan_l5["dps"]) == ("core_supported_primary_secondary_partial", 9200, 405, 270)
    thrower = next(content for content in catalogue["contents"] if content["id"] == "thrower")
    thrower_l4 = next(level for level in thrower["levels"] if level["level"] == 4)
    assert (thrower["support"], thrower["range_tiles"], thrower["attack_speed_seconds"],
            thrower_l4["hitpoints"], thrower_l4["damage_per_shot"], thrower_l4["dps"]) == (
                "core_supported_primary_secondary_partial", 6, 2.5, 2800, 600, 240)
    assert thrower_delta["known_uncertainties"][0].startswith("No consulted source defines Thrower projectile")
    root_rider = next(content for content in catalogue["contents"] if content["id"] == "root_rider")
    root_rider_l4 = next(level for level in root_rider["levels"] if level["level"] == 4)
    assert (root_rider["support"], root_rider["target_focus"], root_rider["wall_damage_per_hit"],
            root_rider["wall_attack_speed_seconds"], root_rider_l4["hitpoints"],
            root_rider_l4["damage_per_shot"], root_rider_l4["dps"],
            root_rider_l4["laboratory_required"]) == (
                "core_supported_primary_secondary_partial", "defenses", 4000, .4, 6700, 275, 125, 16)
    assert root_rider_delta["known_uncertainties"][0].startswith("No consulted source fixes Root Rider route topology")
    sneaky_extra = next(item for item in FROZEN_REFERENCE["catalogue_extra_levels"]
                        if item["content_id"] == "sneaky_goblin" and item["level"] == 10)
    sneaky_observation_bytes = SNEAKY_GOBLIN_OBSERVATION.read_bytes()
    assert sneaky_extra["observation_file"] == SNEAKY_GOBLIN_OBSERVATION.name
    assert sneaky_extra["observation_sha256"] == hashlib.sha256(sneaky_observation_bytes).hexdigest()
    assert sneaky_extra["core_materializable"] is False
    sneaky_observation = json.loads(sneaky_observation_bytes)["observations"][0]
    assert sneaky_observation["fields"] == {"hitpoints": 380, "damage_per_shot": 190, "dps": 190, "upgrade_operation": "not_applicable", "upgrade_time_seconds": 0}
    sneaky = next(content for content in catalogue["contents"] if content["id"] == "sneaky_goblin")
    sneaky_l10 = next(level for level in sneaky["levels"] if level["level"] == 10)
    assert (sneaky["support"], sneaky["resource_damage_multiplier"], sneaky["defense_invisibility_duration_seconds"], sneaky_l10["hitpoints"], sneaky_l10["damage_per_shot"], sneaky_l10["dps"], sneaky_l10["core_materializable"]) == ("core_supported_primary_secondary_partial", 2, 5, 380, 190, 190, False)
    assert sneaky_goblin_delta["known_uncertainties"][0].startswith("The exact current L10 numeric values")
    air_sweeper = next(content for content in catalogue["contents"] if content["id"] == "air_sweeper")
    contract = air_sweeper["partial_mechanics_contract"]
    assert air_sweeper["support"] == "catalogued_only_partial_mechanics_contract"
    assert contract["implementation_status"] == "data_only_no_directional_displacement_contract"
    assert contract["catalogue"]["hitpoints_by_level"] == [750, 800, 850, 900, 950, 1000, 1050]
    assert contract["reported_mechanics"]["push_strength_tiles_by_level"] == [1.6, 2.0, 2.4, 2.8, 3.2, 3.6, 4.0]
    assert len(contract["not_materialized"]) >= 6
    assert air_sweeper_current_delta["conclusion"].startswith("Air Sweeper L1-L7 now has a current additive")
    spell_tower = next(content for content in catalogue["contents"] if content["id"] == "spell_tower")
    mode_parameters = {mode["id"]: mode for mode in spell_tower["modes"]}
    assert spell_tower["support"] == "catalogued_only_partial_mechanics_contract"
    assert (mode_parameters["rage"]["spell_radius_tiles"], mode_parameters["rage"]["spell_duration_seconds"], mode_parameters["rage"]["damage_increase_percent"], mode_parameters["rage"]["recharge_time_seconds"]) == (5, 18, 60, 70)
    assert (mode_parameters["poison"]["max_damage_per_second"], mode_parameters["poison"]["attack_rate_decrease_percent"], mode_parameters["poison"]["recharge_time_seconds"]) == (60, 25, 70)
    assert (mode_parameters["invisibility"]["spell_radius_tiles"], mode_parameters["invisibility"]["spell_duration_seconds"], mode_parameters["invisibility"]["recharge_time_seconds"]) == (4.5, 4.5, 50)
    assert (mode_parameters["earthquake"]["spell_radius_tiles"], mode_parameters["earthquake"]["troop_damage_percent"], mode_parameters["earthquake"]["recharge_time_seconds"]) == (4.7, 30, 50)
    assert spell_tower["partial_mechanics_contract"]["implementation_status"] == "data_only_mode_parameters_without_defensive_spell_lifecycle"
    assert spell_tower_current_delta["conclusion"].startswith("Spell Tower L1-L4 now has an additive")
    firespitter = next(content for content in catalogue["contents"] if content["id"] == "firespitter")
    contract = firespitter["partial_mechanics_contract"]
    assert firespitter["support"] == "catalogued_only_partial_mechanics_contract"
    assert contract["implementation_status"] == "data_only_no_directional_burst_projectile_contract"
    assert contract["levels"]["3"] == {"hitpoints": 5300, "dps": 455, "damage_per_shot": 51, "town_hall_required": 18}
    assert contract["reported_shared"]["shots_per_burst"] == 20
    assert contract["reported_shared"]["time_between_bursts_seconds"] == 1.0167
    assert firespitter_current_delta["conclusion"].startswith("Firespitter L1-L3 is recorded")
    builders_hut = next(content for content in catalogue["contents"] if content["id"] == "builders_hut")
    assert builders_hut["support"] == "catalogued_only_partial_mechanics_contract"
    builder_contract = builders_hut["partial_mechanics_contract"]
    assert builder_contract["l2_to_l8_hp_dps_repair_per_second"][-1] == [2100, 180, 95]
    assert builder_contract["builder"]["movement_speed"] == 20
    assert builder_contract["additional_builder_effectiveness_percent_by_total_count"] == [100, 100, 90, 90, 70]
    assert builders_hut_multi_builder_delta["conclusion"].startswith("Builder's Hut now retains the available multi-Builder")
    assert builders_hut_current_delta["conclusion"].startswith("Builder's Hut and its required Builder")
    ricochet = next(content for content in catalogue["contents"] if content["id"] == "ricochet_cannon")
    assert (ricochet["chain_damage_multiplier"], ricochet["chain_target_count"], ricochet["chain_radius_tiles"], ricochet["support"]) == (0.7, 2, 3.5, "core_supported_primary_secondary_partial")
    super_wizard_tower = next(content for content in catalogue["contents"] if content["id"] == "super_wizard_tower")
    assert (super_wizard_tower["chain_damage_multiplier"], super_wizard_tower["chain_target_count"], super_wizard_tower["chain_radius_tiles"], super_wizard_tower["support"]) == (0.4, 16, 4, "core_supported_primary_secondary_partial")
    revenge_tower = next(content for content in catalogue["contents"] if content["id"] == "revenge_tower")
    assert revenge_tower["support"] == "catalogued_only_partial_mechanics_contract"
    assert revenge_tower["partial_mechanics_contract"]["current_destroyed_building_thresholds"] == {"inactive": 0, "stage1": 5, "stage2": 25, "stage3": 50}
    multi_archer_tower = next(content for content in catalogue["contents"] if content["id"] == "multi_archer_tower")
    assert multi_archer_tower["support"] == "core_supported_primary_secondary_partial"
    multi_gear_tower = next(content for content in catalogue["contents"] if content["id"] == "multi_gear_tower")
    assert multi_gear_tower["support"] == "core_supported_primary_secondary_partial"
    assert [(mode["id"], mode["range_tiles"], mode["attack_speed_seconds"], mode["shots_per_burst"])
            for mode in multi_gear_tower["modes"]] == [("normal", 12, 1, 1), ("fastAttack", 8, 0.192, 4)]
    scattershot = next(content for content in catalogue["contents"] if content["id"] == "scattershot")
    assert scattershot["support"] == "core_supported_primary_secondary_partial"
    assert (scattershot["min_range_tiles"], scattershot["range_tiles"], scattershot["target_type"], scattershot["behavior"]["rear_cone_angle_degrees"]) == (3, 10, "both", 90)
    assert eagle_artillery_scope_delta["conclusion"].startswith("Standalone Eagle Artillery is excluded")
    town_hall = next(content for content in catalogue["contents"] if content["id"] == "town_hall")
    assert town_hall["support"] == "core_supported_primary_secondary_passive_partial"
    bomb_tower = next(content for content in catalogue["contents"] if content["id"] == "bomb_tower")
    assert bomb_tower["support"] == "core_supported_primary_secondary_partial"
    assert (bomb_tower["target_type"], bomb_tower["splash_radius_tiles"], bomb_tower["death_splash_radius_tiles"], bomb_tower["death_damage_delay_seconds"], bomb_tower["death_damage_ground_only"]) == ("ground", 1.5, 2.75, 1, True)
    assert bomb_tower_delta["conclusion"].startswith("Bomb Tower L1-L13 has a shared-Core")
    x_bow = next(content for content in catalogue["contents"] if content["id"] == "x_bow")
    charges = {(level["level"], level["variant"]): level for level in x_bow["levels"]}
    assert x_bow["support"] == "core_supported_primary_secondary_partial"
    assert [(mode["id"], mode["range_tiles"], mode["target_type"]) for mode in x_bow["modes"]] == [("normal", 14, "ground"), ("airAndGround", 11.5, "both")]
    assert [(charges[(level, "supercharged")]["hitpoints"], charges[(level, "supercharged")]["damage_per_shot"], charges[(level, "supercharged")]["dps"], charges[(level, "supercharged")]["town_hall_required"]) for level in (1, 2)] == [(5000, 32.64, 255, 18), (5100, 32.64, 255, 18)]
    assert x_bow["partial_mechanics_contract"]["ammunition_rounds"] == 1500
    assert x_bow_delta["conclusion"].startswith("X-Bow L1-L13 and TH18 Supercharge")
    inferno_tower = next(content for content in catalogue["contents"] if content["id"] == "inferno_tower")
    inferno_charges = {(level["level"], level["variant"]): level for level in inferno_tower["levels"]}
    assert inferno_tower["support"] == "core_supported_primary_secondary_partial"
    assert [(mode["id"], mode["range_tiles"], mode["target_type"]) for mode in inferno_tower["modes"]] == [("normal", 9, "both"), ("multiTarget", 10, "both")]
    assert [(inferno_charges[(level, "supercharged")]["hitpoints"], inferno_charges[(level, "supercharged")]["damage_per_shot"], inferno_charges[(level, "supercharged")]["dps"], inferno_charges[(level, "supercharged")]["town_hall_required"]) for level in (1, 2)] == [(5100, 448, 3500, 18), (5300, 448, 3500, 18)]
    assert [inferno_charges[(level, "supercharged")]["mode_stats"]["normal"]["dps_after_1p5_seconds"] for level in (1, 2)] == [350, 350]
    assert [inferno_charges[(level, "supercharged")]["mode_stats"]["multiTarget"]["number_of_targets"] for level in (1, 2)] == [6, 6]
    assert inferno_tower["partial_mechanics_contract"]["ammunition"]["uninterrupted_depletion_seconds"] == 128
    assert inferno_tower_delta["conclusion"].startswith("Inferno Tower L1-L12 and TH18 Supercharge")
    assert {source["id"] for source in ruin_witch_knight_followup_delta["sources"]} == {
        "supercell-2026-06-15-anime-fury-followup", "supercell-2026-08-30-ruin-witch-followup",
        "goblinsfarm-2026-09-20-ruin-witch-followup", "fandom-2026-09-20-ruin-witch-followup",
        "fandom-2026-09-20-ruin-knight-followup"}
    assert ruin_witch_knight_followup_delta["conclusion"].startswith("Fresh 20 September 2026 public-source review")
    assert furnace_firemite_followup_delta["conclusion"].startswith("Furnace L1-L4 and Firemite L1-L4 remain data-only")
    assert meteor_golem_meteormite_followup_delta["conclusion"].startswith("Meteor Golem L1-L3 and required Meteormite remain data-only")
    assert revenge_tower_followup_delta["conclusion"].startswith("The 20 September follow-up corroborates")
    assert crafting_station_passive_delta["conclusion"].startswith("Crafting Station L1 is now a source-backed passive destructible")
    assert electromite_followup_delta["conclusion"].startswith("The 20 September review corroborates")
    assert big_boy_followup_delta["conclusion"].startswith("The 20 September review corroborates")
    assert lava_hound_lava_pup_delta["conclusion"].startswith("Lava Hound L1-L8 and required Lava Pup L1")
    print("Active TH18 reference validation passed: all active deltas through the Monolith Arrow Passive override are hash-pinned")


if __name__ == "__main__":
    main()
