#pragma once

#include "cocsim/core.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cstdlib>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cocsim::viewer {

std::string default_asset_relative_path(Kind kind, int level, const std::string& variant = "normal") {
  std::string id = to_string(kind);
  if (kind == Kind::ArcherTower) id = "archer-tower";
  if (kind == Kind::WallBreaker) id = "wall-breaker";
  if (kind == Kind::HogRider) id = "hog-rider";
  if (kind == Kind::SneakyGoblin) id = "sneaky-goblin";
  if (kind == Kind::RootRider) id = "root-rider";
  if (kind == Kind::EagleArtillery) id = "eagle-artillery";
  if (kind == Kind::InfernoTower) id = "inferno-tower";
  if (kind == Kind::AirDefense) id = "air-defense";
  if (kind == Kind::WizardTower) id = "wizard-tower";
  if (kind == Kind::XBow) id = "x-bow";
  if (kind == Kind::BombTower) id = "bomb-tower";
  if (kind == Kind::HiddenTesla) id = "hidden-tesla";
  if (kind == Kind::MultiArcherTower) id = "multi-archer-tower";
  if (kind == Kind::MultiGearTower) id = "multi-gear-tower";
  if (kind == Kind::Scattershot) id = "scattershot";
  if (kind == Kind::ElectroTitan) id = "electro-titan";
  if (kind == Kind::TownHall) return "images/home/town-hall/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::GoldMine) return "images/home/resource-buildings/gold-mine/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::ElixirCollector) return "images/home/resource-buildings/elixir-collector/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::GoldStorage) return "images/home/resource-buildings/gold-storage/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::ElixirStorage) return "images/home/resource-buildings/elixir-storage/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::DarkElixirStorage) return "images/home/resource-buildings/dark-elixir-storage/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::DarkElixirDrill) return "images/home/resource-buildings/dark-elixir-drill/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::ClanCastle) return "images/home/resource-buildings/clan-castle/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::ArmyCamp) return "images/home/army-buildings/army-camp/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::Barracks) return "images/home/army-buildings/barracks/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::DarkBarracks) return "images/home/army-buildings/dark-barracks/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::SpellFactory) return "images/home/army-buildings/spell-factory/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::DarkSpellFactory) return "images/home/army-buildings/dark-spell-factory/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::Laboratory) return "images/home/army-buildings/laboratory/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::Workshop) return "images/home/army-buildings/workshop/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::Blacksmith) return "images/home/army-buildings/blacksmith/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::HeroHall) return "images/home/army-buildings/hero-hall/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::PetHouse) return "images/home/army-buildings/pet-house/normal/level-" + std::to_string(level) + ".png";
  if (kind == Kind::HeroBanner) {
    if (variant == "barbarian_king") return "images/home/army-buildings/hero-banner/barbarian-king.png";
    if (variant == "archer_queen") return "images/home/army-buildings/hero-banner/archer-queen.png";
    if (variant == "grand_warden") return "images/home/army-buildings/hero-banner/grand-warden.png";
    if (variant == "royal_champion") return "images/home/army-buildings/hero-banner/royal-champion.png";
    if (variant == "minion_prince") return "images/home/army-buildings/hero-banner/minion-prince.png";
    if (variant == "dragon_duke") return "images/home/army-buildings/hero-banner/dragon-duke.png";
    return "images/home/army-buildings/hero-banner/empty.png";
  }
  if (kind == Kind::Wall) return "images/home/defenses/wall/normal/level-" + std::to_string(level) + ".png";
  // Traps live in a different asset family from defenses.  Keeping the list
  // here prevents a valid core trap from silently falling back to the
  // `defenses/` path and rendering as a generic rectangle.
  if (kind == Kind::Bomb || kind == Kind::GiantBomb || kind == Kind::AirBomb || kind == Kind::SeekingAirMine || kind == Kind::SpringTrap || kind == Kind::GigaBomb || kind == Kind::TornadoTrap || kind == Kind::SkeletonTrap)
    return "images/home/traps/" + id + "/normal/level-" + std::to_string(level) + ".png";
  // Golemites reuse the Golem artwork in the public source; this is an asset
  // alias, not a distinct hand-drawn placeholder.
  if (kind == Kind::Golemite) id = "golem";
  if (kind == Kind::Barbarian || kind == Kind::Archer || kind == Kind::Giant || kind == Kind::Wizard || kind == Kind::WallBreaker || kind == Kind::Golem || kind == Kind::Golemite || kind == Kind::Pekka || kind == Kind::Balloon || kind == Kind::Dragon || kind == Kind::BabyDragon || kind == Kind::Miner || kind == Kind::Healer || kind == Kind::Goblin || kind == Kind::Minion || kind == Kind::HogRider || kind == Kind::Valkyrie || kind == Kind::DragonRider || kind == Kind::SneakyGoblin || kind == Kind::RootRider || kind == Kind::Thrower || kind == Kind::ElectroTitan)
    return "images/home/troops/" + id + "/normal/level-" + std::to_string(level) + ".png";
  return "images/home/defenses/" + id + "/normal/level-" + std::to_string(level) + ".png";
}

