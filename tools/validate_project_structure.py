#!/usr/bin/env python3
"""Guard the lightweight module and documentation conventions."""
from __future__ import annotations

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
CMAKE = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")

required = (
    "docs/README.md",
    "docs/architecture/overview.md",
    "state/README.md",
    "state/current.md",
    "state/backlog.md",
    "state/coverage-summary.md",
    "state/blockers.md",
    "state/decisions/README.md",
    "tests/README.md",
    "tools/dev.ps1",
    "tools/importer/README.md",
)
for relative in required:
    assert (ROOT / relative).is_file(), f"missing project navigation file: {relative}"

library = re.search(r"add_library\(cocsim_core STATIC(.*?)\)", CMAKE, re.DOTALL)
assert library, "cannot locate cocsim_core source list"
declared = set(re.findall(r"src/core/[A-Za-z0-9_./-]+\.cpp", library.group(1)))
actual = {
    path.relative_to(ROOT).as_posix()
    for path in (ROOT / "src" / "core").rglob("*.cpp")
}
assert declared == actual, (
    f"Core source list drift; undeclared={sorted(actual - declared)}, "
    f"missing={sorted(declared - actual)}"
)

viewer_target = re.search(r"add_executable\(cocsim_viewer\s(.*?)\)", CMAKE, re.DOTALL)
assert viewer_target, "cannot locate cocsim_viewer source list"
viewer_declared = set(re.findall(r"src/viewer/[A-Za-z0-9_./-]+\.cpp", viewer_target.group(1)))
viewer_actual = {
    path.relative_to(ROOT).as_posix()
    for path in (ROOT / "src" / "viewer").glob("*.cpp")
}
assert viewer_declared == viewer_actual, (
    f"Viewer source list drift; undeclared={sorted(viewer_actual - viewer_declared)}, "
    f"missing={sorted(viewer_declared - viewer_actual)}"
)

current_lines = (ROOT / "state" / "current.md").read_text(encoding="utf-8").splitlines()
assert len(current_lines) <= 120, "state/current.md must remain a short operational summary"

# These are orchestration files, not domains. Keep them short enough that a
# contributor can understand their complete control flow in one pass.
line_limits = {
    "src/core/core.cpp": 250,
    "src/viewer/main.cpp": 500,
    "tools/import_data.py": 400,
}
for relative, maximum in line_limits.items():
    lines = (ROOT / relative).read_text(encoding="utf-8").splitlines()
    assert len(lines) <= maximum, f"{relative} grew past its orchestration limit ({maximum})"

# Validators that inspect implementation details must discover all Core
# modules through project_layout instead of pinning the former monolith.
for path in (ROOT / "tools").glob("*.py"):
    if path.name in {"project_layout.py", "validate_project_structure.py"}:
        continue
    source = path.read_text(encoding="utf-8")
    assert '"core" / "core.cpp"' not in source, f"hard-coded Core monolith in {path.name}"

print(
    f"Project structure validation passed "
    f"({len(actual)} Core modules, {len(viewer_actual)} Viewer modules)"
)
