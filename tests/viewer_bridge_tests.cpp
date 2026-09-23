#include "simulation_bridge.hpp"
#include "board_layout.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#undef assert
#define assert(condition) do { if (!(condition)) { std::fprintf(stderr, "assertion failed: %s at line %d\n", #condition, __LINE__); std::abort(); } } while (false)
using namespace clash_battle_engine;
int main() {
  Board board;
  const auto viewport=viewer::BoardViewport::for_window(1100,850);
  const GridCell origin_cell{0,0};
  assert(viewport.side>650.0f);
  assert(viewport.cell_at(viewport.x+1.0f,viewport.y+1.0f,board)==origin_cell);
  assert(!viewport.cell_at(viewport.x-1.0f,viewport.y,board));
  assert(!viewport.cell_at(viewport.x+viewport.side,viewport.y,board));
  viewer::SimulationBridge bridge;
  assert(bridge.state().time_ms()==0);
  bridge.present_elapsed(1000); assert(bridge.state().time_ms()==0);
  bridge.set_speed(2); bridge.set_paused(false);
  bridge.present_elapsed(8); assert(bridge.state().time_ms()==16);
  bridge.present_elapsed(8); assert(bridge.state().time_ms()==32);
  bridge.set_paused(true); bridge.step(); assert(bridge.state().time_ms()==48);
  bridge.reset(); assert(bridge.state().time_ms()==0);
  std::string error;
  assert(bridge.wait_next_tick(error));
  const auto replay_path=std::string("viewer-bridge-replay.json");
  assert(save_replay(replay_path,bridge.state().scenario(),bridge.state().commands(),error));
  assert(bridge.load_replay_file(replay_path,error));
  assert(bridge.replay_loaded() && bridge.state().time_ms()==0);
  bridge.step(); assert(bridge.state().time_ms()==16);
  assert(bridge.state().events().size()==2);
  bridge.reset(); assert(bridge.state().time_ms()==0 && bridge.replay_loaded());
  bridge.new_scenario(); assert(!bridge.replay_loaded());
  assert(save_replay(replay_path,bridge.state().scenario(),{},error));
  assert(bridge.load_replay_file(replay_path,error));
  assert(bridge.replay_loaded());
  assert(!bridge.wait_next_tick(error));
  return 0;
}
