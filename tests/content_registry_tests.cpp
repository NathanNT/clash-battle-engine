#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <iostream>
#include <algorithm>

using namespace cocsim;

int main() {
  for (int index = 0; index < static_cast<int>(Kind::Count); ++index) {
    const auto kind = static_cast<Kind>(index);
    const auto serialized = to_string(kind);
    COCSIM_REQUIRE(serialized != "invalid");
    COCSIM_REQUIRE(parse_kind(serialized) == kind);
  }
  for (int index = 0; index < static_cast<int>(SpellKind::Count); ++index) {
    const auto spell = static_cast<SpellKind>(index);
    const auto serialized = to_string(spell);
    COCSIM_REQUIRE(serialized != "invalid");
    COCSIM_REQUIRE(parse_spell(serialized) == spell);
  }
  COCSIM_REQUIRE(!parse_kind("not_a_kind"));
  COCSIM_REQUIRE(!parse_spell("not_a_spell"));
  const auto data = GameData::v0();
  const auto& support = data.hero_support_catalogue();
  COCSIM_REQUIRE(support.size() == 54);
  COCSIM_REQUIRE(std::count_if(support.begin(), support.end(), [](const auto& row) {
    return row.category == "pet" && row.support == "catalogued_only";
  }) == 12);
  COCSIM_REQUIRE(std::count_if(support.begin(), support.end(), [](const auto& row) {
    return row.category == "hero-equipment" && row.support == "catalogued_only";
  }) == 42);
  const auto rocket_backpack = std::find_if(support.begin(), support.end(), [](const auto& row) {
    return row.id == "rocket_backpack";
  });
  COCSIM_REQUIRE(rocket_backpack != support.end());
  COCSIM_REQUIRE(rocket_backpack->hero == "dragon_duke");
  COCSIM_REQUIRE(rocket_backpack->max_level == 27);
  COCSIM_REQUIRE(rocket_backpack->ability_type == "Active");
  const auto frosty = std::find_if(support.begin(), support.end(), [](const auto& row) {
    return row.id == "frosty";
  });
  COCSIM_REQUIRE(frosty != support.end());
  COCSIM_REQUIRE(frosty->max_level == 15);
  COCSIM_REQUIRE(frosty->ability_type == "not_applicable");
  COCSIM_REQUIRE(std::count_if(support.begin(), support.end(), [](const auto& row) {
    return row.category == "hero-equipment" && row.ability_type == "unverified";
  }) == 0);
  const auto electro_fangs = std::find_if(support.begin(), support.end(), [](const auto& row) {
    return row.id == "electro_fangs";
  });
  COCSIM_REQUIRE(electro_fangs != support.end());
  COCSIM_REQUIRE(electro_fangs->ability_type == "Passive");
  const auto monolith_arrow = std::find_if(support.begin(), support.end(), [](const auto& row) {
    return row.id == "monolith_arrow";
  });
  COCSIM_REQUIRE(monolith_arrow != support.end());
  COCSIM_REQUIRE(monolith_arrow->hero == "archer_queen");
  COCSIM_REQUIRE(monolith_arrow->ability_type == "Passive");
  COCSIM_REQUIRE(monolith_arrow->normalized_content_json.find("clash-ninja-2026-06-03-monolith-arrow-passive")
                 != std::string::npos);
  std::size_t support_level_count = 0;
  for (const auto& entry : support) {
    COCSIM_REQUIRE(!entry.normalized_content_json.empty());
    COCSIM_REQUIRE(data.find_hero_support(entry.id) == &entry);
    for (int level = 1; level <= entry.max_level; ++level) {
      const auto* row = data.find_hero_support_level(entry.id, level);
      COCSIM_REQUIRE(row && row->level == level);
      COCSIM_REQUIRE(row->normalized_row_json.find("\"provenance\"") != std::string::npos);
      ++support_level_count;
    }
    COCSIM_REQUIRE(!data.find_hero_support_level(entry.id, entry.max_level + 1));
  }
  COCSIM_REQUIRE(support_level_count == 1073);
  COCSIM_REQUIRE(!data.find_hero_support("temporary_nonexistent_pet"));
  std::cout << "content registry tests passed\n";
}
