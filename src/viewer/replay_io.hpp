#pragma once

#include "cocsim/core.hpp"

#include <chrono>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace cocsim::viewer {

struct ViewerReplay {
  Scenario scenario;
  std::vector<Command> commands;
  std::string path;
};

inline bool load_viewer_replay(const std::string& path, ViewerReplay& replay, std::string& error) {
  Scenario scenario;
  std::vector<Command> commands;
  if (!load_replay(path, scenario, commands, error)) return false;
  replay = {std::move(scenario), std::move(commands), path};
  return true;
}

// File naming is presentation I/O only. The contents are the exact
// tick-aligned commands supplied by Core, so wall-clock time cannot affect a
// replayed battle.
inline bool save_finished_viewer_replay(const Scenario& scenario, const std::vector<Command>& commands,
                                        std::string& saved_path, std::string& error) {
  std::error_code filesystem_error;
  const auto directory = std::filesystem::current_path() / "replays";
  std::filesystem::create_directories(directory, filesystem_error);
  if (filesystem_error) {
    error = "cannot create replay directory: " + filesystem_error.message();
    return false;
  }
  const auto stamp = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
  for (int suffix = 0; suffix != 1000; ++suffix) {
    const auto filename = "viewer-replay-" + std::to_string(stamp)
      + (suffix == 0 ? std::string{} : "-" + std::to_string(suffix)) + ".json";
    const auto candidate = directory / filename;
    if (std::filesystem::exists(candidate, filesystem_error)) {
      if (filesystem_error) { error = "cannot inspect replay directory: " + filesystem_error.message(); return false; }
      continue;
    }
    std::string save_error;
    if (!save_replay(candidate.string(), scenario, commands, save_error)) {
      error = save_error;
      return false;
    }
    saved_path = candidate.string();
    return true;
  }
  error = "could not allocate a unique replay filename";
  return false;
}

} // namespace cocsim::viewer
