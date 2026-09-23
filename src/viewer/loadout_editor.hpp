#pragma once

#include "cocsim/core.hpp"
#include "imgui.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>

namespace cocsim::viewer {

inline HeroLoadout& editable_loadout(Scenario& scenario, Kind hero) {
  const auto found = std::find_if(scenario.hero_loadouts.begin(), scenario.hero_loadouts.end(),
                                  [&](const HeroLoadout& row) { return row.hero == hero; });
  if (found != scenario.hero_loadouts.end()) return *found;
  scenario.hero_loadouts.push_back({hero, std::nullopt, {}});
  return scenario.hero_loadouts.back();
}

inline void draw_hero_support_level_source(const GameData& data, const HeroSupportChoice& choice) {
  const auto* row = data.find_hero_support_level(choice.id, choice.level);
  if (!row) return;
  if (ImGui::TreeNode("Pinned level record")) {
    ImGui::TextWrapped("%s", row->normalized_row_json.c_str());
    ImGui::TreePop();
  }
}

inline bool draw_hero_loadout_editor(const GameData& data, Scenario& draft) {
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  if (!ImGui::TreeNode("Hero loadouts (avant combat)")) return false;
  ImGui::TextDisabled("Selection seulement; appliquer recommence au tick T+0.");
  const auto& entries = data.hero_support_catalogue();
  for (auto& army : draft.army) {
    const auto hero = to_string(army.kind);
    const auto has_equipment = std::any_of(entries.begin(), entries.end(), [&](const HeroSupportCatalogEntry& row) {
      return row.category == "hero-equipment" && row.hero == hero;
    });
    if (!has_equipment || army.count != 1) continue;
    ImGui::PushID(hero.c_str());
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode(hero.c_str())) {
      auto& loadout = editable_loadout(draft, army.kind);
      if (army.kind == Kind::GrandWarden) {
        const char* mode_preview = army.mode == "air" ? "air" : "ground";
        if (ImGui::BeginCombo("Mode", mode_preview)) {
          if (ImGui::Selectable("ground", army.mode != "air")) army.mode = "ground";
          if (ImGui::Selectable("air", army.mode == "air")) army.mode = "air";
          ImGui::EndCombo();
        }
      }
      const char* pet_preview = loadout.pet ? loadout.pet->id.c_str() : "Aucun";
      if (ImGui::BeginCombo("Pet", pet_preview)) {
        if (ImGui::Selectable("Aucun", !loadout.pet)) loadout.pet.reset();
        for (const auto& row : entries) if (row.category == "pet")
          if (ImGui::Selectable(row.id.c_str(), loadout.pet && loadout.pet->id == row.id))
            loadout.pet = HeroSupportChoice{row.id, 1};
        ImGui::EndCombo();
      }
      if (loadout.pet) {
        const auto pet = std::find_if(entries.begin(), entries.end(), [&](const HeroSupportCatalogEntry& row) {
          return row.category == "pet" && row.id == loadout.pet->id;
        });
        int level = loadout.pet->level;
        if (pet != entries.end() && ImGui::InputInt("Pet level", &level))
          loadout.pet->level = std::clamp(level, 1, pet->max_level);
        draw_hero_support_level_source(data, *loadout.pet);
      }
      for (std::size_t index = 0; index < loadout.equipment.size();) {
        ImGui::PushID(static_cast<int>(index));
        const auto& choice = loadout.equipment[index];
        const auto row = std::find_if(entries.begin(), entries.end(), [&](const HeroSupportCatalogEntry& entry) {
          return entry.category == "hero-equipment" && entry.id == choice.id;
        });
        ImGui::Text("%s", choice.id.c_str());
        ImGui::SameLine();
        int level = choice.level;
        ImGui::SetNextItemWidth(90.0f);
        if (row != entries.end() && ImGui::InputInt("Level", &level))
          loadout.equipment[index].level = std::clamp(level, 1, row->max_level);
        ImGui::SameLine();
        const bool removed = ImGui::Button("Retirer");
        if (!removed) draw_hero_support_level_source(data, loadout.equipment[index]);
        ImGui::PopID();
        if (removed) loadout.equipment.erase(loadout.equipment.begin() + static_cast<std::ptrdiff_t>(index));
        else ++index;
      }
      if (loadout.equipment.size() < 2 && ImGui::BeginCombo("Ajouter equipement", "Choisir")) {
        for (const auto& row : entries) if (row.category == "hero-equipment" && row.hero == hero) {
          const bool used = std::any_of(loadout.equipment.begin(), loadout.equipment.end(),
                                        [&](const HeroSupportChoice& choice) { return choice.id == row.id; });
          if (!used && ImGui::Selectable(row.id.c_str())) loadout.equipment.push_back({row.id, 1});
        }
        ImGui::EndCombo();
      }
      ImGui::TreePop();
    }
    ImGui::PopID();
  }
  constexpr std::array banner_heroes = {
      Kind::BarbarianKing, Kind::ArcherQueen, Kind::GrandWarden,
      Kind::RoyalChampion, Kind::MinionPrince, Kind::DragonDuke};
  for (std::size_t index = 0; index < draft.non_combat_obstacles.size(); ++index) {
    auto& placement = draft.non_combat_obstacles[index];
    if (placement.kind != Kind::HeroBanner) continue;
    ImGui::PushID(static_cast<int>(index));
    ImGui::Text("Hero Banner %zu", index + 1);
    if (ImGui::BeginCombo("Defending Hero", placement.variant.c_str())) {
      for (const auto hero : banner_heroes) {
        const auto id = to_string(hero);
        if (ImGui::Selectable(id.c_str(), placement.variant == id)) placement.variant = id;
      }
      ImGui::EndCombo();
    }
    ImGui::PopID();
  }
  const bool apply = ImGui::Button("Appliquer la selection et recommencer");
  ImGui::TreePop();
  return apply;
}

} // namespace cocsim::viewer
