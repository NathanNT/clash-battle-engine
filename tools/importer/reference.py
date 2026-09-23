"""Frozen source inventory and explicit TH18 reference supplements."""

from __future__ import annotations

import hashlib
import json
import pathlib
import copy

FILES = {
    "barbarian": ("barbarian", "data/home/troops/barbarian.json"),
    "archer": ("archer", "data/home/troops/archer.json"),
    "giant": ("giant", "data/home/troops/giant.json"),
    "cannon": ("cannon", "data/home/defenses/cannon.json"),
    "archer_tower": ("archer-tower", "data/home/defenses/archer-tower.json"),
    "mortar": ("mortar", "data/home/defenses/mortar.json"),
    "wall": ("wall", "data/home/walls/wall.json"),
    "town_hall": ("town-hall", "data/home/town-hall/town-hall.json"),
    "gold_mine": ("gold-mine", "data/home/resource-buildings/gold-mine.json"),
    "elixir_collector": ("elixir-collector", "data/home/resource-buildings/elixir-collector.json"),
    "wizard": ("wizard", "data/home/troops/wizard.json"),
    "wall_breaker": ("wall-breaker", "data/home/troops/wall-breaker.json"),
    "golem": ("golem", "data/home/troops/golem.json"),
    "pekka": ("pekka", "data/home/troops/pekka.json"),
    "balloon": ("balloon", "data/home/troops/balloon.json"),
    "dragon": ("dragon", "data/home/troops/dragon.json"),
    "healer": ("healer", "data/home/troops/healer.json"),
    "eagle_artillery": ("eagle-artillery", "data/home/defenses/eagle-artillery.json"),
    "inferno_tower": ("inferno-tower", "data/home/defenses/inferno-tower.json"),
    "goblin": ("goblin", "data/home/troops/goblin.json"),
    "minion": ("minion", "data/home/troops/minion.json"),
    "hog_rider": ("hog-rider", "data/home/troops/hog-rider.json"),
    "air_defense": ("air-defense", "data/home/defenses/air-defense.json"),
    "wizard_tower": ("wizard-tower", "data/home/defenses/wizard-tower.json"),
    "x_bow": ("x-bow", "data/home/defenses/x-bow.json"),
    "valkyrie": ("valkyrie", "data/home/troops/valkyrie.json"),
    "dragon_rider": ("dragon-rider", "data/home/troops/dragon-rider.json"),
    "baby_dragon": ("baby-dragon", "data/home/troops/baby-dragon.json"),
    "miner": ("miner", "data/home/troops/miner.json"),
    "electro_dragon": ("electro-dragon", "data/home/troops/electro-dragon.json"),
    "yeti": ("yeti", "data/home/troops/yeti.json"),
    "witch": ("witch", "data/home/troops/witch.json"),
    "lava_hound": ("lava-hound", "data/home/troops/lava-hound.json"),
    "bowler": ("bowler", "data/home/troops/bowler.json"),
    "ice_golem": ("ice-golem", "data/home/troops/ice-golem.json"),
    "headhunter": ("headhunter", "data/home/troops/headhunter.json"),
    "electro_titan": ("electro-titan", "data/home/troops/electro-titan.json"),
    "root_rider": ("root-rider", "data/home/troops/root-rider.json"),
    "druid": ("druid", "data/home/troops/druid.json"),
    "furnace": ("furnace", "data/home/troops/furnace.json"),
    "meteor_golem": ("meteor-golem", "data/home/troops/meteor-golem.json"),
    "thrower": ("thrower", "data/home/troops/thrower.json"),
    "apprentice_warden": ("apprentice-warden", "data/home/troops/apprentice-warden.json"),
    "air_sweeper": ("air-sweeper", "data/home/defenses/air-sweeper.json"),
    "scattershot": ("scattershot", "data/home/defenses/scattershot.json"),
    "spell_tower": ("spell-tower", "data/home/defenses/spell-tower.json"),
    "monolith": ("monolith", "data/home/defenses/monolith.json"),
    "builders_hut": ("builders-hut", "data/home/defenses/builders-hut.json"),
    "firespitter": ("firespitter", "data/home/defenses/firespitter.json"),
    "ricochet_cannon": ("ricochet-cannon", "data/home/defenses/ricochet-cannon.json"),
    "multi_archer_tower": ("multi-archer-tower", "data/home/defenses/multi-archer-tower.json"),
    "multi_gear_tower": ("multi-gear-tower", "data/home/defenses/multi-gear-tower.json"),
    "super_wizard_tower": ("super-wizard-tower", "data/home/defenses/super-wizard-tower.json"),
    "revenge_tower": ("revenge-tower", "data/home/defenses/revenge-tower.json"),
    "crafting_station": ("crafting-station", "data/home/defenses/crafting-station.json"),
    "roaster": ("roaster", "data/home/crafted-defenses/roaster.json"),
    "air_bombs": ("air-bombs", "data/home/crafted-defenses/air-bombs.json"),
    "lava_launcher": ("lava-launcher", "data/home/crafted-defenses/lava-launcher.json"),
    "bomb_hive": ("bomb-hive", "data/home/crafted-defenses/bomb-hive.json"),
    "hero_bell": ("hero-bell", "data/home/crafted-defenses/hero-bell.json"),
    "light_beam": ("light-beam", "data/home/crafted-defenses/light-beam.json"),
    "bobs_hut": ("bobs-hut", "data/home/other/bobs-hut.json"),
    "helper_hut": ("helper-hut", "data/home/other/helper-hut.json"),
    "clan_castle": ("clan-castle", "data/home/resource-buildings/clan-castle.json"),
    "wall_wrecker": ("wall-wrecker", "data/home/siege-machines/wall-wrecker.json"),
    "battle_blimp": ("battle-blimp", "data/home/siege-machines/battle-blimp.json"),
    "stone_slammer": ("stone-slammer", "data/home/siege-machines/stone-slammer.json"),
    "siege_barracks": ("siege-barracks", "data/home/siege-machines/siege-barracks.json"),
    "log_launcher": ("log-launcher", "data/home/siege-machines/log-launcher.json"),
    "flame_flinger": ("flame-flinger", "data/home/siege-machines/flame-flinger.json"),
    "battle_drill": ("battle-drill", "data/home/siege-machines/battle-drill.json"),
    "troop_launcher": ("troop-launcher", "data/home/siege-machines/troop-launcher.json"),
    "barbarian_king": ("barbarian-king", "data/home/heroes/barbarian-king.json"),
    "archer_queen": ("archer-queen", "data/home/heroes/archer-queen.json"),
    "grand_warden": ("grand-warden", "data/home/heroes/grand-warden.json"),
    "royal_champion": ("royal-champion", "data/home/heroes/royal-champion.json"),
    "minion_prince": ("minion-prince", "data/home/heroes/minion-prince.json"),
    "dragon_duke": ("dragon-duke", "data/home/heroes/dragon-duke.json"),
    "smasher": ("smasher", "data/home/guardians/smasher.json"),
    "longshot": ("longshot", "data/home/guardians/longshot.json"),
    "lassi": ("lassi", "data/home/pets/lassi.json"),
    "electro_owl": ("electro-owl", "data/home/pets/electro-owl.json"),
    "mighty_yak": ("mighty-yak", "data/home/pets/mighty-yak.json"),
    "unicorn": ("unicorn", "data/home/pets/unicorn.json"),
    "diggy": ("diggy", "data/home/pets/diggy.json"),
    "poison_lizard": ("poison-lizard", "data/home/pets/poison-lizard.json"),
    "phoenix": ("phoenix", "data/home/pets/phoenix.json"),
    "frosty": ("frosty", "data/home/pets/frosty.json"),
    "spirit_fox": ("spirit-fox", "data/home/pets/spirit-fox.json"),
    "angry_jelly": ("angry-jelly", "data/home/pets/angry-jelly.json"),
    "sneezy": ("sneezy", "data/home/pets/sneezy.json"),
    "greedy_raven": ("greedy-raven", "data/home/pets/greedy-raven.json"),
    "bomb_tower": ("bomb-tower", "data/home/defenses/bomb-tower.json"),
    "hidden_tesla": ("hidden-tesla", "data/home/defenses/hidden-tesla.json"),
    "gold_storage": ("gold-storage", "data/home/resource-buildings/gold-storage.json"),
    "elixir_storage": ("elixir-storage", "data/home/resource-buildings/elixir-storage.json"),
    "dark_elixir_storage": ("dark-elixir-storage", "data/home/resource-buildings/dark-elixir-storage.json"),
    "dark_elixir_drill": ("dark-elixir-drill", "data/home/resource-buildings/dark-elixir-drill.json"),
    "army_camp": ("army-camp", "data/home/army-buildings/army-camp.json"),
    "barracks": ("barracks", "data/home/army-buildings/barracks.json"),
    "dark_barracks": ("dark-barracks", "data/home/army-buildings/dark-barracks.json"),
    "spell_factory": ("spell-factory", "data/home/army-buildings/spell-factory.json"),
    "dark_spell_factory": ("dark-spell-factory", "data/home/army-buildings/dark-spell-factory.json"),
    "laboratory": ("laboratory", "data/home/army-buildings/laboratory.json"),
    "workshop": ("workshop", "data/home/army-buildings/workshop.json"),
    "blacksmith": ("blacksmith", "data/home/army-buildings/blacksmith.json"),
    "hero_hall": ("hero-hall", "data/home/army-buildings/hero-hall.json"),
    "pet_house": ("pet-house", "data/home/army-buildings/pet-house.json"),
    "hero_banner": ("hero-banner", "data/home/army-buildings/hero-banner.json"),
    # Traps are catalogued from the same immutable snapshot.  Keep the entire
    # family here even where the combat behaviour is scheduled for a later
    # increment: importing data is deliberately distinct from claiming an
    # implementation in the coverage matrix.
    "bomb": ("bomb", "data/home/traps/bomb.json"),
    "giant_bomb": ("giant-bomb", "data/home/traps/giant-bomb.json"),
    "air_bomb": ("air-bomb", "data/home/traps/air-bomb.json"),
    "seeking_air_mine": ("seeking-air-mine", "data/home/traps/seeking-air-mine.json"),
    "spring_trap": ("spring-trap", "data/home/traps/spring-trap.json"),
    "skeleton_trap": ("skeleton-trap", "data/home/traps/skeleton-trap.json"),
    "tornado_trap": ("tornado-trap", "data/home/traps/tornado-trap.json"),
    "giga_bomb": ("giga-bomb", "data/home/traps/giga-bomb.json"),
}
# Hero equipment has no independent combat entity: its level records augment a
# selected hero and use a different schema. Keep the complete, explicit roster
# here so the immutable import cannot silently follow a mutable directory list.
HERO_EQUIPMENT = {
    identity: (identity.replace("_", "-"), f"data/home/hero-equipment/{identity.replace('_', '-')}.json")
    for identity in (
        "action_figure", "archer_puppet", "barbarian_puppet", "dark_crown", "dark_orb",
        "earthquake_boots", "electro_boots", "eternal_tome", "fire_heart", "fireball",
        "flame_blower", "frost_flake", "frozen_arrow", "giant_arrow", "giant_gauntlet",
        "haste_vial", "healer_puppet", "healing_tome", "henchmen_puppet", "heroic_torch",
        "hog_rider_puppet", "invisibility_vial", "lavaloon_puppet", "life_gem", "magic_mirror",
        "metal_pants", "meteor_staff", "noble_iron", "rage_gem", "rage_vial",
        "rocket_backpack", "rocket_spear", "royal_gem", "seeking_shield", "snake_bracelet",
        "spiky_ball", "stick_horse", "stun_blaster", "vampstache",
    )
}
FILES.update(HERO_EQUIPMENT)
TARGET_FOCUS = {"giant": "defenses", "golem": "defenses", "balloon": "defenses_only", "wall_breaker": "walls", "healer": "friendly_troops", "goblin": "resources", "hog_rider": "defenses", "dragon_rider": "defenses"}
FLYING = {"balloon", "dragon", "healer", "minion", "dragon_rider", "baby_dragon", "electro_dragon", "lava_hound"}
HEALERS = {"healer"}
# Public unit documentation specifies a 1-tile ground splash radius for the
# Valkyrie. The upstream structured record only identifies its damage type as
# `splash`, so retain this separately sourced geometric value explicitly.
SPLASH_RADIUS_TILES = {"valkyrie": 1.0}
DEATH_SPLASH_RADIUS_TILES = {"dragon_rider": 2.0}
REFERENCE_PATH = pathlib.Path(__file__).resolve().parents[2] / "data" / "reference" / "th18-2026-09-17.json"
# The active reference is an ordered chain, never an in-place mutation of a
# historical ledger.  Each new delta pins the exact bytes and identity of the
# preceding link, which makes subsequent TH18 corrections additive and
# reviewable instead of silently rewriting an earlier conclusion.
ACTIVE_DELTA_PATHS = (
    REFERENCE_PATH.with_name("th18-2026-09-18-monolith-l5.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-wall-breaker.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-barbarian.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-giant.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-archer.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-rocket-balloon.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-inferno-dragon.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-wizard.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-minion.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-bowler.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-dragon.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-x-bow-footprint.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-electro-dragon-chain.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-bowler-bounce.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-ice-golem-death-freeze.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-apprentice-warden-life-aura.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-hog-rider-split.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-miner.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-valkyrie.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-yeti-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-super-witch-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-ice-hound-ice-pup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-yeti-yetimite.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-witch-skeleton-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-lava-hound-lava-pup-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-headhunter-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-druid-bear-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-furnace-firemite-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-18-meteor-golem-meteormite-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-ruin-witch-knight-contract.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-baby-dragon-tantrum-contract.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-pekka-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-goblin-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-minion-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-hog-rider-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-valkyrie-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-dragon-rider-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-miner-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-electro-dragon-current-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-ruin-witch-knight-current-evidence.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-dragon-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-balloon-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-golem-golemite-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-wizard-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-giant-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-archer-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-barbarian-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-healer-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-wall-breaker-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-electro-titan-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-thrower-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-root-rider-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-sneaky-goblin-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-air-sweeper-current-evidence.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-spell-tower-current-evidence.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-firespitter-current-evidence.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-builders-hut-current-evidence.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-ricochet-cannon-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-super-wizard-tower-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-revenge-tower-current-evidence.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-multi-archer-tower-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-multi-gear-tower-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-scattershot-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-eagle-artillery-th18-scope.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-town-hall-th18-passive-scope.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-bomb-tower-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-19-x-bow-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-inferno-tower-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-ruin-witch-knight-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-furnace-firemite-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-meteor-golem-meteormite-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-revenge-tower-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-crafting-station-passive-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-electromite-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-big-boy-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-lava-hound-lava-pup-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-ruin-witch-movement-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-air-sweeper-mechanics-recheck.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-builders-hut-multi-builder-followup.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-furnace-firemite-lifecycle-recheck.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-spawned-subunit-assets-recheck.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-barbarian-king-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-archer-queen-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-grand-warden-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-royal-champion-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-minion-prince-baseline.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-dragon-duke-current-evidence.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-dragon-duke-base-attack.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-royal-champion-wall-crossing.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-hero-scope-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-hero-visual-icons.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-hero-banner-defense-recheck.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-flying-hero-defense-targetability.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-grand-warden-air-mode.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-hero-banner-visual-variants.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-hero-banner-assignment-official.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-hero-banner-th18-quantity.json"),
    REFERENCE_PATH.with_name("th18-2026-09-20-attacking-hero-slots.json"),
    REFERENCE_PATH.with_name("th18-2026-09-21-hero-battlefield-asset-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-21-hero-asset-rights-policy.json"),
    REFERENCE_PATH.with_name("th18-2026-09-21-hero-projectile-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-21-dragon-duke-trap-mitigation.json"),
    REFERENCE_PATH.with_name("th18-2026-09-21-hero-friendly-challenge-modifier-scope.json"),
    REFERENCE_PATH.with_name("th18-2026-09-21-hero-banner-defense-behavior-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-21-hero-collision-geometry-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-hero-defense-evidence-closure.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-meteor-golem-meteormite-evidence-recheck.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-hero-pet-equipment-scope.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-lassi-combat-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-electro-owl-level-and-behavior-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-mighty-yak-level-and-wall-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-unicorn-healing-and-lifecycle-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-frosty-frostmite-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-diggy-current-level-and-behavior-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-poison-lizard-level-and-effect-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-phoenix-revival-and-interaction-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-spirit-fox-invisibility-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-angry-jelly-brainwash-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-sneezy-booger-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-greedy-raven-resource-target-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-barbarian-puppet-summon-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-rage-vial-effect-order-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-earthquake-boots-ground-target-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-vampstache-heal-event-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-giant-gauntlet-current-effect-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-spiky-ball-bounce-and-count-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-snake-bracelet-spawn-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-stick-horse-deployment-and-wall-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-archer-puppet-summon-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-invisibility-vial-damage-and-target-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-giant-arrow-air-defense-damage-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-healer-puppet-summon-and-healing-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-frozen-arrow-slow-and-impact-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-magic-mirror-clone-and-invisibility-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-action-figure-giant-giant-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-monolith-arrow-housing-and-projectile-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-henchmen-puppet-and-child-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-dark-orb-projectile-and-slow-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-metal-pants-barrier-and-recovery-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-noble-iron-opening-shots-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-dark-crown-defeat-threshold-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-meteor-staff-target-and-cadence-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-eternal-tome-aura-immunity-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-life-gem-balance-and-aura-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-rage-gem-aura-and-stacking-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-healing-tome-aura-and-pulse-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-fireball-target-size-and-splash-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-lavaloon-puppet-and-child-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-heroic-torch-balance-and-wall-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-royal-gem-recovery-and-bonus-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-seeking-shield-target-and-bounce-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-hog-rider-puppet-and-child-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-haste-vial-cadence-and-speed-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-rocket-spear-charged-shots-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-electro-boots-aura-and-balance-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-frost-flake-freeze-and-target-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-fire-heart-balance-and-death-burst-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-flame-blower-direction-and-activation-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-stun-blaster-shockwave-and-target-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-electro-fangs-chain-and-target-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-rocket-backpack-dash-and-balance-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-hero-pet-equipment-attachment-contract-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-electro-fangs-passive-type-override.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-hero-battlefield-art-rights-audit.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-royal-gem-official-level-bonus-table.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-earthquake-boots-official-radius-and-damage-table.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-vampstache-official-dps-table.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-barbarian-puppet-official-bonus-and-spawn-rate.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-archer-puppet-official-bonus-and-invisibility-table.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-electro-boots-official-aura-and-healing-table.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-healing-tome-official-duration-table.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-healing-tome-primary-level-provenance.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-six-equipment-primary-level-provenance.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-royal-gem-active-heal-source-recheck.json"),
    REFERENCE_PATH.with_name("th18-2026-09-23-monolith-arrow-passive-type-override.json"),
)

