#include "cocsim/core.hpp"
#include "loadout_editor.hpp"
#include "test_support.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <array>
#include <map>

using namespace cocsim;

namespace {
struct Frame {
  ImVec2 apply_min{};
  ImVec2 apply_max{};
  bool applied{};
};
}

int main() {
  const auto data = GameData::v0();
  Scenario draft;
  draft.width = 30;
  draft.height = 20;
  draft.duration_ms = 3000;
  draft.defenders = {{Kind::GoldStorage, 1, {25.5, 15.5}}};
  draft.army = {{Kind::ArcherQueen, 110, 1}};

  ImGui::CreateContext();
  auto& io = ImGui::GetIO();
  io.DisplaySize = {900.0f, 700.0f};
  io.DeltaTime = 1.0f / 60.0f;
  io.Fonts->Build();
  auto draw = [&](ImVec2 mouse, bool button_down) {
    io.AddMousePosEvent(mouse.x, mouse.y);
    io.AddMouseButtonEvent(0, button_down);
    ImGui::NewFrame();
    ImGui::SetNextWindowPos({10.0f, 10.0f});
    ImGui::SetNextWindowSize({600.0f, 500.0f});
    ImGui::Begin("Hero selection test");
    const bool applied = viewer::draw_hero_loadout_editor(data, draft);
    const Frame result{ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), applied};
    ImGui::End();
    ImGui::Render();
    return result;
  };
  auto click = [&](ImVec2 point) {
    draw(point, true);
    return draw(point, false);
  };

  const auto initial = draw({-1.0f, -1.0f}, false);
  const float step = ImGui::GetFrameHeightWithSpacing();
  const float pet_y = initial.apply_min.y - 2.0f * step + ImGui::GetFrameHeight() * 0.5f;
  click({initial.apply_min.x + 80.0f, pet_y});
  draw({-1.0f, -1.0f}, false);
  auto& context = *ImGui::GetCurrentContext();
  COCSIM_REQUIRE(context.OpenPopupStack.Size == 1);
  auto* popup = context.OpenPopupStack[0].Window;
  COCSIM_REQUIRE(popup);
  const ImVec2 pet_choice{popup->Pos.x + popup->WindowPadding.x + 50.0f,
                          popup->Pos.y + popup->WindowPadding.y
                              + ImGui::GetTextLineHeightWithSpacing() * 1.5f};
  const auto after_pet = click(pet_choice);
  COCSIM_REQUIRE(draft.hero_loadouts.size() == 1);
  COCSIM_REQUIRE(draft.hero_loadouts[0].pet);
  COCSIM_REQUIRE(draft.hero_loadouts[0].pet->id == "lassi");

  const float equipment_y = after_pet.apply_min.y - step + ImGui::GetFrameHeight() * 0.5f;
  click({after_pet.apply_min.x + 80.0f, equipment_y});
  draw({-1.0f, -1.0f}, false);
  COCSIM_REQUIRE(context.OpenPopupStack.Size == 1);
  popup = context.OpenPopupStack[0].Window;
  COCSIM_REQUIRE(popup);
  const ImVec2 equipment_choice{popup->Pos.x + popup->WindowPadding.x + 50.0f,
                                popup->Pos.y + popup->WindowPadding.y
                                    + ImGui::GetTextLineHeightWithSpacing() * 0.5f};
  click(equipment_choice);
  COCSIM_REQUIRE(draft.hero_loadouts[0].equipment.size() == 1);
  const auto& selected = draft.hero_loadouts[0].equipment.front();
  const auto* selected_record = data.find_hero_support(selected.id);
  COCSIM_REQUIRE(selected_record && selected_record->hero == "archer_queen");
  COCSIM_REQUIRE(selected.level == 1);
  BattleState battle(data, draft);
  COCSIM_REQUIRE(battle.scenario().hero_loadouts[0].equipment.front().id == selected.id);

  // Render every catalogue choice at its highest sourced level and check that
  // the same value is accepted by Core. Mouse activation is covered above.
  int rendered_pets = 0;
  int rendered_equipment = 0;
  for (const auto& row : data.hero_support_catalogue()) {
    if (row.category == "pet") {
      draft.army = {{Kind::ArcherQueen, 110, 1}};
      draft.hero_loadouts = {{Kind::ArcherQueen, HeroSupportChoice{row.id, row.max_level}, {}}};
      ++rendered_pets;
    } else if (row.category == "hero-equipment") {
      const auto hero = parse_kind(row.hero);
      COCSIM_REQUIRE(hero);
      draft.army = {{*hero, 1, 1}};
      draft.hero_loadouts = {{*hero, std::nullopt, {{row.id, row.max_level}}}};
      ++rendered_equipment;
    } else {
      continue;
    }
    const auto frame = draw({-1.0f, -1.0f}, false);
    COCSIM_REQUIRE(frame.apply_max.x > frame.apply_min.x);
    COCSIM_REQUIRE(ImGui::GetDrawData()->TotalVtxCount > 0);
    BattleState selected_battle(data, draft);
    COCSIM_REQUIRE(to_json(selected_battle.scenario().hero_loadouts) == to_json(draft.hero_loadouts));
  }
  COCSIM_REQUIRE(rendered_pets == 12);
  COCSIM_REQUIRE(rendered_equipment == 42);
  const auto* healing_tome_level = data.find_hero_support_level("healing_tome", 18);
  COCSIM_REQUIRE(healing_tome_level);
  COCSIM_REQUIRE(healing_tome_level->normalized_row_json.find("official_duration_evidence")
                 != std::string::npos);
  COCSIM_REQUIRE(healing_tome_level->normalized_row_json.find("20000")
                 != std::string::npos);
  for (const char* content_id : std::array{"royal_gem", "earthquake_boots", "vampstache",
                                            "barbarian_puppet", "archer_puppet", "electro_boots"}) {
    const auto* content = data.find_hero_support(content_id);
    COCSIM_REQUIRE(content);
    const auto* level = data.find_hero_support_level(content_id, content->max_level);
    COCSIM_REQUIRE(level);
    COCSIM_REQUIRE(level->normalized_row_json.find("primary_level_evidence") != std::string::npos);
  }

  int clicked_pets = 0;
  int clicked_equipment = 0;
  std::map<std::string, int> equipment_index;
  for (const auto& row : data.hero_support_catalogue()) {
    const bool is_pet = row.category == "pet";
    if (!is_pet && row.category != "hero-equipment") continue;
    const auto hero = is_pet ? std::optional<Kind>{Kind::ArcherQueen} : parse_kind(row.hero);
    COCSIM_REQUIRE(hero);
    draft.army = {{*hero, 1, 1}};
    draft.hero_loadouts.clear();
    const auto initial_frame = draw({-1.0f, -1.0f}, false);
    const float combo_y = initial_frame.apply_min.y - (is_pet ? 2.0f : 1.0f) * step
                          + ImGui::GetFrameHeight() * 0.5f;
    click({initial_frame.apply_min.x + 80.0f, combo_y});
    draw({-1.0f, -1.0f}, false);
    COCSIM_REQUIRE(context.OpenPopupStack.Size == 1);
    popup = context.OpenPopupStack[0].Window;
    COCSIM_REQUIRE(popup);
    const int index = is_pet ? ++clicked_pets : equipment_index[row.hero]++;
    const float line_height = ImGui::GetTextLineHeightWithSpacing();
    popup->Scroll.y = std::clamp((static_cast<float>(index) - 2.0f) * line_height,
                                 0.0f, popup->ScrollMax.y);
    draw({-1.0f, -1.0f}, false);
    popup = context.OpenPopupStack[0].Window;
    const ImVec2 choice{popup->Pos.x + popup->WindowPadding.x + 50.0f,
                        popup->Pos.y + popup->WindowPadding.y
                            + line_height * (static_cast<float>(index) + 0.5f)
                            - popup->Scroll.y};
    COCSIM_REQUIRE(choice.y > popup->Pos.y && choice.y < popup->Pos.y + popup->Size.y);
    click(choice);
    COCSIM_REQUIRE(draft.hero_loadouts.size() == 1);
    if (is_pet) {
      COCSIM_REQUIRE(draft.hero_loadouts[0].pet);
      COCSIM_REQUIRE(draft.hero_loadouts[0].pet->id == row.id);
    } else {
      ++clicked_equipment;
      COCSIM_REQUIRE(draft.hero_loadouts[0].equipment.size() == 1);
      COCSIM_REQUIRE(draft.hero_loadouts[0].equipment[0].id == row.id);
    }
    BattleState clicked_battle(data, draft);
    COCSIM_REQUIRE(to_json(clicked_battle.scenario().hero_loadouts) == to_json(draft.hero_loadouts));
  }
  COCSIM_REQUIRE(clicked_pets == 12);
  COCSIM_REQUIRE(clicked_equipment == 42);

  draft.army = {{Kind::GrandWarden, 85, 1, "air"}};
  draft.hero_loadouts.clear();
  const auto air_frame = draw({-1.0f, -1.0f}, false);
  const float mode_y = air_frame.apply_min.y - 3.0f * step
                       + ImGui::GetFrameHeight() * 0.5f;
  click({air_frame.apply_min.x + 80.0f, mode_y});
  draw({-1.0f, -1.0f}, false);
  COCSIM_REQUIRE(context.OpenPopupStack.Size == 1);
  popup = context.OpenPopupStack[0].Window;
  COCSIM_REQUIRE(popup);
  const ImVec2 ground_choice{popup->Pos.x + popup->WindowPadding.x + 50.0f,
                             popup->Pos.y + popup->WindowPadding.y
                                 + ImGui::GetTextLineHeightWithSpacing() * 0.5f};
  click(ground_choice);
  COCSIM_REQUIRE(draft.army[0].mode == "ground");
  BattleState ground_battle(data, draft);
  COCSIM_REQUIRE(ground_battle.scenario().army[0].mode == "ground");
  const auto ground_frame = draw({-1.0f, -1.0f}, false);
  click({ground_frame.apply_min.x + 80.0f, mode_y});
  draw({-1.0f, -1.0f}, false);
  COCSIM_REQUIRE(context.OpenPopupStack.Size == 1);
  popup = context.OpenPopupStack[0].Window;
  COCSIM_REQUIRE(popup);
  const ImVec2 air_choice{popup->Pos.x + popup->WindowPadding.x + 50.0f,
                          popup->Pos.y + popup->WindowPadding.y
                              + ImGui::GetTextLineHeightWithSpacing() * 1.5f};
  click(air_choice);
  COCSIM_REQUIRE(draft.army[0].mode == "air");
  BattleState air_battle(data, draft);
  COCSIM_REQUIRE(air_battle.scenario().army[0].mode == "air");

  ImGui::DestroyContext();
  return 0;
}
