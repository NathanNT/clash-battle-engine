#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace cocsim;

namespace {

struct VariantSpec {
  Kind kind;
  int level;
};

const EntityView& view_for(const std::vector<EntityView>& views, Kind kind) {
  const auto it = std::find_if(views.begin(), views.end(), [kind](const EntityView& view) {
    return view.kind == kind;
  });
  COCSIM_REQUIRE(it != views.end());
  return *it;
}

} // namespace

int main() {
  // PERMANENT_VARIANT_SNAPSHOT_REPLAY: every current Core-materialized
  // supercharged family has a separate catalogue record with the same numeric
  // level as its normal record. Exercise the shared placement selector rather
  // than treating the variant as a weapon mode or silently falling back to
  // normal data.
  const std::vector<VariantSpec> variants = {
    {Kind::GoldMine, 1}, {Kind::ElixirCollector, 1}, {Kind::InfernoTower, 1},
    {Kind::AirDefense, 1}, {Kind::XBow, 1}, {Kind::Scattershot, 1},
    {Kind::Monolith, 1}, {Kind::RicochetCannon, 1},
    {Kind::MultiArcherTower, 1}, {Kind::MultiGearTower, 1},
    {Kind::SuperWizardTower, 1}, {Kind::BombTower, 1},
    {Kind::DarkElixirDrill, 1},
  };
  const auto data = GameData::v0();
  Scenario scenario;
  scenario.width = 40;
  scenario.height = 20;
  scenario.duration_ms = 1000;
  for (std::size_t index = 0; index < variants.size(); ++index) {
    const auto& variant = variants[index];
    const auto* stats = data.find(variant.kind, variant.level, "supercharged");
    COCSIM_REQUIRE(stats && stats->attributes.supercharged);
    scenario.defenders.push_back({variant.kind, variant.level,
                                  {4.5 + 5.0 * (index % 7), 4.5 + 8.0 * (index / 7)},
                                  "normal", "supercharged"});
  }

  BattleState battle(data, scenario);
  const auto initial = battle.observe();
  COCSIM_REQUIRE(initial.size() == variants.size());
  for (const auto& variant : variants) {
    const auto* stats = data.find(variant.kind, variant.level, "supercharged");
    const auto& entity = view_for(initial, variant.kind);
    COCSIM_REQUIRE(entity.supercharged && entity.max_hp == stats->hp);
  }

  const auto snapshot = battle.snapshot();
  BattleState restored(data, scenario);
  COCSIM_REQUIRE(restored.restore(snapshot));
  COCSIM_REQUIRE(restored.state_hash() == battle.state_hash());

  const std::string replay_path = "replay-permanent-variants-test.json";
  std::string error;
  const std::vector<Command> commands;
  COCSIM_REQUIRE(save_replay(replay_path, scenario, commands, error));
  Scenario replay_scenario;
  std::vector<Command> replay_commands;
  COCSIM_REQUIRE(load_replay(replay_path, replay_scenario, replay_commands, error));
  COCSIM_REQUIRE(replay_scenario.defenders.size() == variants.size());
  COCSIM_REQUIRE(std::all_of(replay_scenario.defenders.begin(), replay_scenario.defenders.end(),
                             [](const Placement& placement) {
                               return placement.variant == "supercharged";
                             }));
  BattleState replay(data, replay_scenario);
  COCSIM_REQUIRE(replay.state_hash() == battle.state_hash());
  COCSIM_REQUIRE(std::remove(replay_path.c_str()) == 0);
}
