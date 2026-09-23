#pragma once
#include "clash_battle_engine/core.hpp"
#include <algorithm>
#include <optional>
namespace clash_battle_engine::viewer {
struct BoardViewport {
  float x{};
  float y{};
  float side{};
  static BoardViewport for_window(int width, int height) {
    constexpr float margin=20.0f, sidebar=300.0f, gap=24.0f;
    const float area_width=std::max(1.0f,static_cast<float>(width)-sidebar-gap-2.0f*margin);
    const float area_height=std::max(1.0f,static_cast<float>(height)-2.0f*margin);
    const float side=std::min(area_width,area_height);
    return {margin+(area_width-side)/2.0f,(static_cast<float>(height)-side)/2.0f,side};
  }
  std::optional<GridCell> cell_at(float px,float py,const Board& board) const {
    if (side<=0 || px<x || py<y || px>=x+side || py>=y+side) return std::nullopt;
    const auto cell=side/static_cast<float>(board.width());
    GridCell result{static_cast<int>((px-x)/cell),static_cast<int>((py-y)/cell)};
    return board.contains(result)?std::optional<GridCell>{result}:std::nullopt;
  }
};
} // namespace clash_battle_engine::viewer