def load_reference() -> dict:
    """Load the active reference chain without mutating its frozen parent.

    A post-freeze official level cannot be hidden in the historical ledger.  The
    active import therefore merges explicit additive deltas after checking the
    parent's identity and byte hash.  Observation files remain relative to the
    parent reference directory, so every payload retains a stable local path.
    """
    parent_bytes = REFERENCE_PATH.read_bytes()
    active = json.loads(parent_bytes)
    prior_path = REFERENCE_PATH
    prior_bytes = parent_bytes
    chain = [active["reference_id"]]
    links = []
    for delta_path in ACTIVE_DELTA_PATHS:
        delta_bytes = delta_path.read_bytes()
        delta = json.loads(delta_bytes)
        parent_link = delta["parent_reference"]
        if (parent_link["file"] != prior_path.name
                or parent_link["reference_id"] != active["reference_id"]
                or parent_link["sha256"] != hashlib.sha256(prior_bytes).hexdigest()):
            raise RuntimeError("active TH18 delta does not match its frozen parent")
        for key in ("sources", "official_deltas", "catalogue_extra_levels",
                    "catalogue_overrides", "catalogue_content_overrides",
                    "catalogue_supplemental_contents"):
            active.setdefault(key, []).extend(delta.get(key, []))
        for addition in delta.get("secondary_inventory_additions", []):
            inventory = next((entry for entry in active["secondary_inventory_indices"]
                              if entry["family"] == addition["family"]), None)
            if inventory is None or any(content_id in inventory["ids"] for content_id in addition["ids"]):
                raise RuntimeError("active TH18 inventory addition is missing its family or duplicates an id")
            inventory["ids"].extend(addition["ids"])
        active["reference_id"] = delta["reference_id"]
        active["frozen_at"] = delta["frozen_at"]
        active["status"] = "versioned_additive_delta"
        links.append({"file": delta_path.name, "parent": parent_link,
                      "reference_id": delta["reference_id"],
                      "sha256": hashlib.sha256(delta_bytes).hexdigest()})
        chain.append(delta["reference_id"])
        prior_path, prior_bytes = delta_path, delta_bytes
    active["reference_chain"] = {"root": {"file": REFERENCE_PATH.name,
                                              "reference_id": chain[0],
                                              "sha256": hashlib.sha256(parent_bytes).hexdigest()},
                                 "deltas": links, "ids": chain}
    return active

