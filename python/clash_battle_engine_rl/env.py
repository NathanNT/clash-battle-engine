"""Gymnasium adapter for the deterministic Core ruleset."""
from __future__ import annotations
import gymnasium as gym
from gymnasium import spaces
import numpy as np
from _clash_battle_engine import NativeBattle, tick_ms

class ClashBattleEngineEnv(gym.Env):
    metadata = {"render_modes": []}
    def __init__(self):
        super().__init__()
        self.native = NativeBattle()
        self.action_space = spaces.Discrete(3)  # advance, wait next tick, end next tick
        self.observation_space = spaces.Dict({
            "time_ms": spaces.Box(np.int64(0), np.int64(np.iinfo(np.int64).max), shape=(), dtype=np.int64),
            "state_hash": spaces.Box(np.uint64(0), np.uint64(np.iinfo(np.uint64).max), shape=(), dtype=np.uint64),
        })
    def reset(self, *, seed=None, options=None):
        super().reset(seed=seed)
        self.native.reset(1 if seed is None else int(seed))
        if options and "scenario_json" in options:
            self.native.load_scenario_json(options["scenario_json"])
        return self._observation(), {}
    def step(self, action):
        before = self.native.observation()
        effective = before["time_ms"] + tick_ms
        if action == 1:
            self.native.wait_at(effective)
        elif action == 2:
            self.native.end_at(effective)
        elif action != 0:
            raise ValueError("unsupported action")
        self.native.advance_ticks(1)
        state = self.native.observation()
        return self._observation(), 0.0, state["result"] == "ended", state["result"] == "timed_out", state
    def _observation(self):
        state = self.native.observation()
        return {
            "time_ms": np.asarray(state["time_ms"], dtype=np.int64),
            "state_hash": np.asarray(state["state_hash"], dtype=np.uint64),
        }
    def snapshot(self):
        return self.native.snapshot()
    def restore(self, data):
        self.native.restore(data)
