#include "catalogue_panel.hpp"
#include "loadout_editor.hpp"
#include "demo_scenario.hpp"
#include "replay_io.hpp"
#include "render.hpp"
#include "selection.hpp"
#include "simulation_bridge.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace cocsim;
using namespace cocsim::viewer;

int main(int argc, char** argv) {
  if (!SDL_Init(SDL_INIT_VIDEO)) { std::fprintf(stderr, "SDL init: %s\n", SDL_GetError()); return 1; }
  SDL_Window* window = SDL_CreateWindow("CoCSim viewer", 1280, 920, SDL_WINDOW_RESIZABLE);
  SDL_Renderer* renderer = window ? SDL_CreateRenderer(window, nullptr) : nullptr;
  if (!renderer) { std::fprintf(stderr, "SDL renderer: %s\n", SDL_GetError()); if (window) SDL_DestroyWindow(window); SDL_Quit(); return 1; }
  SDL_SetRenderVSync(renderer, 1);
  // Show a real frame before touching the catalogue.  The catalogue is
  // intentionally loaded synchronously and then frozen for the whole battle,
  // so this does not change a rule or a timing decision; it merely makes a
  // cold filesystem/cache start visibly responsive instead of looking hung.
  SDL_SetWindowTitle(window, "CoCSim | chargement du catalogue...");
  SDL_SetRenderDrawColor(renderer, 24, 29, 37, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
  IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImGui::StyleColorsDark();
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer); ImGui_ImplSDLRenderer3_Init(renderer);

  Scenario initial = demo_scenario();
  std::vector<Command> initial_replay_commands;
  std::string initial_replay_path;
  bool replay_mode = false;
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "--scenario") {
      if (i + 1 == argc) { std::fprintf(stderr, "--scenario requires a JSON path\n"); return 2; }
      std::string error;
      if (!load_scenario(argv[++i], initial, error)) {
        std::fprintf(stderr, "invalid scenario: %s\n", error.c_str());
        return 2;
      }
      initial_replay_commands.clear();
      initial_replay_path.clear();
      replay_mode = false;
    } else if (std::string(argv[i]) == "--replay") {
      if (i + 1 == argc) { std::fprintf(stderr, "--replay requires a JSON path\n"); return 2; }
      std::string error;
      if (!load_replay(argv[++i], initial, initial_replay_commands, error)) {
        std::fprintf(stderr, "invalid replay: %s\n", error.c_str());
        return 2;
      }
      initial_replay_path = argv[i];
      replay_mode = true;
    }
  }
  // The normalized catalogue is deliberately comprehensive (including
  // provenance) and is parsed by the core before a battle starts.  It can be
  // noticeably expensive on a cold disk.  Loading it on a worker leaves the
  // window responsive; no BattleState exists yet, so wall-clock time here
  // cannot affect a combat command or a fixed simulation tick.
  const auto catalogue_load_started = SDL_GetTicks();
  auto catalogue_future = std::async(std::launch::async, [] { return GameData::v0(); });
  for (;;) {
    SDL_Event loading_event;
    bool cancelled = false;
    while (SDL_PollEvent(&loading_event)) {
      ImGui_ImplSDL3_ProcessEvent(&loading_event);
      cancelled = cancelled || loading_event.type == SDL_EVENT_QUIT || loading_event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED;
    }
    if (cancelled) {
      ImGui_ImplSDLRenderer3_Shutdown(); ImGui_ImplSDL3_Shutdown(); ImGui::DestroyContext();
      SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
      return 0;
    }
    if (catalogue_future.wait_for(std::chrono::milliseconds{0}) == std::future_status::ready) break;
    SDL_SetRenderDrawColor(renderer, 24, 29, 37, 255);
    SDL_RenderClear(renderer);
    const float phase = float((SDL_GetTicks() / 12) % 160) / 160.0f;
    SDL_SetRenderDrawColor(renderer, 86, 162, 108, 255);
    SDL_FRect progress{360.0f, 445.0f, 560.0f * phase, 8.0f};
    SDL_RenderFillRect(renderer, &progress);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }
  GameData data;
  try { data = catalogue_future.get(); }
  catch (const std::exception& error) {
    std::fprintf(stderr, "catalogue load: %s\n", error.what());
    ImGui_ImplSDLRenderer3_Shutdown(); ImGui_ImplSDL3_Shutdown(); ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
    return 1;
  }
  const auto catalogue_load_ms = SDL_GetTicks() - catalogue_load_started;
  Images images{renderer, data}; images.set_root(initial_asset_root(data, argc, argv));
  // Asset decoding and transparent-bound scanning are deliberately deferred.
  // A large asset pack must not block the first responsive Viewer frame.
  images.queue_scene(initial);
  std::array<char, 1024> asset_root{};
  std::snprintf(asset_root.data(), asset_root.size(), "%s", images.root.c_str());
  std::array<char, 1024> replay_path{};
  std::snprintf(replay_path.data(), replay_path.size(), "%s", initial_replay_path.c_str());
  // The bridge starts the simulation thread after the immutable catalogue and
  // scenario have been fixed.  SDL/ImGui never receive a BattleState.
  auto simulation = std::make_unique<viewer::SimulationBridge>(data, initial, initial_replay_commands);
  // The GUI derives its selectable inventory from the core-validated Scenario.
  // This keeps its choices identical to headless and RL scenario inputs.
  std::vector<Kind> troops;
  for (const auto& slot : initial.army) troops.push_back(slot.kind);
  std::vector<SpellKind> spells;
  for (const auto& slot : initial.spells) spells.push_back(slot.kind);
  bool running = true, ranges = false, heatmap = false, heatmap_air = false, draw_board = true, adaptive_board = true, show_projectiles = true;
  std::uint64_t last_frame_work_ms = 0;
  int selected = 0, selected_spell = 0;
  bool selecting_spell = false;
  EntityId selected_defender{};
  float zoom = 16.0f;
  float map_origin_x = 460.0f, map_origin_y = 70.0f;
  std::uint64_t camera_previous_ms = SDL_GetTicks();
  // Troop swarms are the normal case where the viewer becomes draw-call
  // bound.  The read model is capped strictly below a full army; all omitted
  // entities still advance in BattleState and appear again as the swarm
  // shrinks.  The UI explicitly reports the omission.
  constexpr std::size_t kMinRenderedAttackers = 24;
  constexpr std::size_t kMaxRenderedAttackers = 120;
  std::size_t rendered_attacker_budget = kMaxRenderedAttackers;
  constexpr std::array<int, 4> kPlaybackSpeeds{1, 2, 4, 10};
  int custom_playback_speed = 1;
  bool maximum_playback = false;
  // Texture decoding remains a GUI-only budgeted task. It cannot block the
  // simulation thread or change its command/tick sequence.
  constexpr std::uint64_t kPreloadWorkBudgetMs = 16;
  HeatmapCache heatmap_cache;
  std::vector<ProjectileTrace> projectile_traces;
  // Identity comes from Core events, never from a GUI-side target lookup. It
  // lets a real moving projectile leave a short impact trace after the Core
  // has correctly removed it from the active read model.
  std::unordered_set<ProjectileId> sourced_projectile_ids;
  std::size_t viewed_event_count = 0;
  bool first_frame_presented = false;
  bool terminal_replay_saved = false;
  std::string terminal_replay_path;
  std::string terminal_replay_error;
  Scenario loadout_draft = initial; bool pending_replay_mode = true; std::optional<Scenario> pending_replay_scenario;
  std::vector<Command> pending_replay_commands;
  std::string replay_load_error;
  std::string loaded_replay_path = initial_replay_path;

  while (running) {
    const auto frame_start = SDL_GetTicks();
    if (pending_replay_scenario) {
      if (replace_simulation_bridge(simulation, data, *pending_replay_scenario, pending_replay_commands, replay_load_error)) {
        initial = std::move(*pending_replay_scenario);
        initial_replay_commands = std::move(pending_replay_commands);
        replay_mode = pending_replay_mode; loadout_draft = initial;
        troops.clear(); for (const auto& slot : initial.army) troops.push_back(slot.kind);
        spells.clear(); for (const auto& slot : initial.spells) spells.push_back(slot.kind);
        selected = 0; selected_spell = 0; selecting_spell = false; selected_defender = 0;
        viewed_event_count = 0; projectile_traces.clear(); sourced_projectile_ids.clear();
        terminal_replay_saved = false; terminal_replay_path.clear(); terminal_replay_error.clear();
        images.queue_scene(initial);
      }
      pending_replay_scenario.reset(); pending_replay_commands.clear();
    }
    auto frame = simulation->latest();
    if (!frame) continue;
    if (!replay_mode && frame->result.finished && !terminal_replay_saved) {
      terminal_replay_saved = true;
      terminal_replay_error.clear();
      if (!save_finished_viewer_replay(initial, frame->commands, terminal_replay_path, terminal_replay_error))
        terminal_replay_path.clear();
    }
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) running = false;
      const ImGuiIO& io = ImGui::GetIO();
      if (event.type == SDL_EVENT_MOUSE_WHEEL && !io.WantCaptureMouse) zoom = std::clamp(zoom + float(event.wheel.y) * 2, 8.0f, 28.0f);
      if (event.type == SDL_EVENT_KEY_DOWN && !io.WantCaptureKeyboard) {
        switch (event.key.key) { case SDLK_SPACE: simulation->set_paused(!simulation->paused()); break; case SDLK_N: simulation->request_step(); break; case SDLK_1: selected = 0; break; case SDLK_2: selected = 1; break; case SDLK_3: selected = 2; break; case SDLK_R: simulation->request_reset(); viewed_event_count = 0; projectile_traces.clear(); sourced_projectile_ids.clear(); selected_defender = 0; terminal_replay_saved = false; terminal_replay_path.clear(); terminal_replay_error.clear(); break; case SDLK_V: ranges = !ranges; break; case SDLK_H: heatmap = !heatmap; break; case SDLK_P: draw_board = !draw_board; break; default: break; }
      }
      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT && !io.WantCaptureMouse) {
        const float raw_x = (event.button.x - map_origin_x) / zoom, raw_y = (event.button.y - map_origin_y) / zoom;
        const bool in_map = raw_x >= 0 && raw_x < kHomeVillageTotalTiles && raw_y >= 0 && raw_y < kHomeVillageTotalTiles;
        const Vec2 position{std::floor(raw_x) + 0.5, std::floor(raw_y) + 0.5};
        // Shift-click is a read-only inspector.  It uses the actual Core
        // footprint in the immutable presentation frame and never becomes a
        // command, so it cannot interfere with deployment or rule timing.
        if (in_map && (SDL_GetModState() & SDL_KMOD_SHIFT)) {
          selected_defender = defender_at(frame->entities, {raw_x, raw_y}).value_or(EntityId{});
        } else if (!replay_mode && in_map && selecting_spell) {
          const auto kind = spells[static_cast<std::size_t>(selected_spell)];
          const auto* slot = spell_slot(frame->spells, kind);
          if (slot && slot->count > 0) simulation->enqueue({CommandType::CastSpell, Kind::Barbarian, slot->level, position, kind});
        } else if (!replay_mode && !troops.empty()) {
          const auto selected_kind = troops[static_cast<std::size_t>(selected)];
          const auto* selected_slot = army_slot(frame->army, selected_kind);
          // Placement validity belongs solely to cocsim_core. The viewer draws
          // the red margin as a hint, then submits the same command as RL/replay.
          if (in_map && selected_slot && selected_slot->count > 0) {
            simulation->enqueue({CommandType::Deploy, selected_kind, selected_slot->level, position, SpellKind::Rage});
          }
        }
      }
    }
    const auto now = SDL_GetTicks();
    const auto camera_elapsed_ms = now - camera_previous_ms;
    camera_previous_ms = now;
    if (!ImGui::GetIO().WantCaptureKeyboard) {
      const bool* keys = SDL_GetKeyboardState(nullptr);
      const float pan = static_cast<float>(camera_elapsed_ms) * 0.75f;
      if (keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_UP]) map_origin_y += pan;
      if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) map_origin_y -= pan;
      if (keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_LEFT]) map_origin_x += pan;
      if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) map_origin_x -= pan;
    }
    const auto playback_speed = static_cast<std::uint32_t>(custom_playback_speed);
    simulation->set_maximum_playback(maximum_playback);
    if (!maximum_playback) simulation->set_playback_speed(playback_speed);
    // At most one image is decoded per presentation cycle.  The simulation is
    // never gated on asset I/O: absent/unloaded art simply uses the existing
    // fallback until its source texture becomes available.
    // Let SDL present controls once before decoding/scanning the first sprite.
    // Large transparent source canvases are otherwise visible as a startup
    // stall even though sprite I/O is unrelated to the combat simulation.
    // Asset I/O is presentation-only. In particular it must not trigger the
    // adaptive catch-up path that used to clear the arena for one second.
    const bool render_scene = draw_board;
    const bool preload_this_frame = first_frame_presented && render_scene
      && last_frame_work_ms < kPreloadWorkBudgetMs;
    if (preload_this_frame)
      images.pump_preload();
    const auto& views = frame->entities;
    if (selected_defender != 0 && !defender_with_id(views, selected_defender))
      selected_defender = 0;
    std::vector<EntityView> render_views;
    const auto& spell_effects = frame->spell_effects;
    const auto& death_explosions = frame->death_explosions;
    const auto& projectile_views = frame->projectiles;
    std::size_t rendered_attackers = 0, omitted_attackers = 0;
    if (render_scene) {
      // GUI widgets and the renderer consume this one immutable snapshot.
      auto selected_entities = select_render_entities(views, rendered_attacker_budget);
      rendered_attackers = selected_entities.attackers_total - selected_entities.attackers_omitted;
      omitted_attackers = selected_entities.attackers_omitted;
      render_views = std::move(selected_entities.entities);
      const auto& events = frame->events;
      if (show_projectiles) {
        // Retain a bounded, clearly non-physical impact trace for one-tick
        // projectiles. Its endpoints are the immutable positions captured by
        // Core at the event tick, not a later frame lookup that can lose a
        // destroyed target or drift after an adaptive-rendering pause.
        const auto first = std::max(viewed_event_count, frame->first_event_index);
        for (std::size_t index = first; index < frame->event_count; ++index) {
          const auto& event = events[index - frame->first_event_index];
          const bool logical_healing = event.type == EventType::Projectile
            && event.detail == "logical healing projectile";
          const bool logical_projectile = event.type == EventType::Projectile
            && event.detail == "logical projectile";
          const bool sourced_launch = event.type == EventType::Projectile
            && event.detail == "seeking air mine launched" && event.projectile_id != 0;
          if (sourced_launch) sourced_projectile_ids.insert(event.projectile_id);
          const bool sourced_impact = event.type == EventType::Impact
            && event.detail == "projectile impact" && event.projectile_id != 0
            && sourced_projectile_ids.erase(event.projectile_id) != 0;
          const bool inferno_beam = event.type == EventType::Attack && event.detail == "inferno beam";
          if (!logical_projectile && !logical_healing && !sourced_impact && !inferno_beam) continue;
          const auto trace_kind = inferno_beam ? ProjectileTraceKind::Beam : sourced_impact ? ProjectileTraceKind::SourcedImpact
            : logical_healing ? ProjectileTraceKind::Healing : ProjectileTraceKind::LogicalImpact;
          if (event.has_origin && event.has_target_position) projectile_traces.push_back({
            event.origin, event.target_position, now,
            now + (uses_cosmetic_projectile_flight(trace_kind) ? kCosmeticProjectileTraceMs : 100), trace_kind
          });
        }
        projectile_traces.erase(std::remove_if(projectile_traces.begin(), projectile_traces.end(), [now](const ProjectileTrace& trace) { return trace.expires_wall_ms <= now; }), projectile_traces.end());
        constexpr std::size_t kMaxProjectileTraces = 128;
        if (projectile_traces.size() > kMaxProjectileTraces) projectile_traces.erase(projectile_traces.begin(), projectile_traces.end() - static_cast<std::ptrdiff_t>(kMaxProjectileTraces));
      } else {
        // There is no reason to retain a GUI-only event history when its
        // layer is hidden.  The battle/event history remains entirely in Core.
        projectile_traces.clear();
        sourced_projectile_ids.clear();
      }
      viewed_event_count = frame->event_count;
      if (heatmap) heatmap_cache.update(data, views, frame->deployed_housing, heatmap_air);
    } else {
      // The board can only be omitted by the explicit P toggle. Do not retain
      // presentation history while it is hidden; Core remains untouched.
      viewed_event_count = frame->event_count;
      projectile_traces.clear();
      sourced_projectile_ids.clear();
    }

    ImGui_ImplSDLRenderer3_NewFrame(); ImGui_ImplSDL3_NewFrame(); ImGui::NewFrame();
    ImGui::SetNextWindowPos({12, 12}, ImGuiCond_Always); ImGui::SetNextWindowSize({440, 0}, ImGuiCond_Always);
    ImGui::Begin("Army & controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text(replay_mode ? "Replay : commandes en lecture seule" : "Deploiement : clic hors des cadres rouges"); ImGui::Separator();
    for (std::size_t i = 0; i < troops.size(); ++i) {
      const Kind kind = troops[i]; const auto* slot = army_slot(frame->army, kind); const int count = slot ? slot->count : 0;
      ImGui::PushID(static_cast<int>(i));
      if (auto* texture = images.find(kind, slot ? slot->level : 1)) ImGui::Image((ImTextureID)texture, {42, 42});
      else ImGui::Dummy({42, 42});
      ImGui::SameLine();
      ImGui::BeginDisabled(replay_mode || count == 0);
      char label[80]; std::snprintf(label, sizeof(label), "%s L%d%s  x%d", to_string(kind).c_str(), slot ? slot->level : 0,
                                    slot && slot->mode != "normal" ? (slot->mode == "air" ? " (air)" : " (ground)") : "", count);
      if (ImGui::Selectable(label, !selecting_spell && selected == static_cast<int>(i), 0, {count ? 350.0f : 0.0f, 42.0f})) { selected = static_cast<int>(i); selecting_spell = false; }
      ImGui::EndDisabled(); ImGui::PopID();
    }
    ImGui::Separator(); ImGui::Text("Sorts : selectionner puis cliquer sur le village");
    for (std::size_t i = 0; i < spells.size(); ++i) {
      const auto kind = spells[i]; const auto* slot = spell_slot(frame->spells, kind); const int count = slot ? slot->count : 0;
      ImGui::PushID(100 + static_cast<int>(i)); ImGui::BeginDisabled(replay_mode || count == 0);
      char label[64]; std::snprintf(label, sizeof(label), "%s L%d  x%d", to_string(kind).c_str(), slot ? slot->level : 0, count);
      if (ImGui::Selectable(label, selecting_spell && selected_spell == static_cast<int>(i), 0, {400.0f, 26.0f})) { selected_spell = static_cast<int>(i); selecting_spell = true; }
      ImGui::EndDisabled(); ImGui::PopID();
    }
    ImGui::Separator();
    const bool paused = simulation->paused();
    if (ImGui::Button(paused ? "Resume (Space)" : "Pause (Space)")) simulation->set_paused(!paused);
    ImGui::SameLine(); if (ImGui::Button("One tick (N)")) simulation->request_step();
    ImGui::SameLine();
    ImGui::BeginDisabled(frame->result.finished || replay_mode);
    if (ImGui::Button("Finir le combat")) simulation->enqueue({CommandType::EndBattle});
    ImGui::EndDisabled();
    if (ImGui::Button(replay_mode ? "Rejouer depuis T+0 (R)" : "Reset (R)")) { simulation->request_reset(); viewed_event_count = 0; projectile_traces.clear(); sourced_projectile_ids.clear(); selected_defender = 0; terminal_replay_saved = false; terminal_replay_path.clear(); terminal_replay_error.clear(); }
    ImGui::SameLine(); ImGui::Checkbox("Ranges (V)", &ranges);
    ImGui::Checkbox("Heatmap degats (H)", &heatmap);
    if (heatmap) { ImGui::SameLine(); ImGui::Checkbox("Cible air", &heatmap_air); }
    ImGui::Checkbox("Afficher plateau (P)", &draw_board);
    ImGui::Checkbox("Projectiles", &show_projectiles);
    if (show_projectiles) {
      ImGui::TextDisabled("Cyan : vol source ; ambre : degats sans vitesse publiee ; vert : soin ; violet : rayon instantane");
      ImGui::Text("Projectiles Core actifs : %llu | traces GUI : %llu", static_cast<unsigned long long>(projectile_views.size()), static_cast<unsigned long long>(projectile_traces.size()));
    }
    ImGui::Checkbox("Rendu adaptatif 30 FPS", &adaptive_board);
    if (omitted_attackers > 0)
      ImGui::TextColored({1.0f, 0.72f, 0.25f, 1.0f}, "Troupes affichees : %llu / %llu (simulation complete)", static_cast<unsigned long long>(rendered_attackers), static_cast<unsigned long long>(rendered_attackers + omitted_attackers));
    ImGui::Text("Temps GUI precedent : %llu ms", static_cast<unsigned long long>(last_frame_work_ms));
    if (maximum_playback) ImGui::Text("Vitesse : instantanee (maximum CPU disponible)");
    else ImGui::Text("Vitesse : x%u (%u ticks/s, %u ms simules / ms reel)", playback_speed,
      100u * playback_speed, playback_speed);
    for (std::size_t i = 0; i < kPlaybackSpeeds.size(); ++i) {
      if (i != 0) ImGui::SameLine();
      char speed_label[8]; std::snprintf(speed_label, sizeof(speed_label), "x%d", kPlaybackSpeeds[i]);
      if (ImGui::Button(speed_label)) { custom_playback_speed = kPlaybackSpeeds[i]; maximum_playback = false; }
    }
    ImGui::SetNextItemWidth(115.0f);
    if (ImGui::InputInt("Vitesse personnalisee (x)", &custom_playback_speed, 1, 10)) {
      custom_playback_speed = std::clamp(custom_playback_speed, 1, 10'000);
      maximum_playback = false;
    }
    ImGui::SameLine();
    if (ImGui::Button(maximum_playback ? "Instantane actif" : "Instantane")) maximum_playback = !maximum_playback;
    ImGui::TextDisabled("ZQSD ou fleches : deplacer la camera");
    ImGui::Separator();
    ImGui::Text("Replay JSON");
    ImGui::SetNextItemWidth(300.0f); ImGui::InputText("##replay_path", replay_path.data(), replay_path.size()); ImGui::SameLine();
    if (ImGui::Button("Charger replay")) {
      ViewerReplay loaded;
      if (load_viewer_replay(replay_path.data(), loaded, replay_load_error)) {
        pending_replay_scenario = std::move(loaded.scenario);
        pending_replay_mode = true;
        pending_replay_commands = std::move(loaded.commands);
        loaded_replay_path = std::move(loaded.path);
        replay_load_error.clear();
      }
    }
    if (replay_mode) ImGui::TextColored({0.35f, 0.9f, 0.45f, 1.0f}, "Replay actif : %s", loaded_replay_path.c_str());
    else if (!replay_load_error.empty()) ImGui::TextColored({1.0f, 0.55f, 0.25f, 1.0f}, "Erreur replay : %s", replay_load_error.c_str());
    ImGui::Text("Catalogue charge en %llu ms", static_cast<unsigned long long>(catalogue_load_ms));
    draw_hero_support_catalogue(data, frame->hero_loadouts, frame->monolith_arrow_housing,
                                frame->monolith_arrow_tier, frame->monolith_arrow_damage_percent);
    if (!replay_mode && draw_hero_loadout_editor(data, loadout_draft)) { pending_replay_scenario = loadout_draft; pending_replay_commands.clear(); pending_replay_mode = false; replay_load_error.clear(); }
    ImGui::Separator();
    ImGui::Text("Destruction : %.1f%% | Etoiles : %d/3", frame->result.destruction, frame->result.stars);
    ImGui::Text("Valeur armee restante : %d logements | Unites : %d", frame->result.remaining_housing_space,
                frame->result.troops_remaining);
    ImGui::Text("Temps restant : %lld s",
                static_cast<long long>(frame->result.remaining_time_ms / 1000));
    if (frame->result.finished) ImGui::Text("Resultat : %s", frame->result.reason.c_str());
    if (frame->result.finished && !terminal_replay_path.empty())
      ImGui::TextColored({0.35f, 0.9f, 0.45f, 1.0f}, "Replay enregistre : %s", terminal_replay_path.c_str());
    else if (frame->result.finished && !terminal_replay_error.empty())
      ImGui::TextColored({1.0f, 0.55f, 0.25f, 1.0f}, "Erreur replay : %s", terminal_replay_error.c_str());
    for (const auto& entity : views) if (entity.kind == Kind::EagleArtillery && entity.side == Side::Defender) {
      const auto* eagle = data.find(entity.kind, entity.level);
      const int threshold = eagle ? eagle->activation_housing_space : 0;
      const int housing = frame->deployed_housing;
      const char* state = housing >= threshold ? "ACTIF" : housing >= 150 ? "pre-alerte" : housing >= 100 ? "alerte" : housing >= 50 ? "veille" : "dormant";
      const ImVec4 colour = housing >= threshold ? ImVec4{0.95f, 0.25f, 0.20f, 1.0f} : ImVec4{0.95f, 0.75f, 0.25f, 1.0f};
      ImGui::TextColored(colour, "Aigle : %s — logement deploye %d / %d", state, housing, threshold);
      break;
    }
    if (!selecting_spell && !troops.empty()) {
      const auto selected_kind = troops[static_cast<std::size_t>(selected)];
      const auto* selected_slot = army_slot(frame->army, selected_kind);
      ImGui::BeginDisabled(replay_mode || !selected_slot || selected_slot->count == 0);
      if (ImGui::Button("Deployer a gauche")) {
        simulation->enqueue({CommandType::Deploy, selected_kind, selected_slot->level, {0.5, 25.5}, SpellKind::Rage});
      }
      ImGui::EndDisabled();
      if (selected_slot) if (const auto* stats = data.find(selected_kind, selected_slot->level)) {
        ImGui::Separator();
        ImGui::Text("Niveau %d : PV %.0f, degats %.1f, DPS %.1f", stats->level, stats->hp, stats->damage, stats->dps);
        ImGui::Text("Cible : %s%s%s", to_string(stats->target_focus).c_str(), stats->flying ? " | volant" : "", stats->heals ? " | soigne" : "");
        if (stats->heals) ImGui::SameLine(), ImGui::Text("%.1f PV/s", stats->healing_per_second);
        ImGui::Text("HV %d, labo %d, cout %.0f %s", stats->attributes.town_hall_required, stats->attributes.laboratory_required, stats->attributes.upgrade_cost, stats->attributes.upgrade_cost_resource.c_str());
      }
    } else {
      const auto kind = spells[static_cast<std::size_t>(selected_spell)]; const auto* slot = spell_slot(frame->spells, kind);
      if (slot) if (const auto* stats = data.find_spell(kind, slot->level)) {
        ImGui::Separator(); ImGui::Text("%s L%d : rayon %.1f", to_string(kind).c_str(), stats->level, stats->radius);
        if (kind == SpellKind::Lightning) ImGui::Text("%.0f degats instantanes", stats->damage);
        else if (kind == SpellKind::Heal) ImGui::Text("%.0f PV au total", stats->total_healing);
        else if (kind == SpellKind::Earthquake) ImGui::Text("%.1f%% batiments, %.1f%% troupes sol", stats->building_damage_percent, stats->troop_damage_percent);
        else ImGui::Text("Duree %.1f s", double(stats->duration) / 1000.0);
      }
    }
    if (const auto* inspected = defender_with_id(views, selected_defender)) {
      ImGui::Separator();
      ImGui::Text("Selection (Shift+clic) : %s L%d", to_string(inspected->kind).c_str(), inspected->level);
      ImGui::Text("PV %.0f / %.0f | emprise %dx%d", inspected->hp, inspected->max_hp,
                  inspected->footprint_width, inspected->footprint_height);
      if (const auto* stats = data.find(inspected->kind, inspected->level,
                                        inspected->supercharged ? "supercharged" : "normal")) {
        ImGui::Text("Portee %.1f | DPS %.1f | cible %s", stats->range, stats->dps,
                    to_string(stats->target_focus).c_str());
      }
    } else {
      ImGui::TextDisabled("Shift+clic sur un defenseur : emprise, portee et statistiques Core.");
    }
    ImGui::Text("Zoom: %.0f px/tile", zoom); ImGui::Text("T + %lld ms", static_cast<long long>(frame->now_ms));
    ImGui::Text("Sprites: %d charges, %d manquants", images.loaded, images.failures);
    ImGui::End();

    ImGui::SetNextWindowPos({460, 12}, ImGuiCond_Always); ImGui::SetNextWindowSize({800, 0}, ImGuiCond_Always);
    ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Root must contain images/home/... (not the images folder itself).");
    ImGui::SetNextItemWidth(540); ImGui::InputText("##asset_root", asset_root.data(), asset_root.size()); ImGui::SameLine();
    if (ImGui::Button("Apply")) { images.set_root(asset_root.data()); images.queue_scene(initial); }
    if (images.root.empty()) ImGui::TextColored({1.0f, 0.55f, 0.25f, 1.0f}, "No asset root configured: fallback shapes are shown.");
    else if (images.failures > 0) ImGui::TextColored({1.0f, 0.55f, 0.25f, 1.0f}, "Loaded %d sprites; %d paths missing. Last: %s", images.loaded, images.failures, images.last_error.c_str());
    else ImGui::TextColored({0.35f, 0.9f, 0.45f, 1.0f}, "Loaded %d textures from: %s", images.loaded, images.root.c_str());
    ImGui::End();

    if (render_scene) draw_scene(renderer, initial, data, images, render_views, spell_effects, death_explosions, projectile_views, projectile_traces, now, heatmap_cache, map_origin_x, map_origin_y, zoom, ranges, heatmap, show_projectiles, selected_defender);
    else { SDL_SetRenderDrawColor(renderer, 24, 29, 37, 255); SDL_RenderClear(renderer); }
    char title[512];
    if (images.failures) std::snprintf(title, sizeof(title), "CoCSim | sprites: %d charges, %d manquants | %s", images.loaded, images.failures, images.last_error.c_str());
    else std::snprintf(title, sizeof(title), "CoCSim | sprites: %d charges", images.loaded);
    SDL_SetWindowTitle(window, title);
    ImGui::Render(); ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer); SDL_RenderPresent(renderer);
    first_frame_presented = true;
    // Present at 30 FPS. Fixed 16 ms core ticks continue between presentations
    // in order; a visual hitch can slow playback but can never skip a combat
    // event. Press P to retain controls while omitting all board rendering.
    const auto frame_ms = SDL_GetTicks() - frame_start;
    last_frame_work_ms = frame_ms;
    // A frame that exceeded the 30 FPS budget is normally caused by raster
    // work (large troop counts, range overlays or the heatmap). Reduce the
    // next frame's visual detail, never the arena itself. Asset decoding is
    // intentionally excluded: it is a one-frame presentation operation, not
    // a signal that combat or the arena cannot continue to be shown.
    if (adaptive_board && render_scene && !preload_this_frame && frame_ms > 33) {
      rendered_attacker_budget = std::max(kMinRenderedAttackers, rendered_attacker_budget / 2);
    } else if (adaptive_board && render_scene && frame_ms < 20 && rendered_attacker_budget < kMaxRenderedAttackers) {
      rendered_attacker_budget = std::min(kMaxRenderedAttackers, rendered_attacker_budget + 12);
    }
    if (frame_ms < 33) SDL_Delay(33 - frame_ms);
  }
  simulation->stop();
  ImGui_ImplSDLRenderer3_Shutdown(); ImGui_ImplSDL3_Shutdown(); ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
  return 0;
}