def load_catalogue_overrides(reference: dict) -> dict[tuple[str, int, str], dict]:
    """Load variant-aware level corrections from the frozen evidence ledger."""
    result: dict[tuple[str, int, str], dict] = {}
    for item in reference.get("catalogue_overrides", []):
        key = (item["content_id"], int(item["level"]), item.get("variant", "normal"))
        # Deltas may add independent sourced fields to an already-corrected
        # level. Merge them in chain order rather than discarding an earlier
        # field when a later contract contributes only one value.
        result.setdefault(key, {}).update({
            name: metadata["value"] for name, metadata in item["fields"].items()
        })
    return result

FROZEN_REFERENCE = load_reference()
CURRENT_LEVEL_OVERRIDES = load_catalogue_overrides(FROZEN_REFERENCE)

def load_content_overrides(reference: dict) -> dict[str, dict[str, dict]]:
    # Several immutable deltas may correct independent fields on one content.
    # Merge their field maps in reference-chain order so a newer additive
    # mechanics contract cannot erase an older, sourced numeric correction.
    result: dict[str, dict[str, dict]] = {}
    for entry in reference.get("catalogue_content_overrides", []):
        result.setdefault(entry["content_id"], {}).update(entry["fields"])
    return result

CONTENT_OVERRIDES = load_content_overrides(FROZEN_REFERENCE)

