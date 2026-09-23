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
int damaged_count(const BattleState& battle, double amount) {
  return static_cast<int>(std::count_if(battle.events().begin(), battle.events().end(), [&](const Event& event) {
    return event.type == EventType::Damaged && std::abs(event.value-amount)<1e-9;
  }));
}
} // namespace

int main() {
  const auto data=GameData::v0();
  const auto* tower=data.find(Kind::SuperWizardTower,2);
  COCSIM_REQUIRE(tower && tower->hp==6300 && tower->damage==416 && tower->cooldown==1296);
  COCSIM_REQUIRE(tower->target_type==TargetType::Both && std::abs(tower->range-8)<1e-9);
  COCSIM_REQUIRE(std::abs(tower->chain_damage_multiplier-.4)<1e-9);
  COCSIM_REQUIRE(tower->chain_target_count==16 && std::abs(tower->chain_radius-4)<1e-9);

  // The source describes a primary strike plus up to 15 nearby branches.
  // Core makes its otherwise unpublished selection deterministic: candidates
  // are measured from the primary, then ordered by distance and entity ID.
  Scenario scenario; scenario.width=20; scenario.height=20; scenario.duration_ms=4000;
  scenario.defenders={{Kind::SuperWizardTower,2,{10.5,10.5}}};
  scenario.army={{Kind::Barbarian,1,17}};
  const std::vector<Vec2> positions={
    {7.5,10.5}, {4.5,7.5}, {4.5,8.5}, {4.5,9.5}, {4.5,10.5}, {4.5,11.5}, {4.5,12.5}, {4.5,13.5},
    {5.5,7.5}, {5.5,8.5}, {5.5,9.5}, {5.5,10.5}, {5.5,11.5}, {5.5,12.5}, {5.5,13.5}, {6.5,9.5}, {6.5,11.5}};
  std::vector<Command> commands;
  for (std::size_t index=0; index<positions.size(); ++index)
    commands.push_back({CommandType::Deploy,kTickMs,0,index,Kind::Barbarian,1,positions[index]});
  BattleState battle(data,scenario); submit_all(battle,commands); battle.advance_ticks(2);
  const auto projectiles=battle.observe_projectiles();
  COCSIM_REQUIRE(std::count_if(projectiles.begin(),projectiles.end(),[](const ProjectileView& projectile){ return projectile.owner==1; })==16);
  const auto snapshot=battle.snapshot();
  BattleState restored(data,scenario); COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash()==battle.state_hash());
  const std::string replay_path="replay-super-wizard-tower-test.json"; std::string error;
  COCSIM_REQUIRE(save_replay(replay_path,scenario,commands,error)); Scenario replay_scenario; std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path,replay_scenario,replay_commands,error)); BattleState replay(data,replay_scenario); submit_all(replay,replay_commands); replay.advance_ticks(2);
  battle.advance_ticks(1); restored.advance_ticks(1); replay.advance_ticks(1);
  COCSIM_REQUIRE(damaged_count(battle,416)==1 && damaged_count(battle,166.4)==15);
  COCSIM_REQUIRE(battle.state_hash()==restored.state_hash() && battle.state_hash()==replay.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str())==0);
}
