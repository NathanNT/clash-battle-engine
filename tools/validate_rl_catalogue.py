#!/usr/bin/env python3
"""Ensure the Python observation enum cannot reject a core-supported kind."""
from __future__ import annotations
import ast
import pathlib

from project_layout import registered_kind_ids

root = pathlib.Path(__file__).resolve().parents[1]
env = ast.parse((root / "python" / "cocsim_rl" / "env.py").read_text(encoding="utf-8"))
core_kinds = registered_kind_ids()
kinds = next(ast.literal_eval(node.value) for node in env.body if isinstance(node, ast.Assign) and any(isinstance(t, ast.Name) and t.id == "KINDS" for t in node.targets))
for node in env.body:
    if not (
        isinstance(node, ast.Expr)
        and isinstance(node.value, ast.Call)
        and isinstance(node.value.func, ast.Attribute)
        and isinstance(node.value.func.value, ast.Name)
        and node.value.func.value.id == "KINDS"
        and node.value.func.attr == "append"
        and len(node.value.args) == 1
        and not node.value.keywords
    ):
        continue
    kinds.append(ast.literal_eval(node.value.args[0]))
assert len(kinds) == len(set(kinds)), kinds
assert kinds == core_kinds, (kinds, core_kinds)
print("RL catalogue validation passed")
