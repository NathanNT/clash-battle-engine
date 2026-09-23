#pragma once

#include "cocsim/core.hpp"
#include "imgui.h"

namespace cocsim::viewer {

inline void draw_hero_support_catalogue(const GameData& data,
                                         const std::vector<HeroLoadout>& loadouts, int housing,
                                         int tier, int damage_percent) {
  if (!ImGui::TreeNode("Pets et equipements (catalogue seulement)")) return;
  ImGui::TextDisabled("Monolith Arrow : logement deploye %d, palier %d, %d%% PV max (projection seulement)",
                      housing, tier + 1, damage_percent);
  for (const auto& loadout : loadouts) {
    ImGui::Text("%s | Pet: %s", to_string(loadout.hero).c_str(),
                loadout.pet ? loadout.pet->id.c_str() : "aucun");
    for (const auto& equipment : loadout.equipment)
      ImGui::BulletText("%s L%d (selection seulement)", equipment.id.c_str(), equipment.level);
  }
  for (const auto& entry : data.hero_support_catalogue()) {
    if (entry.hero.empty())
      ImGui::Text("%s | %s | L1-%d", entry.category.c_str(), entry.id.c_str(), entry.max_level);
    else
      ImGui::Text("%s | %s | %s | L1-%d | %s", entry.category.c_str(), entry.id.c_str(),
                  entry.hero.c_str(), entry.max_level, entry.ability_type.c_str());
  }
  ImGui::TreePop();
}

} // namespace cocsim::viewer
