#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {
void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}
std::vector<Milliseconds> projectile_times(const BattleState& battle) {
  std::vector<Milliseconds> result;
  for (const auto& event : battle.events())
    if (event.type==EventType::Projectile && event.actor==1) result.push_back(event.time_ms);
  return result;
}
} // namespace

int main() {
  const auto data=GameData::v0();
  const auto* tower=data.find(Kind::MultiGearTower,1);
  COCSIM_REQUIRE(tower && tower->hp==4000 && tower->damage==350 && tower->range==12 && tower->cooldown==1008);
  COCSIM_REQUIRE(tower->burst_damage==156 && tower->burst_range==8 && tower->burst_shots==4);
  COCSIM_REQUIRE(tower->burst_cooldown==192 && tower->burst_pause==384);

  // Officially, either selected mode targets one air or ground unit.  The
  // long-range normal mode retains the catalogue's range/cadence; projectile
  // flight is deliberately the shared logical T+16 ms contract.
  Scenario normal; normal.width=24; normal.height=20; normal.duration_ms=3000;
  normal.defenders={{Kind::MultiGearTower,1,{17.5,10.5}}}; normal.army={{Kind::Golem,1,1}};
  const std::vector<Command> normal_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Golem,1,{5.5,10.5}}};
  BattleState long_range(data,normal); submit_all(long_range,normal_commands); long_range.advance_ticks(2);
  COCSIM_REQUIRE(projectile_times(long_range)==std::vector<Milliseconds>{kTickMs});
  const auto snapshot=long_range.snapshot(); BattleState restored(data,normal); COCSIM_REQUIRE(restored.restore(snapshot));

  // Fast Attack uses the published four-shot burst.  192 and 383 ms cannot be
  // represented by the fixed 16-ms Core tick, so GameData's deterministic
  // nearest-tick conversion is asserted rather than hidden in an adapter.
  Scenario fast=normal; fast.defenders={{Kind::MultiGearTower,1,{17.5,10.5},"fast_attack"}}; fast.army={{Kind::Dragon,1,1}};
  const std::vector<Command> fast_commands={{CommandType::Deploy,kTickMs,0,0,Kind::Dragon,1,{10.5,10.5}}};
  BattleState burst(data,fast); submit_all(burst,fast_commands); burst.advance_ticks(50);
  const std::vector<Milliseconds> expected_burst_times{16,208,400,592};
  COCSIM_REQUIRE(projectile_times(burst)==expected_burst_times);
  COCSIM_REQUIRE(std::all_of(burst.events().begin(),burst.events().end(),[](const Event& event) {
    return event.type!=EventType::Projectile || std::abs(event.value-156)<1e-9;
  }));
  std::string error; const std::string replay_path="replay-multi-gear-tower-test.json";
  COCSIM_REQUIRE(save_replay(replay_path,fast,fast_commands,error)); Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error)); BattleState replay(data,replay_scenario); submit_all(replay,replay_commands); replay.advance_ticks(50);
  long_range.advance_ticks(1); restored.advance_ticks(1);
  COCSIM_REQUIRE(long_range.state_hash()==restored.state_hash() && burst.state_hash()==replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
