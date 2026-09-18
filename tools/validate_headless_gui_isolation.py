"""Prevent accidental GUI dependencies from slowing or changing headless runs."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CMAKE = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")

# The only executable used by headless/RL is linked to the core alone.  SDL,
# SDL_image and ImGui are declared inside the opt-in viewer branch below it.
assert re.search(r"add_executable\(cocsim\s+src/cli/main\.cpp\)\s*\n"
                 r"target_link_libraries\(cocsim\s+PRIVATE\s+cocsim_core\)", CMAKE)
viewer_branch = CMAKE.index("if(COCSIM_BUILD_VIEWER)")
headless_graph = re.sub(r"#.*", "", CMAKE[:viewer_branch])
# The option's user-facing help text names the viewer libraries, but no core or
# CLI target may consume one before the opt-in branch.
assert not re.search(r"target_(?:link_libraries|include_directories|sources)"
                     r"\(\s*(?:cocsim_core|cocsim)\b[^\n]*(?:SDL|imgui)",
                     headless_graph, re.IGNORECASE)

# The combat library is intentionally dependency-free.  This is stronger than
# a timing benchmark: a disabled GUI cannot create a renderer, load an asset,
# or insert a GUI call into a headless battle at all.
core_files = sorted((ROOT / "src/core").rglob("*.cpp"))
core_files += sorted((ROOT / "src/core").rglob("*.hpp"))
core_files += sorted((ROOT / "include/cocsim").rglob("*.hpp"))
for path in (*core_files, ROOT / "src/cli/main.cpp"):
    relative = path.relative_to(ROOT).as_posix()
    source = path.read_text(encoding="utf-8")
    assert not re.search(r"#\s*include\s*[<\"](?:SDL|imgui)", source, re.IGNORECASE), relative

# Projectile rendering state is presentation-only.  In particular, wall-clock
# trace expiry and SDL timing must never leak into future-influencing Core/RL
# state, where all combat timing is kTickMs based.
core_sources = {path.relative_to(ROOT).as_posix(): path.read_text(encoding="utf-8") for path in core_files}
core_sources["bindings/module.cpp"] = (ROOT / "bindings/module.cpp").read_text(encoding="utf-8")
for relative, source in core_sources.items():
    assert "ProjectileTrace" not in source, relative
    assert "SDL_GetTicks" not in source, relative
    assert "std::chrono" not in source, relative

viewer_main = (ROOT / "src/viewer/main.cpp").read_text(encoding="utf-8")
viewer_paths = [
    path for path in sorted((ROOT / "src/viewer").glob("*"))
    if path.suffix in {".cpp", ".hpp"} and path.name != "simulation_bridge.hpp"
]
viewer = "\n".join(path.read_text(encoding="utf-8") for path in viewer_paths)
bridge = (ROOT / "src/viewer/simulation_bridge.hpp").read_text(encoding="utf-8")
assert '#include "simulation_bridge.hpp"' in viewer_main
assert "viewer::SimulationBridge simulation(data, initial);" in viewer_main
assert "battle.observe_projectiles()" in bridge
assert "struct ProjectileTrace" in viewer
assert "ProjectileTraceKind::Healing" in viewer
assert "ProjectileTraceKind::SourcedImpact" in viewer
assert 'event.detail == "logical healing projectile"' in viewer
# Rendering receives values and the immutable scenario only; trace state is
# owned by the Viewer and cannot submit commands, advance ticks or otherwise
# mutate Core while drawing.
assert re.search(r"void draw_scene\(SDL_Renderer\* renderer, const Scenario& scenario", viewer)
draw_scene = viewer[viewer.index("void draw_scene("):viewer.index("} // namespace")]
assert "BattleState" not in draw_scene and "simulation.enqueue(" not in draw_scene
# Event geometry is authored by Core at the rule tick. The Viewer must not
# reconstruct it from a previous rendered entity frame.
assert "event.has_origin && event.has_target_position" in viewer
assert "event.projectile_id" in viewer and "sourced_projectile_ids" in viewer
assert "previous_positions" not in viewer
assert "has_target_position" in core_sources["include/cocsim/core.hpp"]
# The projectile's endpoint comes directly from the immutable Core projection,
# so the renderer's sampled entity list cannot hide its defense-to-target
# segment or inform a combat lookup.
assert "const Vec2 endpoint = projectile.target_position" in viewer
assert "target_position" in core_sources["include/cocsim/core.hpp"]
assert "projectile.speed_sourced ? SDL_Color{105, 225, 255, 235}" in viewer
# The rounded projectile marker is a renderer-only primitive. It receives
# projected coordinates, never a mutable battle object or wall-clock rule.
orb_renderer = viewer[viewer.index("void draw_projectile_orb("):viewer.index("void draw_scene(")]
assert "BattleState" not in orb_renderer and "SDL_RenderFillRect" in orb_renderer
assert viewer.count("draw_projectile_orb(renderer,") == 2
assert re.search(r"target_link_libraries\(_cocsim\s+PRIVATE\s+cocsim_core\)", CMAKE)

# The simulation bridge is the sole BattleState owner. It publishes a bounded
# triple buffer and assigns the next fixed rule boundary to every GUI command.
# GUI rendering therefore cannot block Core or pass a wall-clock command time.
assert "std::thread simulation_thread_" in bridge
assert "std::array<std::shared_ptr<const PresentationSnapshot>, 3> buffers_" in bridge
assert "command.requested_ms = battle.now() + kTickMs;" in bridge
assert "command.sequence = next_command_sequence_++;" in bridge
assert "next_command_sequence_{1}" in bridge
assert "reset_through_sequence_" in bridge
assert "return command.sequence <= discarded_through;" in bridge
assert "BattleState battle(data_, scenario_);" in bridge
assert "BattleState battle" not in viewer_main
# The arena is only hidden by the user's explicit toggle, never by rendering
# debt; a slow GUI continues to consume the newest immutable snapshot.
assert "const bool render_scene = draw_board;" in viewer
assert "board_resume_after_ms" not in viewer
assert "else ImGui::Dummy({42, 42});" in viewer
# Playback is a bridge-only presentation multiplier. It may change how many
# existing fixed ticks are advanced per wall-clock slice, never kTickMs itself
# or any Core/RL source file.
assert "kPlaybackSpeeds{1, 2, 4, 10}" in viewer
assert "presentation_debt_ms" in bridge
assert "simulation.set_playback_speed(playback_speed);" in viewer

print("headless GUI isolation validation passed")
