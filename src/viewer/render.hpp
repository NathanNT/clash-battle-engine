#pragma once
#include "board_layout.hpp"
#include <SDL3/SDL.h>
#include <optional>
namespace cocsim::viewer {
inline void draw_board(SDL_Renderer* renderer,const Board& board,const BoardViewport& viewport,
                       const std::optional<GridCell>& selected) {
  const float tile=viewport.side/static_cast<float>(board.width());
  const SDL_FRect ground{viewport.x,viewport.y,viewport.side,viewport.side};
  SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer,29,45,39,255);
  SDL_RenderFillRect(renderer,&ground);
  constexpr float border=static_cast<float>(kHomeVillageDeploymentBorderTiles);
  const SDL_FRect build_area{
    viewport.x+tile*border,viewport.y+tile*border,
    viewport.side-2.0f*tile*border,viewport.side-2.0f*tile*border
  };
  SDL_SetRenderDrawColor(renderer,55,81,60,255);
  SDL_RenderFillRect(renderer,&build_area);
  for (int i=0;i<=board.width();++i) {
    const bool major=i%5==0;
    SDL_SetRenderDrawColor(renderer,144,174,145,major?75:38);
    const float x=viewport.x+tile*static_cast<float>(i);
    SDL_RenderLine(renderer,x,viewport.y,x,viewport.y+viewport.side);
  }
  for (int i=0;i<=board.height();++i) {
    const bool major=i%5==0;
    SDL_SetRenderDrawColor(renderer,144,174,145,major?75:38);
    const float y=viewport.y+tile*static_cast<float>(i);
    SDL_RenderLine(renderer,viewport.x,y,viewport.x+viewport.side,y);
  }
  SDL_SetRenderDrawColor(renderer,165,199,164,190);
  SDL_RenderRect(renderer,&ground);
  SDL_SetRenderDrawColor(renderer,186,214,157,160);
  SDL_RenderRect(renderer,&build_area);
  if (selected && board.contains(*selected)) {
    const SDL_FRect cell{
      viewport.x+tile*static_cast<float>(selected->x),
      viewport.y+tile*static_cast<float>(selected->y),tile,tile
    };
    SDL_SetRenderDrawColor(renderer,255,218,95,90);
    SDL_RenderFillRect(renderer,&cell);
    SDL_SetRenderDrawColor(renderer,255,218,95,255);
    SDL_RenderRect(renderer,&cell);
  }
}
} // namespace cocsim::viewer