def apply_content_overrides(entry: dict, content_id: str) -> dict:
    # The compact Core JSON reader scans forward by field name. Keep the
    # concrete override fields ahead of their metadata block, otherwise a
    # similarly named provenance entry can shadow an object-valued contract
    # such as Inferno Dragon's `inferno_ramp`. Provenance remains unchanged
    # and is deliberately restored after every overridden field.
    provenance = entry.pop("field_provenance", {})
    for field, evidence in CONTENT_OVERRIDES.get(content_id, {}).items():
        entry[field] = evidence["value"]
        provenance[field] = {
            "source": evidence["source"], "note": evidence.get("note", "")}
    if provenance:
        entry["field_provenance"] = provenance
    return entry

def non_combat_content(reference: dict, content_id: str, source_id: str, category: str) -> dict:
    """Build a source-declared non-combat placement record.

    Hero Banner is intentionally not a zero-DPS combat building.  Keeping this
    route in the importer prevents a subsequent catalogue regeneration from
    silently turning its source-declared non-targetable state back into a
    generic attackable entity.
    """
    observation = next((item for item in reference.get("secondary_observations", [])
                        if item.get("id") == content_id), None)
    if observation is None:
        raise RuntimeError(f"missing non-combat observation: {content_id}")
    values = observation["values"]
    fields = ("targetable", "counts_toward_destruction", "deployment_margin_tiles")
    entry = {"id": content_id, "source_id": source_id, "category": category,
             "footprint_tiles": values["footprint_tiles"], "levels": [],
             "targetable": values["targetable"],
             "counts_toward_destruction": values["counts_toward_destruction"],
             "deployment_margin_tiles": values["deployment_margin_tiles"],
             "image": values["image"], "support": "catalogued_only",
             "field_provenance": {
                 field: {"source": observation["source"], "tier": "secondary"}
                 for field in fields}}
    entry["field_provenance"]["image"] = {"source": observation["image_source"], "tier": "secondary"}
    return apply_content_overrides(entry, content_id)