std::string asset_relative_path(const GameData& data, Kind kind, int level, const std::string& variant = "normal") {
  // The catalogue owns the artwork association. Levels can reuse a preceding
  // visual tier, so deriving a `level-N.png` filename is not reliable.
  const Kind image_kind = kind == Kind::Golemite ? Kind::Golem : kind;
  if (const auto* metadata = data.find_non_combat(image_kind); metadata && !metadata->image.empty()
      && !(image_kind == Kind::HeroBanner && variant != "normal")) return metadata->image;
  for (int candidate_level = level; candidate_level >= 1; --candidate_level) {
    if (const auto* stats = data.find(image_kind, candidate_level, variant);
        stats && !stats->attributes.image.empty()) return stats->attributes.image;
  }
  // A supplemental numerical level can postdate the pinned visual snapshot.
  // This fallback remains explicit and uses the most precise path available.
  return default_asset_relative_path(image_kind, level, variant);
}

std::string initial_asset_root(const GameData& data, int argc, char** argv) {
  for (int i = 1; i + 1 < argc; ++i) if (std::string(argv[i]) == "--assets") return argv[i + 1];
  if (const char* root = std::getenv("COCSIM_ASSET_ROOT")) return root;
#ifdef COCSIM_PROJECT_ASSET_ROOT
  if (std::filesystem::exists(std::filesystem::path(COCSIM_PROJECT_ASSET_ROOT) / asset_relative_path(data, Kind::Barbarian, 1)))
    return COCSIM_PROJECT_ASSET_ROOT;
#endif
  for (const auto& candidate : {"assets", "data/assets", "."}) {
    if (std::filesystem::exists(std::filesystem::path(candidate) / asset_relative_path(data, Kind::Barbarian, 1)))
      return std::filesystem::absolute(candidate).string();
  }
  return {};
}

struct Images {
  struct Asset { SDL_Texture* texture{}; SDL_FRect source{}; };
  struct Request { Kind kind{}; int level{}; std::string variant{"normal"}; };
  Images(SDL_Renderer* in_renderer, const GameData& in_data) : renderer(in_renderer), data(in_data) {}
  ~Images() { clear(); }
  void set_root(std::string in_root) { clear(); root = std::move(in_root); failures = 0; last_error.clear(); }
  SDL_Texture* get(Kind kind, int level, const std::string& variant = "normal") {
    if (root.empty()) return nullptr;
    const auto file = (std::filesystem::path(root) / asset_relative_path(data, kind, level, variant)).string();
    if (const auto it = cache.find(file); it != cache.end()) return it->second.texture;
    Asset asset;
    // The public sprite canvas is authored with its own visual centre and
    // padding. Rendering a cropped opaque silhouette stretched it to the
    // logical footprint and displaced asymmetric buildings from grid cells.
    // Keep the complete source canvas and fit that canvas to the exact sourced
    // footprint instead; this also removes an expensive alpha-pixel scan.
    if (SDL_Surface* surface = IMG_Load(file.c_str())) {
      asset.source = {0, 0, float(surface->w), float(surface->h)};
      asset.texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_DestroySurface(surface);
    }
    if (asset.texture) ++loaded; else { ++failures; last_error = file + " :: " + SDL_GetError(); }
    const auto [it, _] = cache.emplace(file, asset);
    return it->second.texture;
  }
  const Asset* find_asset(Kind kind, int level, const std::string& variant = "normal") const {
    if (root.empty()) return nullptr;
    const auto file = (std::filesystem::path(root) / asset_relative_path(data, kind, level, variant)).string();
    const auto it = cache.find(file);
    return it == cache.end() ? nullptr : &it->second;
  }
  SDL_Texture* find(Kind kind, int level, const std::string& variant = "normal") const {
    if (const auto* asset = find_asset(kind, level, variant)) return asset->texture;
    return nullptr;
  }
  void queue_scene(const Scenario& scenario) {
    pending.clear(); pending_cursor = 0;
    // The deploy palette must become useful first. Previously every defense
    // (and every repeated wall request) was decoded before the first troop
    // icon, which made the palette appear to fill itself slowly on a cold
    // asset cache. Deduplicate by resolved path: a visual tier shared by
    // multiple placements is a single texture, not a frame of preload debt.
    std::unordered_set<std::string> queued;
    const auto enqueue = [this, &queued](Kind kind, int level, const std::string& variant) {
      if (queued.insert(asset_relative_path(data, kind, level, variant)).second)
        pending.push_back({kind, level, variant});
    };
    for (const auto& slot : scenario.army) enqueue(slot.kind, slot.level, "normal");
    for (const auto& placement : scenario.defenders) enqueue(placement.kind, placement.level, placement.variant);
    for (const auto& obstacle : scenario.non_combat_obstacles) enqueue(obstacle.kind, 1, obstacle.variant);
  }
  void pump_preload(std::size_t count = 1) {
    while (count-- > 0 && pending_cursor < pending.size()) {
      const auto& request = pending[pending_cursor++];
      get(request.kind, request.level, request.variant);
    }
  }
  void clear() { for (auto& [_, asset] : cache) if (asset.texture) SDL_DestroyTexture(asset.texture); cache.clear(); pending.clear(); pending_cursor = 0; loaded = 0; }
  SDL_Renderer* renderer{};
  const GameData& data;
  std::string root;
  std::unordered_map<std::string, Asset> cache;
  std::vector<Request> pending;
  std::size_t pending_cursor{};
  int loaded{};
  int failures{};
  std::string last_error;
};

} // namespace cocsim::viewer
