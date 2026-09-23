#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

const EntityView& view_for(const std::vector<EntityView>& views, Kind kind) {
  const auto it = std::find_if(views.begin(), views.end(), [kind](const EntityView& view) {
    return view.kind == kind;
  });
  COCSIM_REQUIRE(it != views.end());
  return *it;
}

void submit_all(BattleState& battle, const std::vector<Command>& commands) {
  for (const auto& command : commands) COCSIM_REQUIRE(battle.submit(command));
}

} // namespace

int main() {
  const auto data = GameData::v0();
  const auto* bobs = data.find(Kind::BobsHut, 1);
  const auto* helper = data.find(Kind::HelperHut, 1);
  const auto* crafting_station = data.find(Kind::CraftingStation, 1);
  const auto* xbow = data.find(Kind::XBow, 13);
  COCSIM_REQUIRE(bobs && bobs->hp == 250 && bobs->footprint_width == 2 && bobs->footprint_height == 2);
  COCSIM_REQUIRE(helper && helper->hp == 500 && helper->footprint_width == 3 && helper->footprint_height == 3);
  COCSIM_REQUIRE(crafting_station && crafting_station->hp == 1000 && crafting_station->footprint_width == 3 && crafting_station->footprint_height == 3);
  COCSIM_REQUIRE(xbow && xbow->footprint_width == 3 && xbow->footprint_height == 3);
  COCSIM_REQUIRE(bobs->category == EntityCategory::Other && helper->category == EntityCategory::Other);
  COCSIM_REQUIRE(crafting_station->category == EntityCategory::Defense && crafting_station->damage == 0.0 && crafting_station->range == 0.0);

  Scenario scenario;
  scenario.width = 24;
  scenario.height = 20;
  scenario.duration_ms = 10000;
  scenario.defenders = {
    {Kind::BobsHut, 1, {10.0, 10.0}},
    {Kind::HelperHut, 1, {15.5, 10.5}},
    {Kind::CraftingStation, 1, {20.5, 10.5}},
  };
  scenario.army = {{Kind::Barbarian, 1, 1}};
  const std::vector<Command> commands = {
    {CommandType::Deploy, kTickMs, 0, 0, Kind::Barbarian, 1, {6.0, 10.0}},
  };

  BattleState battle(data, scenario);
  submit_all(battle, commands);
  const auto initial = battle.observe();
  const auto& bobs_view = view_for(initial, Kind::BobsHut);
  const auto& helper_view = view_for(initial, Kind::HelperHut);
  const auto& crafting_station_view = view_for(initial, Kind::CraftingStation);
  COCSIM_REQUIRE(bobs_view.max_hp == 250 && bobs_view.footprint_width == 2 && bobs_view.footprint_height == 2);
  COCSIM_REQUIRE(helper_view.max_hp == 500 && helper_view.footprint_width == 3 && helper_view.footprint_height == 3);
  COCSIM_REQUIRE(crafting_station_view.max_hp == 1000 && crafting_station_view.footprint_width == 3 && crafting_station_view.footprint_height == 3);

  battle.advance_ticks(150);
  COCSIM_REQUIRE(std::any_of(battle.events().begin(), battle.events().end(), [](const Event& event) {
    return event.type == EventType::Damaged && event.target == 1 && event.value > 0;
  }));
  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  battle.advance_ticks(100);
  restored.advance_ticks(100);
  COCSIM_REQUIRE(battle.state_hash() == restored.state_hash());

  const std::string replay_path = "replay-permanent-buildings-test.json";
  std::string error;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  BattleState replay(data, replay_scenario);
  submit_all(replay, replay_commands);
  replay.advance_ticks(250);
  BattleState direct(data, scenario);
  submit_all(direct, commands);
  direct.advance_ticks(250);
  COCSIM_REQUIRE(replay.state_hash() == direct.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