def extra_levels(reference: dict) -> dict[str, list[dict]]:
    """Load small secondary supplements only when the exact local artefact hashes.

    This is deliberately separate from field overrides: an absent source level
    needs a complete normalized record, not a mutation of the preceding level.
    """
    sources = {source["id"]: source for source in reference["sources"]}
    root = REFERENCE_PATH.parent
    loaded: dict[str, list[dict]] = {}
    for item in reference.get("catalogue_extra_levels", []):
        path = root / item["observation_file"]
        payload = path.read_bytes()
        digest = hashlib.sha256(payload).hexdigest()
        if digest != item["observation_sha256"]:
            raise RuntimeError(f"supplement hash mismatch: {path.name}")
        variant = item.get("variant", "normal")
        observation = next((entry for entry in json.loads(payload)["observations"]
                            if entry["content_id"] == item["content_id"]
                            and entry["level"] == item["level"]
                            and entry.get("variant", "normal") == variant), None)
        if observation is None or observation["source_id"] != item["source"]:
            raise RuntimeError(f"supplement entry mismatch: {item['content_id']} L{item['level']}")
        source = sources[item["source"]]
        provenance = {"tier": source["tier"], "source": item["source"], "url": source["url"],
                      "version": source.get("version", "unknown"), "raw_sha256": digest,
                      "consulted_at_utc": source["consulted_on"] + "T00:00:00+00:00",
                      "pinned_observation": item["observation_file"]}
        level = {"level": item["level"], "variant": variant, "damage_per_shot": None,
                 "damage": None, "damage_radius_tiles": None, "dps": None,
                 "healing_per_second": None, "death_damage": None, "spawned_units": None,
                 "spawned_unit_level": None, "image": None,
                 "upgrade_operation": "build", "laboratory_required": None, "xp_gained": None,
                 "capacity": None, "production_rate": None, "max_buildings": None,
                 "max_traps": None, "storage_capacity": None, "wall_rings": None,
                 "weapon": None, "mode_stats": {}}
        imported_fields = dict(observation["fields"])
        # A page may publish DPS but not per-hit damage.  The supplemental
        # record can carry its explicit, reviewed derivation from an unchanged
        # catalogue cadence; keep the derivation alongside the raw observation.
        for field, derivation in observation.get("derived_fields", {}).items():
            imported_fields[field] = derivation["value"]
        level.update(imported_fields)
        level["core_materializable"] = item.get("core_materializable", True)
        level["provenance"] = {"level": item["level"], "source": provenance,
                               "overridden_fields": sorted(imported_fields)}
        loaded.setdefault(item["content_id"], []).append(level)
    return loaded

