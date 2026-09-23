"""Small Gymnasium adapter for the native deterministic engine.

Observation V0 is a padded float32 entity table with columns
[kind index, side, x, y, hp/max_hp, present], plus time remaining in info.
"""
from __future__ import annotations
import numpy as np
import gymnasium as gym
from gymnasium import spaces
from _cocsim import NativeBattle, tick_ms

# Keep this order exactly aligned with cocsim::Kind.  Scenarios supplied to
# reset(options={"scenario": ...}) may contain any currently catalogued core
# entity, not only the small default RL map.
KINDS = ["barbarian","archer","giant","cannon","archer_tower","mortar","wall","passive_building","town_hall","gold_mine","elixir_collector","wizard","wall_breaker","golem","pekka","balloon","dragon","healer","eagle_artillery","inferno_tower","goblin","minion","hog_rider","air_defense","wizard_tower","gold_storage","elixir_storage","dark_elixir_storage","dark_elixir_drill","army_camp","barracks","dark_barracks","spell_factory","dark_spell_factory","laboratory","workshop","blacksmith","hero_hall","pet_house","hero_banner","x_bow","valkyrie","dragon_rider","bomb_tower","hidden_tesla","bomb","giant_bomb","air_bomb","seeking_air_mine","spring_trap","giga_bomb","tornado_trap","golemite","skeleton_trap","trap_skeleton_ground","trap_skeleton_air","baby_dragon","miner","clan_castle","sneaky_goblin","root_rider","multi_archer_tower","multi_gear_tower","scattershot","thrower","electro_titan","bobs_hut","helper_hut","crafting_station","monolith","ricochet_cannon","super_wizard_tower","super_wall_breaker","super_barbarian"]
TROOPS = ["barbarian","archer","giant","wizard","wall_breaker","golem","pekka","balloon","dragon","baby_dragon","miner","healer","goblin","minion","hog_rider","valkyrie","dragon_rider","sneaky_goblin","root_rider","thrower","electro_titan","super_wall_breaker","super_barbarian"]
KINDS.append("super_giant")
TROOPS.append("super_giant")
KINDS.append("super_archer")
TROOPS.append("super_archer")
KINDS.append("rocket_balloon")
TROOPS.append("rocket_balloon")
KINDS.append("inferno_dragon")
TROOPS.append("inferno_dragon")
KINDS.append("super_wizard")
TROOPS.append("super_wizard")
KINDS.append("super_minion")
TROOPS.append("super_minion")
KINDS.append("super_bowler")
TROOPS.append("super_bowler")
KINDS.append("super_dragon")
TROOPS.append("super_dragon")
KINDS.append("electro_dragon")
TROOPS.append("electro_dragon")
KINDS.append("bowler")
TROOPS.append("bowler")
KINDS.append("ice_golem")
TROOPS.append("ice_golem")
KINDS.append("apprentice_warden")
TROOPS.append("apprentice_warden")
KINDS.append("super_hog_rider")
TROOPS.append("super_hog_rider")
KINDS.append("super_hog")
KINDS.append("super_rider")
KINDS.append("super_miner")
TROOPS.append("super_miner")
KINDS.append("super_valkyrie")
TROOPS.append("super_valkyrie")
KINDS.append("super_yeti")
TROOPS.append("super_yeti")
KINDS.append("super_witch")
TROOPS.append("super_witch")
KINDS.append("ice_hound")
TROOPS.append("ice_hound")
KINDS.append("ice_pup")
KINDS.append("yeti")
TROOPS.append("yeti")
KINDS.append("yetimite")
KINDS.append("witch")
TROOPS.append("witch")
KINDS.append("lava_hound")
KINDS.append("lava_pup")
KINDS.append("headhunter")
KINDS.append("druid")
KINDS.append("druid_bear")
KINDS.append("barbarian_king")
TROOPS.append("barbarian_king")
KINDS.append("archer_queen")
TROOPS.append("archer_queen")
KINDS.append("grand_warden")
TROOPS.append("grand_warden")
KINDS.append("royal_champion")
TROOPS.append("royal_champion")
KINDS.append("minion_prince")
TROOPS.append("minion_prince")
KINDS.append("dragon_duke")
TROOPS.append("dragon_duke")
TROOPS.append("lava_hound")
TROOPS.append("headhunter")
TROOPS.append("druid")
SPELLS = ["rage","heal","lightning","freeze","haste","jump","earthquake"]
class CoCSimEnv(gym.Env):
    metadata = {"render_modes": []}
    def __init__(self, decision_ms: int = 96, width: int = 20, height: int = 20,
                 max_entities: int = 128):
        if decision_ms <= 0 or decision_ms % tick_ms: raise ValueError(f"decision_ms must be a positive multiple of {tick_ms}")
        if width <= 0 or height <= 0 or max_entities <= 0: raise ValueError("width, height and max_entities must be positive")
        self.decision_ticks = decision_ms // tick_ms; self.width = width; self.height = height
        self.tile_count = width * height; self.max_entities = max_entities; self.native = NativeBattle()
        self.action_space = spaces.Discrete(1 + (len(TROOPS) + len(SPELLS)) * self.tile_count) # WAIT, then troop/spell and map tile.
        self.observation_space = spaces.Box(low=0, high=np.inf, shape=(max_entities, 6), dtype=np.float32)
    def _obs(self):
        out = np.zeros((self.max_entities,6),np.float32)
        entities = self.native.observation()["entities"]
        if len(entities) > self.max_entities:
            raise RuntimeError("observation capacity is too small; recreate CoCSimEnv with a larger max_entities")
        for i,e in enumerate(entities):
            out[i] = (KINDS.index(e["kind"]), e["side"] == "attacker", e["x"], e["y"], e["hp"] / e["max_hp"], 1)
        return out
    def reset(self, *, seed=None, options=None):
        super().reset(seed=seed); scenario_seed = int(self.np_random.integers(0, 2**63-1)) if seed is None else seed
        scenario = (options or {}).get("scenario")
        if scenario is None:
            if (self.width, self.height) != (20, 20):
                raise ValueError("a non-default arena requires options={'scenario': ...} with matching dimensions")
            self.native.reset(scenario_seed)
        else:
            if (scenario.get("width"), scenario.get("height")) != (self.width, self.height):
                raise ValueError("scenario dimensions must match the environment action grid")
            self.native.reset_scenario(scenario, scenario_seed)
        return self._obs(), {"action_mask": self.action_mask(), "observation_mode":"full_state_v0"}
    def action_mask(self):
        mask = np.zeros(self.action_space.n, dtype=np.int8); mask[0] = 1; available = self.native.availability()
        for index, kind in enumerate(TROOPS):
            if available["troops"].get(kind, 0): mask[1 + index * self.tile_count:1 + (index + 1) * self.tile_count] = 1
        offset = 1 + len(TROOPS) * self.tile_count
        for index, kind in enumerate(SPELLS):
            if available["spells"].get(kind, 0): mask[offset + index * self.tile_count:offset + (index + 1) * self.tile_count] = 1
        return mask
    def step(self, action):
        if not self.action_space.contains(action): raise ValueError("malformed action")
        invalid = False
        if action:
            q = action - 1; actor, q = divmod(q, self.tile_count); y, x = divmod(q, self.width)
            invalid = not (self.native.deploy(TROOPS[actor], x, y) if actor < len(TROOPS) else self.native.cast_spell(SPELLS[actor - len(TROOPS)], x, y))
        elapsed = self.native.advance(self.decision_ticks); r = self.native.result(); reward = r["destruction"] / 100.0
        return self._obs(), reward, bool(r["finished"] and not r["timed_out"]), bool(r["timed_out"]), {"invalid_action":invalid,"simulated_ms":elapsed,"action_mask":self.action_mask()}
