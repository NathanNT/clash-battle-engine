#include "cocsim/core.hpp"
#include "loadout_editor.hpp"
#include "test_support.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <array>
#include <cstdio>
#include <utility>

using namespace cocsim;

int main() {
  const auto data = GameData::v0();
  Scenario draft;
  draft.width = 30;
  draft.height = 30;
  draft.duration_ms = 3000;
  draft.defenders = {{Kind::GoldStorage, 1, {25.5, 25.5}}};
  draft.non_combat_obstacles = {{Kind::HeroBanner, {10.0, 10.0}, "normal"}};

  ImGui::CreateContext();
  auto& io = ImGui::GetIO();
  io.DisplaySize = {900.0f, 700.0f};
  io.DeltaTime = 1.0f / 60.0f;
  io.Fonts->Build();
  auto draw = [&](ImVec2 mouse, bool down) {
    io.AddMousePosEvent(mouse.x, mouse.y);
    io.AddMouseButtonEvent(0, down);
    ImGui::NewFrame();
    ImGui::SetNextWindowPos({10.0f, 10.0f});
    ImGui::SetNextWindowSize({600.0f, 500.0f});
    ImGui::Begin("Hero Banner assignment test");
    const bool applied = viewer::draw_hero_loadout_editor(data, draft);
    const auto apply_min = ImGui::GetItemRectMin();
    ImGui::End();
    ImGui::Render();
    return std::pair{apply_min, applied};
  };
  auto click = [&](ImVec2 point) {
    draw(point, true);
    return draw(point, false);
  };

  auto& context = *ImGui::GetCurrentContext();
  constexpr std::array banner_heroes = {
      Kind::BarbarianKing, Kind::ArcherQueen, Kind::GrandWarden,
      Kind::RoyalChampion, Kind::MinionPrince, Kind::DragonDuke};
  for (std::size_t index = 0; index < banner_heroes.size(); ++index) {
    const auto initial = draw({-1.0f, -1.0f}, false);
    const float combo_y = initial.first.y - ImGui::GetFrameHeightWithSpacing()
                          + ImGui::GetFrameHeight() * 0.5f;
    click({initial.first.x + 80.0f, combo_y});
    draw({-1.0f, -1.0f}, false);
    COCSIM_REQUIRE(context.OpenPopupStack.Size == 1);
    const auto* popup = context.OpenPopupStack[0].Window;
    COCSIM_REQUIRE(popup);
    const ImVec2 choice{popup->Pos.x + popup->WindowPadding.x + 50.0f,
                        popup->Pos.y + popup->WindowPadding.y
                            + ImGui::GetTextLineHeightWithSpacing()
                                  * (static_cast<float>(index) + 0.5f)};
    click(choice);
    COCSIM_REQUIRE(draft.non_combat_obstacles[0].variant == to_string(banner_heroes[index]));
  }
  COCSIM_REQUIRE(draft.non_combat_obstacles[0].variant == "dragon_duke");
  const auto apply_frame = draw({-1.0f, -1.0f}, false);
  const ImVec2 apply_point{apply_frame.first.x + 80.0f,
                           apply_frame.first.y + ImGui::GetFrameHeight() * 0.5f};
  const bool applied_on_press = draw(apply_point, true).second;
  const bool applied_on_release = draw(apply_point, false).second;
  COCSIM_REQUIRE(applied_on_press || applied_on_release);

  BattleState selected(data, draft);
  COCSIM_REQUIRE(selected.observe().size() == 1);
  const auto snap = selected.snapshot();
  BattleState restored(data, draft);
  std::string error;
  COCSIM_REQUIRE(restored.restore(snap, &error));
  COCSIM_REQUIRE(restored.state_hash() == selected.state_hash());

  auto queen_draft = draft;
  queen_draft.non_combat_obstacles[0].variant = "archer_queen";
  BattleState queen(data, queen_draft);
  COCSIM_REQUIRE(queen.state_hash() == selected.state_hash());
  COCSIM_REQUIRE(!queen.restore(snap, &error));

  const char* replay_path = "viewer-banner-assignment-replay.json";
  COCSIM_REQUIRE(save_replay(replay_path, draft, {}, error));
  Scenario replayed;
  std::vector<Command> commands;
  COCSIM_REQUIRE(load_replay(replay_path, replayed, commands, error));
  COCSIM_REQUIRE(replayed.non_combat_obstacles[0].variant == "dragon_duke");
  COCSIM_REQUIRE(std::remove(replay_path) == 0);
  ImGui::DestroyContext();
  return 0;
}
