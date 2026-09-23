#include "cocsim/core.hpp"
#include "detail/json_reader.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace cocsim {
namespace {

using detail::array_body;
using detail::number_field;
using detail::object_after_key;
using detail::objects_in;
using detail::read_file;
using detail::string_field;

bool scenario_from_json(const std::string& document, Scenario& output, std::string& error) {
  const auto width = number_field(document, "width");
  const auto height = number_field(document, "height");
  const auto seed = number_field(document, "seed");
  const auto duration = number_field(document, "duration_ms");
  if (!width || !height || !seed || !duration || *width <= 0 || *height <= 0 || *duration <= 0) {
    error = "scenario requires positive width, height, seed and duration_ms";
    return false;
  }

  Scenario scenario;
  scenario.format_version = static_cast<int>(number_field(document, "format_version").value_or(1));
  scenario.width = static_cast<int>(*width);
  scenario.height = static_cast<int>(*height);
  scenario.seed = static_cast<std::uint64_t>(*seed);
  scenario.duration_ms = static_cast<Milliseconds>(*duration);

  const auto defenders = array_body(document, "defenders");
  const auto army = array_body(document, "army");
  if (!defenders || !army) {
    error = "scenario requires defenders and army arrays";
    return false;
  }
  for (const auto& object : objects_in(*defenders)) {
    const auto name = string_field(object, "kind");
    const auto level = number_field(object, "level");
    const auto x = number_field(object, "x");
    const auto y = number_field(object, "y");
    const auto kind = name ? parse_kind(*name) : std::nullopt;
    if (!kind || !level || !x || !y) { error = "invalid defender"; return false; }
    scenario.defenders.push_back({*kind, static_cast<int>(*level), {*x, *y},
                                  string_field(object, "mode").value_or("normal"),
                                  string_field(object, "variant").value_or("normal")});
  }
  if (const auto obstacles = array_body(document, "non_combat_obstacles")) {
    for (const auto& object : objects_in(*obstacles)) {
      const auto name = string_field(object, "kind");
      const auto x = number_field(object, "x");
      const auto y = number_field(object, "y");
      const auto kind = name ? parse_kind(*name) : std::nullopt;
      if (!kind || !x || !y) { error = "invalid non-combat obstacle"; return false; }
      scenario.non_combat_obstacles.push_back({*kind, {*x, *y}, string_field(object, "variant").value_or("normal")});
    }
  }
  for (const auto& object : objects_in(*army)) {
    const auto name = string_field(object, "kind");
    const auto level = number_field(object, "level");
    const auto count = number_field(object, "count");
    const auto kind = name ? parse_kind(*name) : std::nullopt;
    if (!kind || !level || !count || *count < 0) { error = "invalid army slot"; return false; }
    scenario.army.push_back({*kind, static_cast<int>(*level), static_cast<int>(*count),
                            string_field(object, "mode").value_or("normal")});
  }
  // Spells stay optional so version-1 scenarios remain readable.
  if (const auto spells = array_body(document, "spells")) {
    for (const auto& object : objects_in(*spells)) {
      const auto name = string_field(object, "kind");
      const auto level = number_field(object, "level");
      const auto count = number_field(object, "count");
      const auto kind = name ? parse_spell(*name) : std::nullopt;
      if (!kind || !level || !count || *count < 0) { error = "invalid spell slot"; return false; }
      scenario.spells.push_back({*kind, static_cast<int>(*level), static_cast<int>(*count)});
    }
  }
  if (const auto loadouts = array_body(document, "hero_loadouts")) {
    for (const auto& object : objects_in(*loadouts)) {
      const auto name = string_field(object, "hero");
      const auto hero = name ? parse_kind(*name) : std::nullopt;
      if (!hero) { error = "invalid Hero loadout"; return false; }
      HeroLoadout loadout;
      loadout.hero = *hero;
      if (object.find("\"pet\"") != std::string::npos) {
        const auto pet = object_after_key(object, "pet");
        const auto id = pet ? string_field(*pet, "id") : std::nullopt;
        const auto level = pet ? number_field(*pet, "level") : std::nullopt;
        if (!id || !level || !std::isfinite(*level) || *level < 1 ||
            *level > std::numeric_limits<int>::max() || *level != static_cast<int>(*level)) {
          error = "invalid Hero Pet choice"; return false;
        }
        loadout.pet = HeroSupportChoice{*id, static_cast<int>(*level)};
      }
      if (object.find("\"equipment\"") != std::string::npos) {
        const auto equipment = array_body(object, "equipment");
        if (!equipment) { error = "invalid Hero Equipment list"; return false; }
        for (const auto& item : objects_in(*equipment)) {
          const auto id = string_field(item, "id");
          const auto level = number_field(item, "level");
          if (!id || !level || !std::isfinite(*level) || *level < 1 ||
              *level > std::numeric_limits<int>::max() || *level != static_cast<int>(*level)) {
            error = "invalid Hero Equipment choice"; return false;
          }
          loadout.equipment.push_back({*id, static_cast<int>(*level)});
        }
      }
      scenario.hero_loadouts.push_back(std::move(loadout));
    }
  }
  output = std::move(scenario);
  return true;
}

} // namespace

