#include "cocsim/core.hpp"
#include <algorithm>
#include <queue>
#include <stdexcept>
namespace cocsim {
Board::Board(int width, int height) : width_(width), height_(height) {
  if (width != kHomeVillageTotalTiles || height != kHomeVillageTotalTiles)
    throw std::invalid_argument("empty ruleset requires the 50x50 Home Village board");
}
bool Board::contains(GridCell c) const { return c.x >= 0 && c.y >= 0 && c.x < width_ && c.y < height_; }
bool Board::contains(Rect r) const {
  return r.width>0 && r.height>0 && r.x>=0 && r.y>=0
    && static_cast<std::int64_t>(r.x)+r.width<=width_
    && static_cast<std::int64_t>(r.y)+r.height<=height_;
}
bool Board::buildable(GridCell c) const {
  constexpr int border = kHomeVillageDeploymentBorderTiles;
  return c.x >= border && c.y >= border && c.x < width_ - border && c.y < height_ - border;
}
bool Board::buildable(Rect r) const {
  return contains(r) && buildable(GridCell{r.x,r.y})
    && buildable(GridCell{r.x+r.width-1,r.y+r.height-1});
}
std::vector<GridCell> Board::shortest_path(GridCell from, GridCell to, const std::vector<Rect>& blocked) const {
  if (!contains(from) || !contains(to)) return {};
  auto index = [this](GridCell c) { return c.y * width_ + c.x; };
  std::vector<int> predecessor(static_cast<std::size_t>(width_ * height_), -1);
  std::queue<GridCell> queue;
  queue.push(from); predecessor[static_cast<std::size_t>(index(from))] = index(from);
  constexpr GridCell directions[]{{0,-1},{-1,0},{1,0},{0,1}};
  while (!queue.empty()) {
    const auto current = queue.front(); queue.pop();
    if (current == to) break;
    for (auto direction : directions) {
      GridCell next{current.x + direction.x, current.y + direction.y};
      if (!contains(next) || predecessor[static_cast<std::size_t>(index(next))] >= 0) continue;
      bool occupied = false;
      for (const auto& r : blocked)
        if (r.width>0 && r.height>0 && next.x>=r.x && next.y>=r.y
          && static_cast<std::int64_t>(next.x)<static_cast<std::int64_t>(r.x)+r.width
          && static_cast<std::int64_t>(next.y)<static_cast<std::int64_t>(r.y)+r.height) {
          occupied=true; break;
        }
      if (occupied) continue;
      predecessor[static_cast<std::size_t>(index(next))] = index(current);
      queue.push(next);
    }
  }
  if (predecessor[static_cast<std::size_t>(index(to))] < 0) return {};
  std::vector<GridCell> path;
  for (int at = index(to); at != index(from); at = predecessor[static_cast<std::size_t>(at)]) path.push_back({at % width_, at / width_});
  path.push_back(from);
  std::reverse(path.begin(), path.end());
  return path;
}
} // namespace cocsim
