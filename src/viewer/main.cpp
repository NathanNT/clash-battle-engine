#include "simulation_bridge.hpp"
#include "render.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <optional>
#include <string>
using namespace cocsim;
int main(int argc,char** argv) {
  bool smoke=false;
  std::string scenario_path,replay_path,screenshot_path;
  for (int i=1;i<argc;++i) {
    if (std::strcmp(argv[i],"--smoke-test")==0) smoke=true;
    else if (std::strcmp(argv[i],"--screenshot")==0 && i+1<argc) { screenshot_path=argv[++i]; smoke=true; }
    else if (std::strcmp(argv[i],"--scenario")==0 && i+1<argc) scenario_path=argv[++i];
    else if (std::strcmp(argv[i],"--replay")==0 && i+1<argc) replay_path=argv[++i];
    else { std::fprintf(stderr,"unknown option: %s\n",argv[i]); return 2; }
  }
  Scenario scenario;
  std::string error;
  if (!scenario_path.empty() && !load_scenario(scenario_path,scenario,error)) {
    std::fprintf(stderr,"scenario: %s\n",error.c_str()); return 2;
  }
  viewer::SimulationBridge bridge(scenario);
  if (!replay_path.empty() && !bridge.load_replay_file(replay_path,error)) {
    std::fprintf(stderr,"replay: %s\n",error.c_str()); return 2;
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) { std::fprintf(stderr,"SDL init: %s\n",SDL_GetError()); return 1; }
  SDL_Window* window=SDL_CreateWindow("Clash Battle Engine",1100,850,SDL_WINDOW_RESIZABLE);
  SDL_Renderer* renderer=window?SDL_CreateRenderer(window,nullptr):nullptr;
  if (!renderer) {
    std::fprintf(stderr,"SDL renderer: %s\n",SDL_GetError());
    if (window) SDL_DestroyWindow(window);
    SDL_Quit(); return 1;
  }
  SDL_SetWindowMinimumSize(window,800,600);
  IMGUI_CHECKVERSION(); ImGui::CreateContext();
  const char* windows_dir=std::getenv("WINDIR");
  const auto system_font=windows_dir
    ? std::filesystem::path(windows_dir)/"Fonts"/"segoeui.ttf"
    : std::filesystem::path{};
  ImFont* ui_font=nullptr;
  if (!system_font.empty() && std::filesystem::exists(system_font))
    ui_font=ImGui::GetIO().Fonts->AddFontFromFileTTF(system_font.string().c_str(),16.0f);
  if (!ui_font) {
    ImFontConfig font_config;
    font_config.SizePixels=16.0f;
    ImGui::GetIO().Fonts->AddFontDefault(&font_config);
  }
  auto& style=ImGui::GetStyle();
  style.WindowPadding=ImVec2(14.0f,14.0f);
  style.ItemSpacing=ImVec2(8.0f,9.0f);
  style.FramePadding=ImVec2(7.0f,5.0f);
  style.WindowRounding=7.0f;
  style.FrameRounding=4.0f;
  ImGui_ImplSDL3_InitForSDLRenderer(window,renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
  bool running=true;
  int frames=0,window_width=1100,window_height=850,exit_code=0;
  auto previous=SDL_GetTicks();
  char replay_file[512]="replays/session.json";
  std::optional<GridCell> selected_cell;
  while (running) {
    SDL_GetWindowSize(window,&window_width,&window_height);
    SDL_SetRenderLogicalPresentation(renderer,window_width,window_height,SDL_LOGICAL_PRESENTATION_STRETCH);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type==SDL_EVENT_QUIT) running=false;
      if (event.type==SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button==SDL_BUTTON_LEFT) {
        SDL_Event board_event=event;
        SDL_ConvertEventToRenderCoordinates(renderer,&board_event);
        const auto viewport=viewer::BoardViewport::for_window(window_width,window_height);
        const auto cell=viewport.cell_at(board_event.button.x,board_event.button.y,bridge.state().board());
        if (cell) selected_cell=cell;
      }
    }
    const auto now=SDL_GetTicks();
    bridge.present_elapsed(now-previous);
    previous=now;
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    const auto viewport=viewer::BoardViewport::for_window(window_width,window_height);
    auto* overlay=ImGui::GetForegroundDrawList();
    overlay->AddText(ImVec2(viewport.x,viewport.y-25.0f),IM_COL32(231,239,230,255),"Battlefield");
    constexpr float sidebar=300.0f,margin=20.0f;
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(window_width)-sidebar-margin,margin),ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(sidebar,static_cast<float>(window_height)-2.0f*margin),ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.96f);
    constexpr ImGuiWindowFlags panel_flags=ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("Simulation controls",nullptr,panel_flags);
    const auto& state=bridge.state();
    ImGui::TextUnformatted("Simulation");
    ImGui::Separator();
    ImGui::Text("%lld / %lld ms",static_cast<long long>(state.time_ms()),
      static_cast<long long>(state.scenario().duration_ms));
    ImGui::ProgressBar(static_cast<float>(state.time_ms())/
      static_cast<float>(state.scenario().duration_ms),ImVec2(-1.0f,0.0f));
    ImGui::Text("16 ms tick  |  %s",result_name(state.result()));
    if (ImGui::Button(bridge.paused()?"Play":"Pause",ImVec2(80.0f,0.0f)))
      bridge.set_paused(!bridge.paused());
    ImGui::SameLine();
    if (ImGui::Button("Step",ImVec2(80.0f,0.0f))) bridge.step();
    ImGui::SameLine();
    if (ImGui::Button("Reset",ImVec2(80.0f,0.0f))) bridge.reset();
    int speed=bridge.speed();
    ImGui::Text("Playback speed  %dx",speed);
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::SliderInt("##speed",&speed,1,8)) bridge.set_speed(speed);
    if (ImGui::Button("New scenario",ImVec2(-1.0f,0.0f))) {
      bridge.new_scenario();
      selected_cell.reset();
      error.clear();
    }
    ImGui::Spacing();
    ImGui::TextUnformatted("Board");
    ImGui::Separator();
    ImGui::TextUnformatted("50 x 50 tiles");
    ImGui::TextUnformatted("44 x 44 build area");
    if (selected_cell) {
      ImGui::Text("Cell (%d, %d)",selected_cell->x,selected_cell->y);
      ImGui::TextUnformatted(state.board().buildable(*selected_cell)?"Build area":"Border");
    } else ImGui::TextUnformatted("Click a tile to inspect it");
    if (ImGui::CollapsingHeader("Commands")) {
      if (ImGui::Button("Wait next tick")) bridge.wait_next_tick(error);
      if (ImGui::Button("End battle")) bridge.end_next_tick(error);
    }
    ImGui::Spacing();
    ImGui::TextUnformatted("Replay");
    ImGui::Separator();
    ImGui::TextUnformatted("File path");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputText("##replay_file",replay_file,sizeof(replay_file));
    if (ImGui::Button("Save",ImVec2(90.0f,0.0f))) {
      const auto parent=std::filesystem::path(replay_file).parent_path();
      std::error_code filesystem_error;
      if (!parent.empty()) std::filesystem::create_directories(parent,filesystem_error);
      if (filesystem_error) error=filesystem_error.message();
      else save_replay(replay_file,state.scenario(),bridge.commands_for_save(),error);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load",ImVec2(90.0f,0.0f))) {
      if (bridge.load_replay_file(replay_file,error)) selected_cell.reset();
    }
    if (bridge.replay_loaded()) ImGui::TextUnformatted("Replay loaded; press Play or Step");
    if (!error.empty()) ImGui::TextWrapped("%s",error.c_str());
    if (ImGui::CollapsingHeader("Diagnostics")) {
      ImGui::Text("Ruleset: %s",kRulesetId);
      ImGui::Text("State hash: %llu",static_cast<unsigned long long>(state.state_hash()));
      if (ImGui::TreeNode("Event log")) {
        for (const auto& entry:state.events())
          ImGui::Text("%lld ms  #%llu  %s",static_cast<long long>(entry.time_ms),
            static_cast<unsigned long long>(entry.sequence),entry.detail.c_str());
        ImGui::TreePop();
      }
    }
    ImGui::End();
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer,20,29,30,255);
    SDL_RenderClear(renderer);
    viewer::draw_board(renderer,state.board(),viewport,selected_cell);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),renderer);
    if (!screenshot_path.empty() && frames==0) {
      SDL_Surface* screenshot=SDL_RenderReadPixels(renderer,nullptr);
      if (!screenshot || !SDL_SaveBMP(screenshot,screenshot_path.c_str())) {
        std::fprintf(stderr,"screenshot: %s\n",SDL_GetError());
        if (screenshot) SDL_DestroySurface(screenshot);
        exit_code=1;
        break;
      }
      SDL_DestroySurface(screenshot);
    }
    SDL_RenderPresent(renderer);
    if (smoke && ++frames>=1) running=false;
  }
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return exit_code;
}