std::string to_json(const std::vector<HeroLoadout>& loadouts) {
  std::ostringstream output;
  output << '[';
  for (std::size_t index = 0; index < loadouts.size(); ++index) {
    const auto& loadout = loadouts[index];
    if (index) output << ',';
    output << "{\"hero\":\"" << to_string(loadout.hero) << '"';
    if (loadout.pet)
      output << ",\"pet\":{\"id\":\"" << detail::escape_json(loadout.pet->id)
             << "\",\"level\":" << loadout.pet->level << '}';
    output << ",\"equipment\":[";
    for (std::size_t item = 0; item < loadout.equipment.size(); ++item) {
      if (item) output << ',';
      output << "{\"id\":\"" << detail::escape_json(loadout.equipment[item].id)
             << "\",\"level\":" << loadout.equipment[item].level << '}';
    }
    output << "]}";
  }
  output << ']';
  return output.str();
}

std::string to_json(const Scenario& scenario) {
  std::ostringstream output;
  output << std::setprecision(17) << "{\"format_version\":3,\"width\":" << scenario.width
         << ",\"height\":" << scenario.height << ",\"seed\":" << scenario.seed
         << ",\"duration_ms\":" << scenario.duration_ms << ",\"defenders\":[";
  for (std::size_t index = 0; index < scenario.defenders.size(); ++index) {
    const auto& placement = scenario.defenders[index];
    if (index) output << ',';
    output << "{\"kind\":\"" << to_string(placement.kind) << "\",\"level\":" << placement.level
           << ",\"x\":" << placement.position.x << ",\"y\":" << placement.position.y;
    if (placement.mode != "normal") output << ",\"mode\":\"" << detail::escape_json(placement.mode) << '"';
    if (placement.variant != "normal") output << ",\"variant\":\"" << detail::escape_json(placement.variant) << '"';
    output << '}';
  }
  output << "],\"non_combat_obstacles\":[";
  for (std::size_t index = 0; index < scenario.non_combat_obstacles.size(); ++index) {
    const auto& placement = scenario.non_combat_obstacles[index];
    if (index) output << ',';
    output << "{\"kind\":\"" << to_string(placement.kind) << "\",\"x\":" << placement.position.x
           << ",\"y\":" << placement.position.y;
    if (placement.variant != "normal") output << ",\"variant\":\"" << detail::escape_json(placement.variant) << '\"';
    output << '}';
  }
  output << "],\"army\":[";
  for (std::size_t index = 0; index < scenario.army.size(); ++index) {
    const auto& slot = scenario.army[index];
    if (index) output << ',';
    output << "{\"kind\":\"" << to_string(slot.kind) << "\",\"level\":" << slot.level
           << ",\"count\":" << slot.count;
    if (slot.mode != "normal") output << ",\"mode\":\"" << detail::escape_json(slot.mode) << '\"';
    output << '}';
  }
  output << "],\"spells\":[";
  for (std::size_t index = 0; index < scenario.spells.size(); ++index) {
    const auto& slot = scenario.spells[index];
    if (index) output << ',';
    output << "{\"kind\":\"" << to_string(slot.kind) << "\",\"level\":" << slot.level
           << ",\"count\":" << slot.count << '}';
  }
  output << ']';
  if (!scenario.hero_loadouts.empty()) output << ",\"hero_loadouts\":" << to_json(scenario.hero_loadouts);
  output << '}';
  return output.str();
}

std::string to_json(const BattleResult& result) {
  std::ostringstream output;
  output << "{\"finished\":" << (result.finished ? "true" : "false")
         << ",\"attackers_win\":" << (result.attackers_win ? "true" : "false")
         << ",\"timed_out\":" << (result.timed_out ? "true" : "false")
         << ",\"time_ms\":" << result.time_ms << ",\"destruction\":" << result.destruction
         << ",\"stars\":" << result.stars << ",\"troops_remaining\":" << result.troops_remaining
         << ",\"remaining_housing_space\":" << result.remaining_housing_space
         << ",\"remaining_time_ms\":" << result.remaining_time_ms
         << ",\"reason\":\"" << detail::escape_json(result.reason) << "\"}";
  return output.str();
}