EXTRA_LEVELS = extra_levels(FROZEN_REFERENCE)

def supplemental_contents(reference: dict) -> list[dict]:
    """Import data-only content from a separately hashed observation.

    This covers spawned entities and current Home Village entries absent from
    the frozen upstream tree.  Such records remain explicit catalogue data;
    their presence never licenses a guessed Core actor or lifecycle.
    """
    sources = {source["id"]: source for source in reference["sources"]}
    root = REFERENCE_PATH.parent
    loaded: list[dict] = []
    for item in reference.get("catalogue_supplemental_contents", []):
        path = root / item["observation_file"]
        payload = path.read_bytes()
        digest = hashlib.sha256(payload).hexdigest()
        if digest != item["observation_sha256"]:
            raise RuntimeError(f"supplement content hash mismatch: {path.name}")
        observation = json.loads(payload)
        if observation.get("source_id") != item["source"]:
            raise RuntimeError(f"supplement content source mismatch: {item['content_id']}")
        content = dict(observation.get("content", {}))
        if content.get("id") != item["content_id"]:
            raise RuntimeError(f"supplement content id mismatch: {item['content_id']}")
        source = sources[item["source"]]
        provenance = {"tier": source["tier"], "source": item["source"], "url": source["url"],
                      "version": source.get("version", "unknown"), "raw_sha256": digest,
                      "consulted_at_utc": source["consulted_on"] + "T00:00:00+00:00",
                      "pinned_observation": item["observation_file"]}
        levels = []
        for source_level in content.get("levels", []):
            level = dict(source_level)
            level.setdefault("healing_per_second", None)
            level.setdefault("image", None)
            level.setdefault("upgrade_operation", "not_applicable")
            level.setdefault("upgrade_cost", None)
            level.setdefault("upgrade_cost_resource", None)
            level.setdefault("upgrade_time_seconds", None)
            level.setdefault("town_hall_required", None)
            level.setdefault("laboratory_required", None)
            level.setdefault("xp_gained", None)
            level.setdefault("capacity", None)
            level.setdefault("production_rate", None)
            level.setdefault("max_buildings", None)
            level.setdefault("max_traps", None)
            level.setdefault("storage_capacity", None)
            level.setdefault("wall_rings", None)
            level.setdefault("weapon", None)
            level.setdefault("mode_stats", {})
            level["provenance"] = {"level": level["level"], "source": provenance,
                                   "overridden_fields": sorted(key for key in source_level if key != "level")}
            levels.append(level)
        content["levels"] = levels
        loaded.append(content)
    return loaded

