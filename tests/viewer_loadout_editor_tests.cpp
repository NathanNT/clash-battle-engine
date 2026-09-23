#include "cocsim/core.hpp"
#include "demo_scenario.hpp"
#include "loadout_editor.hpp"
#include "test_support.hpp"

#include "imgui.h"

using namespace cocsim;

int main() {
  const auto data = GameData::v0();
  auto draft = viewer::demo_scenario();
  draft.hero_loadouts = {
    {Kind::GrandWarden, HeroSupportChoice{"lassi", 15}, {{"eternal_tome", 18}}},
  };
  ImGui::CreateContext();
  auto& io = ImGui::GetIO();
  io.DisplaySize = {1280.0f, 900.0f};
  io.DeltaTime = 1.0f / 60.0f;
  io.Fonts->Build();
  ImGui::NewFrame();
  ImGui::SetNextWindowPos({10.0f, 10.0f});
  ImGui::SetNextWindowSize({600.0f, 800.0f});
  ImGui::Begin("Viewer loadout controls");
  COCSIM_REQUIRE(!viewer::draw_hero_loadout_editor(data, draft));
  const auto apply_min = ImGui::GetItemRectMin();
  const auto apply_max = ImGui::GetItemRectMax();
  ImGui::End();
  ImGui::Render();
  COCSIM_REQUIRE(ImGui::GetDrawData()->CmdListsCount > 0);
  COCSIM_REQUIRE(draft.hero_loadouts.size() == 3);
  BattleState selected(data, draft);
  COCSIM_REQUIRE(selected.scenario().hero_loadouts.size() == 3);

  const ImVec2 apply_center{(apply_min.x + apply_max.x) * 0.5f,
                            (apply_min.y + apply_max.y) * 0.5f};
  io.AddMousePosEvent(apply_center.x, apply_center.y);
  io.AddMouseButtonEvent(0, true);
  ImGui::NewFrame();
  ImGui::SetNextWindowPos({10.0f, 10.0f});
  ImGui::SetNextWindowSize({600.0f, 800.0f});
  ImGui::Begin("Viewer loadout controls");
  const bool applied_on_press = viewer::draw_hero_loadout_editor(data, draft);
  ImGui::End();
  ImGui::Render();
  io.AddMouseButtonEvent(0, false);
  ImGui::NewFrame();
  ImGui::SetNextWindowPos({10.0f, 10.0f});
  ImGui::SetNextWindowSize({600.0f, 800.0f});
  ImGui::Begin("Viewer loadout controls");
  const bool applied_on_release = viewer::draw_hero_loadout_editor(data, draft);
  ImGui::End();
  ImGui::Render();
  COCSIM_REQUIRE(applied_on_press != applied_on_release);
  ImGui::DestroyContext();
  return 0;
}
