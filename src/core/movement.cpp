#include "cocsim/core.hpp"
#include "detail/battle_state_internal.hpp"
#include "detail/geometry.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

namespace cocsim {

using detail::dist;

std::optional<Vec2> BattleState::next_path_waypoint(const Entity& attacker, const Entity& target, EntityId ignored_obstacle, bool jumps_walls) const {
  const int width=scenario_.width,height=scenario_.height;
  if(width<=0||height<=0) return std::nullopt;
  const auto index=[width](int x,int y){return y*width+x;};
  std::vector<std::uint8_t> blocked(static_cast<std::size_t>(width*height),0);
  for(const auto& obstacle:entities_) if(alive(obstacle)&&obstacle.side==Side::Defender&&obstacle.id!=target.id&&obstacle.id!=ignored_obstacle) {
    const auto* stats=data_.find(obstacle.kind,obstacle.level);
    // Traps are deployed on a tile but do not occupy it as a building. They
    // must be crossed to trigger; treating them as walls lets pathfinding
    // evade them before they can fire.
    if(stats&&stats->category==EntityCategory::Trap) continue;
    const bool jump_open=obstacle.kind==Kind::Wall&&std::any_of(spell_effects_.begin(),spell_effects_.end(),[&](const auto& effect){return effect.kind==SpellKind::Jump&&dist(obstacle.pos,effect.pos)<=effect.radius;});
    if(jump_open || (jumps_walls && obstacle.kind==Kind::Wall)) continue;
    const int footprint_w=stats?stats->footprint_width:1, footprint_h=stats?stats->footprint_height:1;
    const int first_x=static_cast<int>(std::llround(obstacle.pos.x-footprint_w/2.0));
    const int first_y=static_cast<int>(std::llround(obstacle.pos.y-footprint_h/2.0));
    for(int y=first_y;y<first_y+footprint_h;++y) for(int x=first_x;x<first_x+footprint_w;++x) if(x>=0&&x<width&&y>=0&&y<height) blocked[static_cast<std::size_t>(index(x,y))]=1;
  }
  // Core uses continuous positions even though the navigation occupancy is
  // tiled. Test a segment against the expanded occupied cells so a troop can
  // walk directly (or diagonally around a corner) without visually following
  // the grid, while its sourced radius still cannot cut through a building.
  const auto segment_clear=[&](Vec2 from,Vec2 to) {
    const double padding=std::max(0.0,attacker.radius-1e-6);
    const double dx=to.x-from.x,dy=to.y-from.y;
    for(int y=0;y<height;++y) for(int x=0;x<width;++x) if(blocked[static_cast<std::size_t>(index(x,y))]) {
      const double min_x=double(x)-padding,max_x=double(x+1)+padding;
      const double min_y=double(y)-padding,max_y=double(y+1)+padding;
      double enter=0.0,leave=1.0;
      const auto clip=[&](double origin,double delta,double low,double high) {
        if(std::abs(delta)<1e-12) return origin<low||origin>high;
        double first=(low-origin)/delta,last=(high-origin)/delta;
        if(first>last) std::swap(first,last);
        enter=std::max(enter,first);leave=std::min(leave,last);
        return enter>leave;
      };
      if(!clip(from.x,dx,min_x,max_x)&&!clip(from.y,dy,min_y,max_y)) return false;
    }
    return true;
  };
  const int start_x=std::clamp(static_cast<int>(std::floor(attacker.pos.x)),0,width-1);
  const int start_y=std::clamp(static_cast<int>(std::floor(attacker.pos.y)),0,height-1);
  const auto* target_stats=data_.find(target.kind,target.level);
  const double target_radius=target_stats?std::max(target_stats->footprint_width,target_stats->footprint_height)/2.0:target.radius;
  const double reach=attacker.range+attacker.radius+target_radius+0.75;
  std::vector<std::uint8_t> goal(static_cast<std::size_t>(width*height),0);
  for(int y=0;y<height;++y) for(int x=0;x<width;++x) {
    const int cell=index(x,y);
    if(!blocked[static_cast<std::size_t>(cell)]&&dist({x+.5,y+.5},target.pos)<=reach) goal[static_cast<std::size_t>(cell)]=1;
  }
  const int start=index(start_x,start_y);
  if(goal[static_cast<std::size_t>(start)]) return target.pos;
  // An unobstructed target is never quantised through cell centres. This is
  // the common Home Village case and removes the visible L-shaped movement.
  if(segment_clear(attacker.pos,target.pos)) return target.pos;
  constexpr int inf=1'000'000;
  std::vector<int> cost(static_cast<std::size_t>(width*height),inf),parent(static_cast<std::size_t>(width*height),-1);
  // All movement edges have the same cost. A min-heap keeps exactly the old
  // deterministic `(cost, tile-index)` selection order without the former
  // O(tile_count) scan for every expanded tile.
  using OpenNode=std::pair<int,int>; // (cost, tile index)
  std::priority_queue<OpenNode,std::vector<OpenNode>,std::greater<OpenNode>> open;
  cost[static_cast<std::size_t>(start)]=0;open.emplace(0,start);
  int reached=-1;
  while(!open.empty()) {
    const auto [current_cost,current]=open.top();open.pop();
    if(current_cost!=cost[static_cast<std::size_t>(current)]) continue;
    if(goal[static_cast<std::size_t>(current)]) {reached=current;break;}
    const int x=current%width,y=current/width;
    constexpr int dx[]={0,-1,1,0},dy[]={-1,0,0,1};
    for(int n=0;n<4;++n) { const int nx=x+dx[n],ny=y+dy[n]; if(nx<0||nx>=width||ny<0||ny>=height) continue; const int next=index(nx,ny); if(blocked[static_cast<std::size_t>(next)]) continue; const int next_cost=current_cost+1; if(next_cost<cost[static_cast<std::size_t>(next)]) {cost[static_cast<std::size_t>(next)]=next_cost;parent[static_cast<std::size_t>(next)]=current;open.emplace(next_cost,next);} }
  }
  if(reached<0) return std::nullopt;
  std::vector<int> route;
  for(int node=reached;node!=start;node=parent[static_cast<std::size_t>(node)]) route.push_back(node);
  std::reverse(route.begin(),route.end());
  // Keep the deterministic Dijkstra route as the topological authority, then
  // skip all intermediate corners visible from the troop's continuous
  // position. This is an any-angle steering layer, not a GUI interpolation:
  // the selected waypoint remains serialised and is advanced only by kTickMs.
  int step=route.front();
  for(const int candidate:route) {
    const Vec2 point{candidate%width+.5,candidate/width+.5};
    if(!segment_clear(attacker.pos,point)) break;
    step=candidate;
  }
  return Vec2{step%width+.5,step/width+.5};
}
EntityId BattleState::blocking_wall(const Entity& attacker, const Entity& target) const {
  const Entity* best=nullptr;
  for(const auto& wall:entities_) if(alive(wall)&&wall.side!=attacker.side&&wall.kind==Kind::Wall) {
    if(!next_path_waypoint(attacker,target,wall.id)) continue;
    if(!best||dist(attacker.pos,wall.pos)<dist(attacker.pos,best->pos)||(dist(attacker.pos,wall.pos)==dist(attacker.pos,best->pos)&&wall.id<best->id)) best=&wall;
  }
  return best?best->id:0;
}

} // namespace cocsim