SUPPLEMENTAL_CONTENTS = supplemental_contents(FROZEN_REFERENCE)

def supplemental_spells(reference: dict) -> list[dict]:
    """Import a non-upstream spell table from a hashed observation artefact.

    This is intentionally distinct from ``SPELLS``: no guessed upstream path is
    introduced when a current Home Village spell is absent from the frozen raw
    snapshot.  A supplemental spell may be visible in the data catalogue while
    still being unavailable to ``cocsim_core``; ``support`` makes that boundary
    explicit instead of turning incomplete timing evidence into gameplay.
    """
    sources = {source["id"]: source for source in reference["sources"]}
    root = REFERENCE_PATH.parent
    loaded: list[dict] = []
    for item in reference.get("catalogue_supplemental_spells", []):
        path = root / item["observation_file"]
        payload = path.read_bytes()
        digest = hashlib.sha256(payload).hexdigest()
        if digest != item["observation_sha256"]:
            raise RuntimeError(f"supplement spell hash mismatch: {path.name}")
        observation = json.loads(payload)
        if observation.get("source_id") != item["source"]:
            raise RuntimeError(f"supplement spell source mismatch: {item['spell_id']}")
        spell = dict(observation.get("spell", {}))
        if spell.get("id") != item["spell_id"]:
            raise RuntimeError(f"supplement spell id mismatch: {item['spell_id']}")
        source = sources[item["source"]]
        provenance = {
            "tier": source["tier"], "source": item["source"], "url": source["url"],
            "version": source.get("version", "unknown"), "raw_sha256": digest,
            "consulted_at_utc": source["consulted_on"] + "T00:00:00+00:00",
            "pinned_observation": item["observation_file"],
        }
        levels = []
        for observed_level in spell.get("levels", []):
            level = dict(observed_level)
            if not isinstance(level.get("level"), int):
                raise RuntimeError(f"supplement spell level is invalid: {item['spell_id']}")
            level["provenance"] = {
                "level": level["level"], "source": provenance,
                "overridden_fields": sorted(key for key in observed_level if key != "level"),
            }
            levels.append(level)
        if not levels:
            raise RuntimeError(f"supplement spell has no levels: {item['spell_id']}")
        spell["levels"] = sorted(levels, key=lambda level: level["level"])
        spell.setdefault("support", "catalogued_only")
        loaded.append(spell)
    return loaded

