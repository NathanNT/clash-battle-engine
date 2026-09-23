#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace cocsim {
using Milliseconds = std::int64_t;
constexpr Milliseconds kTickMs = 16;
constexpr int kHomeVillageBuildTiles = 44;
constexpr int kHomeVillageDeploymentBorderTiles = 3;
constexpr int kHomeVillageTotalTiles = kHomeVillageBuildTiles + 2 * kHomeVillageDeploymentBorderTiles;
// Saved format identifier. Retained so previously written scenarios and replays load.
constexpr const char* kRulesetId = "empty-16ms-v1";

struct GridCell { int x{}; int y{}; friend bool operator==(const GridCell&, const GridCell&) = default; };
struct Rect { int x{}; int y{}; int width{}; int height{}; };
class Board {
 public:
  Board(int width = kHomeVillageTotalTiles, int height = kHomeVillageTotalTiles);
  int width() const { return width_; }
  int height() const { return height_; }
  bool contains(GridCell cell) const;
  bool contains(Rect area) const;
  bool buildable(GridCell cell) const;
  bool buildable(Rect area) const;
  std::vector<GridCell> shortest_path(GridCell from, GridCell to, const std::vector<Rect>& blocked = {}) const;
 private:
  int width_;
  int height_;
};

// The baseline contains no combat catalogue.
class GameData {
 public:
  static const GameData& baseline();
 private:
  GameData() = default;
};
struct Scenario {
  int width{kHomeVillageTotalTiles};
  int height{kHomeVillageTotalTiles};
  std::uint64_t seed{1};
  Milliseconds duration_ms{1600};
  std::string ruleset{kRulesetId};
};
enum class CommandType { Wait, EndBattle };
struct Command {
  CommandType type{CommandType::Wait};
  Milliseconds requested_at_ms{};
  Milliseconds effective_at_ms{};
  std::uint64_t sequence{};
  friend bool operator==(const Command&, const Command&) = default;
};
enum class EventType { Accepted, Rejected, Executed, Finished };
struct Event {
  EventType type{};
  Milliseconds time_ms{};
  std::uint64_t sequence{};
  std::string detail;
  friend bool operator==(const Event&, const Event&) = default;
};
enum class Result { Active, Ended, TimedOut };
struct Observation {
  Milliseconds time_ms{};
  int width{};
  int height{};
  Result result{Result::Active};
  std::uint64_t state_hash{};
};
class BattleState {
 public:
  explicit BattleState(Scenario scenario = {}, const GameData& data = GameData::baseline());
  const Scenario& scenario() const { return scenario_; }
  const Board& board() const { return board_; }
  Milliseconds time_ms() const { return time_ms_; }
  Result result() const { return result_; }
  const std::vector<Command>& commands() const { return commands_; }
  const std::vector<Event>& events() const { return events_; }
  bool submit(CommandType type, Milliseconds effective_at_ms, std::string& error);
  void advance_ticks(std::uint64_t count);
  bool advance_to(Milliseconds target_ms, std::string& error);
  bool replay_commands(const std::vector<Command>& commands, Milliseconds target_ms, std::string& error);
  Observation observe() const;
  std::uint64_t state_hash() const;
  std::string snapshot() const;
  bool restore(const std::string& bytes, std::string& error);
 private:
  Scenario scenario_;
  Board board_;
  Milliseconds time_ms_{};
  Result result_{Result::Active};
  std::uint64_t next_sequence_{1};
  std::vector<Command> commands_;
  std::vector<Event> events_;
};
std::string scenario_json(const Scenario& scenario);
bool parse_scenario(const std::string& json, Scenario& scenario, std::string& error);
bool load_scenario(const std::string& path, Scenario& scenario, std::string& error);
bool save_scenario(const std::string& path, const Scenario& scenario, std::string& error);
std::string replay_json(const Scenario& scenario, const std::vector<Command>& commands);
bool parse_replay(const std::string& json, Scenario& scenario, std::vector<Command>& commands, std::string& error);
bool load_replay(const std::string& path, Scenario& scenario, std::vector<Command>& commands, std::string& error);
bool save_replay(const std::string& path, const Scenario& scenario, const std::vector<Command>& commands, std::string& error);
bool save_text(const std::string& path, const std::string& text, std::string& error);
bool load_text(const std::string& path, std::string& text, std::string& error);
const char* result_name(Result result);
const char* command_name(CommandType type);
} // namespace cocsim