bool load_scenario(const std::string& path, Scenario& output, std::string& error) {
  return scenario_from_json(read_file(path, error), output, error);
}

bool save_replay(const std::string& path, const Scenario& scenario,
                 const std::vector<Command>& commands, std::string& error) {
  std::ofstream output(path);
  if (!output) { error = "cannot write " + path; return false; }
  output << std::setprecision(17) << "{\"format_version\":5,\"rules_version\":\"v0\",\"tick_ms\":"
         << kTickMs << ",\"scenario\":"
         << to_json(scenario) << ",\"commands\":[";
  for (std::size_t index = 0; index < commands.size(); ++index) {
    const auto& command = commands[index];
    if (index) output << ',';
    const char* type = command.type == CommandType::Deploy ? "deploy"
                     : command.type == CommandType::CastSpell ? "cast_spell"
                     : command.type == CommandType::EndBattle ? "end_battle" : "wait";
    output << "{\"type\":\"" << type << "\",\"requested_ms\":" << command.requested_ms
           << ",\"effective_ms\":" << command.effective_ms << ",\"sequence\":" << command.sequence
           << ",\"level\":" << command.level << ",\"x\":" << command.position.x
           << ",\"y\":" << command.position.y;
    if (command.type == CommandType::CastSpell) output << ",\"spell\":\"" << to_string(command.spell) << '"';
    else if (command.type != CommandType::EndBattle) output << ",\"kind\":\"" << to_string(command.kind) << '"';
    output << '}';
  }
  output << "]}";
  if (!output) { error = "write failed"; return false; }
  return true;
}

bool load_replay(const std::string& path, Scenario& scenario,
                 std::vector<Command>& commands, std::string& error) {
  const auto document = read_file(path, error);
  const auto version = number_field(document, "format_version");
  const auto tick = number_field(document, "tick_ms");
  const auto embedded = object_after_key(document, "scenario");
  const auto list = array_body(document, "commands");
  if (!version || *version != 5 || !embedded || !list) {
    error = "unsupported replay format";
    return false;
  }
  if (!tick || *tick != kTickMs) {
    error = "replay tick duration mismatch (expected 16 ms)";
    return false;
  }
  if (!scenario_from_json(*embedded, scenario, error)) return false;
  std::vector<Command> parsed;
  for (const auto& object : objects_in(*list)) {
    const auto type = string_field(object, "type");
    const auto requested = number_field(object, "requested_ms");
    const auto effective = number_field(object, "effective_ms");
    const auto sequence = number_field(object, "sequence");
    const auto level = number_field(object, "level");
    const auto x = number_field(object, "x");
    const auto y = number_field(object, "y");
    if (!type || !requested || !level || !x || !y) { error = "invalid replay command"; return false; }
    Command command;
    command.requested_ms = static_cast<Milliseconds>(*requested);
    if (*version <= 2 && command.requested_ms == 0) command.requested_ms = kTickMs;
    if (*version >= 3 && effective && *effective != 0) {
      if (!sequence || *effective != *requested || *sequence <= 0) {
        error = "invalid replay command schedule";
        return false;
      }
      command.effective_ms = static_cast<Milliseconds>(*effective);
      command.sequence = static_cast<std::uint64_t>(*sequence);
    }
    command.level = static_cast<int>(*level);
    command.position = {*x, *y};
    if (*type == "deploy" || *type == "wait") {
      const auto name = string_field(object, "kind");
      const auto kind = name ? parse_kind(*name) : std::nullopt;
      if (!kind) { error = "invalid replay troop command"; return false; }
      command.type = *type == "deploy" ? CommandType::Deploy : CommandType::Wait;
      command.kind = *kind;
    } else if (*version >= 2 && *type == "cast_spell") {
      const auto name = string_field(object, "spell");
      const auto spell = name ? parse_spell(*name) : std::nullopt;
      if (!spell) { error = "invalid replay spell command"; return false; }
      command.type = CommandType::CastSpell;
      command.spell = *spell;
    } else if (*version >= 4 && *type == "end_battle") {
      command.type = CommandType::EndBattle;
    } else {
      error = "invalid replay command type";
      return false;
    }
    if (command.requested_ms <= 0 || command.requested_ms % kTickMs != 0) {
      error = "replay command time must be a future multiple of 16 ms";
      return false;
    }
    if (command.effective_ms != 0 && std::any_of(parsed.begin(), parsed.end(), [&](const Command& existing) {
          return existing.sequence == command.sequence;
        })) {
      error = "duplicate replay command sequence";
      return false;
    }
    parsed.push_back(command);
  }
  commands = std::move(parsed);
  return true;
}

} // namespace cocsim