SUPPLEMENTAL_SPELLS = supplemental_spells(FROZEN_REFERENCE)
SPELLS = {
    "rage": ("rage-spell", "data/home/spells/rage-spell.json"),
    "heal": ("healing-spell", "data/home/spells/healing-spell.json"),
    "lightning": ("lightning-spell", "data/home/spells/lightning-spell.json"),
    "freeze": ("freeze-spell", "data/home/spells/freeze-spell.json"),
    "haste": ("haste-spell", "data/home/spells/haste-spell.json"),
    "jump": ("jump-spell", "data/home/spells/jump-spell.json"),
    "earthquake": ("earthquake-spell", "data/home/spells/earthquake-spell.json"),
    "poison": ("poison-spell", "data/home/spells/poison-spell.json"),
    "invisibility": ("invisibility-spell", "data/home/spells/invisibility-spell.json"),
    "clone": ("clone-spell", "data/home/spells/clone-spell.json"),
    "skeleton": ("skeleton-spell", "data/home/spells/skeleton-spell.json"),
    "bat": ("bat-spell", "data/home/spells/bat-spell.json"),
    "recall": ("recall-spell", "data/home/spells/recall-spell.json"),
    "overgrowth": ("overgrowth-spell", "data/home/spells/overgrowth-spell.json"),
    "revive": ("revive-spell", "data/home/spells/revive-spell.json"),
    "totem": ("totem-spell", "data/home/spells/totem-spell.json"),
    "ice_block": ("ice-block-spell", "data/home/spells/ice-block-spell.json"),
}
BASE = "https://raw.githubusercontent.com/chiefpansancolt/clash-of-clans-data/{commit}/"
