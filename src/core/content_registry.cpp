#include "cocsim/core.hpp"

#include <array>
#include <string_view>

namespace cocsim {
namespace {

template <typename Enum, typename Name, std::size_t Size>
std::string enum_name(Enum value, const std::array<Name, Size>& names) {
  const auto index = static_cast<std::size_t>(value);
  return index < names.size() ? std::string(names[index]) : "invalid";
}

constexpr std::array kKindNames{
#define COCSIM_KIND(symbol, serialized_name) std::string_view{serialized_name},
#include "cocsim/detail/content_ids.inc"
#undef COCSIM_KIND
};
static_assert(kKindNames.size() == static_cast<std::size_t>(Kind::Count));

constexpr std::array kSpellNames{
#define COCSIM_SPELL(symbol, serialized_name) std::string_view{serialized_name},
#include "cocsim/detail/spell_ids.inc"
#undef COCSIM_SPELL
};
static_assert(kSpellNames.size() == static_cast<std::size_t>(SpellKind::Count));

constexpr std::array kCategoryNames{"troop", "defense", "resource", "wall", "other", "trap"};
constexpr std::array kFocusNames{"any", "defenses", "resources", "walls", "friendly_troops", "defenses_only"};
constexpr std::array kTargetTypeNames{"ground", "air", "both"};

} // namespace

std::string to_string(Kind kind) { return enum_name(kind, kKindNames); }
std::string to_string(SpellKind kind) { return enum_name(kind, kSpellNames); }
std::string to_string(EntityCategory category) { return enum_name(category, kCategoryNames); }
std::string to_string(TargetFocus focus) { return enum_name(focus, kFocusNames); }
std::string to_string(TargetType target_type) { return enum_name(target_type, kTargetTypeNames); }

std::optional<Kind> parse_kind(const std::string& name) {
  for (std::size_t index = 0; index < kKindNames.size(); ++index) {
    if (kKindNames[index] == name) return static_cast<Kind>(index);
  }
  return std::nullopt;
}

std::optional<SpellKind> parse_spell(const std::string& name) {
  for (std::size_t index = 0; index < kSpellNames.size(); ++index) {
    if (kSpellNames[index] == name) return static_cast<SpellKind>(index);
  }
  return std::nullopt;
}

} // namespace cocsim
