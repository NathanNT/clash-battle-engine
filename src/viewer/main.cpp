#include "simulation_bridge.hpp"
#include "render.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <optional>
#include <string>
using namespace cocsim;
int main(int argc,char** argv) {
  bool smoke=false; std::string scenario_path,replay_path;
  for (int i=1;i<argc;++i) {
    if (std::strcmp(argv[i],"--smoke-test")==0) smoke=true;
    else if (std::strcmp(argv[i],"--scenario")==0 && i+1<argc) scenario_path=argv[++i];
    else if (std::strcmp(argv[i],"--replay")==0 && i+1<argc) replay_path=argv[++i];
    else { std::fprintf(stderr,"unknown option: %s\n",argv[i]); return 2; }
  }
  Scenario scenario; std::string error;
  if (!scenario_path.empty() && !load_scenario(scenario_path,scenario,error)) {
    std::fprintf(stderr,"scenario: %s\n",error.c_str()); return 2;
  }
  viewer::SimulationBridge bridge(scenario);
  if (!replay_path.empty() && !bridge.load_replay_file(replay_path,error)) {
    std::fprintf(stderr,"replay: %s\n",error.c_str()); return 2;
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) { std::fprintf(stderr,"SDL init: %s\n",SDL_GetError()); return 1; }
  SDL_Window* window=SDL_CreateWindow("CoCSim empty engine",1100,850,SDL_WINDOW_RESIZABLE);
  SDL_Renderer* renderer=window?SDL_CreateRenderer(window,nullptr):nullptr;
  if (!renderer) { std::fprintf(stderr,"SDL renderer: %s\n",SDL_GetError()); if(window)SDL_DestroyWindow(window); SDL_Quit(); return 1; }
  IMGUI_CHECKVERSION(); ImGui::CreateContext();
  ImGui_ImplSDL3_InitForSDLRenderer(window,renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
  bool running=true; int frames=0; auto previous=SDL_GetTicks();
  char path[512]="replays/empty.json";
  std::optional<GridCell> selected_cell;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type==SDL_EVENT_QUIT) running=false;
    }
    const auto now=SDL_GetTicks();
    bridge.present_elapsed(now-previous); previous=now;
    ImGui_ImplSDLRenderer3_NewFrame(); ImGui_ImplSDL3_NewFrame(); ImGui::NewFrame();
    ImGui::Begin("Empty Home Village");
    const auto& state=bridge.state();
    ImGui::Text("Ruleset: %s",kEmptyRuleset);
    ImGui::Text("Tick: %lld ms | Time: %lld / %lld ms",
      static_cast<long long>(kTickMs),static_cast<long long>(state.time_ms()),
      static_cast<long long>(state.scenario().duration_ms));
    ImGui::Text("Result: %s | State hash: %llu",result_name(state.result()),
      static_cast<unsigned long long>(state.state_hash()));
    if (ImGui::Button(bridge.paused()?"Play":"Pause")) bridge.set_paused(!bridge.paused());
    ImGui::SameLine(); if (ImGui::Button("Step 16 ms")) bridge.step();
    ImGui::SameLine(); if (ImGui::Button("Reset")) bridge.reset();
    ImGui::SameLine(); if (ImGui::Button("New empty")) bridge.new_empty();
    int speed=bridge.speed(); if (ImGui::SliderInt("Playback speed",&speed,1,8)) bridge.set_speed(speed);
    if (ImGui::Button("Wait next tick")) bridge.wait_next_tick(error);
    ImGui::SameLine(); if (ImGui::Button("End next tick")) bridge.end_next_tick(error);
    ImGui::InputText("Replay path",path,sizeof(path));
    if (ImGui::Button("Save replay")) {
      const auto parent=std::filesystem::path(path).parent_path();
      std::error_code filesystem_error;
      if (!parent.empty()) std::filesystem::create_directories(parent,filesystem_error);
      if (filesystem_error) error=filesystem_error.message();
      else save_replay(path,state.scenario(),bridge.commands_for_save(),error);
    }
    ImGui::SameLine(); if (ImGui::Button("Load replay")) bridge.load_replay_file(path,error);
    if (!error.empty()) ImGui::TextWrapped("%s",error.c_str());
    ImGui::TextUnformatted("Empty board (44x44 build area, 3-tile border)");
    viewer::draw_board(state.board(),selected_cell);
    if (selected_cell) ImGui::Text("Selected cell: (%d, %d)",selected_cell->x,selected_cell->y);
    if (ImGui::CollapsingHeader("Event log")) {
      for (const auto& entry:state.events()) ImGui::Text("%lld ms | #%llu | %s",
        static_cast<long long>(entry.time_ms),static_cast<unsigned long long>(entry.sequence),entry.detail.c_str());
    }
    ImGui::End();
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer,22,29,25,255); SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),renderer);
    SDL_RenderPresent(renderer);
    if (smoke && ++frames>=1) running=false;
  }
  ImGui_ImplSDLRenderer3_Shutdown(); ImGui_ImplSDL3_Shutdown(); ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
  return 0;
}
