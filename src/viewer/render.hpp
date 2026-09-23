#pragma once
#include "cocsim/core.hpp"
#include "imgui.h"
#include <algorithm>
#include <optional>
namespace cocsim::viewer {
inline void draw_board(const Board& board, std::optional<GridCell>& selected) {
  const ImVec2 origin=ImGui::GetCursorScreenPos();
  const float side=std::min(ImGui::GetContentRegionAvail().x,650.0f);
  const float cell=side/static_cast<float>(board.width());
  auto* draw=ImGui::GetWindowDrawList();
  draw->AddRectFilled(origin,ImVec2(origin.x+side,origin.y+side),IM_COL32(35,54,43,255));
  const auto border=kHomeVillageDeploymentBorderTiles;
  draw->AddRectFilled(ImVec2(origin.x+cell*border,origin.y+cell*border),
    ImVec2(origin.x+cell*(board.width()-border),origin.y+cell*(board.height()-border)),IM_COL32(51,78,55,255));
  for (int i=0;i<=board.width();++i) {
    const float x=origin.x+cell*i;
    draw->AddLine(ImVec2(x,origin.y),ImVec2(x,origin.y+side),IM_COL32(90,115,90,70));
  }
  for (int i=0;i<=board.height();++i) {
    const float y=origin.y+cell*i;
    draw->AddLine(ImVec2(origin.x,y),ImVec2(origin.x+side,y),IM_COL32(90,115,90,70));
  }
  if (selected && board.contains(*selected)) {
    const ImVec2 low(origin.x+cell*selected->x,origin.y+cell*selected->y);
    draw->AddRect(low,ImVec2(low.x+cell,low.y+cell),IM_COL32(255,215,90,255),0.0f,0,2.0f);
  }
  ImGui::InvisibleButton("board",ImVec2(side,side));
  if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    const auto mouse=ImGui::GetIO().MousePos;
    GridCell cell_at{static_cast<int>((mouse.x-origin.x)/cell),static_cast<int>((mouse.y-origin.y)/cell)};
    if (board.contains(cell_at)) selected=cell_at;
  }
}
} // namespace cocsim::viewer
