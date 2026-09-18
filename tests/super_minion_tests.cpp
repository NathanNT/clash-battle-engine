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
bool damaged(const BattleState& battle, EntityId target, double amount) {
  return std::any_of(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && event.target == target
      && std::abs(event.value - amount) < 1e-9;
  });
}
} // namespace

int main() {
  const auto data=GameData::v0();
  const auto* minion=data.find(Kind::SuperMinion,14);
  COCSIM_REQUIRE(minion && minion->hp==2300 && minion->damage==400 && minion->cooldown==1000);
  COCSIM_REQUIRE(minion->opening_long_shot_count==8 && minion->opening_long_shot_range==10.25);
  // Supercell confirms extra Long Shot damage but publishes no multiplier.
  // The active reference makes the immutable 1.0 lower-bound fallback explicit.
  COCSIM_REQUIRE(minion->opening_long_shot_damage_multiplier==1.0);

  Scenario scenario; scenario.width=24; scenario.height=20; scenario.duration_ms=10000;
  for (int index=0; index<9; ++index) scenario.defenders.push_back({Kind::Cannon,1,{11.5,10.5}});
  scenario.army={{Kind::SuperMinion,14,1}};
  const std::vector<Command> commands={{CommandType::Deploy,kTickMs,0,0,Kind::SuperMinion,14,{1.5,10.5}}};
  BattleState battle(data,scenario); submit_all(battle,commands);
  battle.advance_ticks(2); // The first 10.25-tile launch is captured at T+10.
  COCSIM_REQUIRE(damaged(battle,1,400)==false);
  const auto first_projectiles=battle.observe_projectiles();
  COCSIM_REQUIRE(first_projectiles.size()==1 && first_projectiles.front().owner==10);
  battle.advance_ticks(1);
  COCSIM_REQUIRE(damaged(battle,1,400));

  // Eight counter-consuming launches each destroy one overlapping Cannon. The
  // ninth remains beyond the ordinary four-tile range, proving the Core state
  // switches from sourced Long Shot reach to normal movement deterministically.
  battle.advance_ticks(699);
  const auto snapshot=battle.snapshot();
  COCSIM_REQUIRE(snapshot.canonical.starts_with("COCSIM-SNAPSHOT-18\n"));
  BattleState restored(data,scenario); COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash()==battle.state_hash());
  const std::string replay_path="replay-super-minion-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path,scenario,commands,error));
  Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error));
  BattleState replay(data,replay_scenario); submit_all(replay,replay_commands); replay.advance_ticks(702);
  COCSIM_REQUIRE(replay.state_hash()==battle.state_hash());
  battle.advance_ticks(1); restored.advance_ticks(1); replay.advance_ticks(1);
  const auto views=battle.observe();
  const auto deployed=std::find_if(views.begin(),views.end(),[](const EntityView& view) { return view.id==10; });
  COCSIM_REQUIRE(deployed!=views.end() && deployed->position.x>1.5);
  COCSIM_REQUIRE(battle.state_hash()==restored.state_hash());
  COCSIM_REQUIRE(battle.state_hash()==replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
