#include "cocsim/core.hpp"
#include "test_support.hpp"

#include <iostream>

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
  std::cout << "content registry tests passed\n";
}
