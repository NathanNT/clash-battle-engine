#include "cocsim/core.hpp"
#include "render.hpp"
#include "test_support.hpp"

#include <SDL3/SDL.h>

#include <array>
#include <cstdint>
#include <set>
#include <string>

using namespace cocsim;

int main() {
  COCSIM_REQUIRE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
  COCSIM_REQUIRE(SDL_Init(SDL_INIT_VIDEO));
  SDL_Surface* target = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA32);
  COCSIM_REQUIRE(target);
  SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(target);
  COCSIM_REQUIRE(renderer);

  const auto data = GameData::v0();
  viewer::Images images(renderer, data);
  images.set_root(COCSIM_PROJECT_ASSET_ROOT);
  Scenario scenario;
  scenario.width = 30;
  scenario.height = 30;
  scenario.non_combat_obstacles = {{Kind::HeroBanner, {10.0, 10.0}, "normal"}};
  const std::array<const char*, 6> variants = {
      "barbarian_king", "archer_queen", "grand_warden",
      "royal_champion", "minion_prince", "dragon_duke"};
  std::set<std::uint64_t> rendered_pixels;
  for (const char* variant : variants) {
    scenario.non_combat_obstacles[0].variant = variant;
    images.queue_scene(scenario);
    images.pump_preload(1);
    const auto* asset = images.find_asset(Kind::HeroBanner, 1, variant);
    COCSIM_REQUIRE(asset && asset->texture && asset->source.w > 0 && asset->source.h > 0);
    viewer::HeatmapCache heatmap;
    viewer::draw_scene(renderer, scenario, data, images, {}, {}, {}, {}, {},
                       0, heatmap, 0.0f, 0.0f, 4.0f, false, false, false, 0);
    COCSIM_REQUIRE(SDL_RenderPresent(renderer));
    SDL_Surface* pixels = SDL_RenderReadPixels(renderer, nullptr);
    COCSIM_REQUIRE(pixels);
    std::uint64_t hash = 1469598103934665603ULL;
    // The 2x2-tile Banner occupies pixels [36,44) in this fixed test scene.
    for (int y = 36; y < 44; ++y) for (int x = 36; x < 44; ++x) {
      Uint8 red{}, green{}, blue{}, alpha{};
      COCSIM_REQUIRE(SDL_ReadSurfacePixel(pixels, x, y, &red, &green, &blue, &alpha));
      for (const auto channel : {red, green, blue, alpha}) {
        hash ^= channel;
        hash *= 1099511628211ULL;
      }
    }
    rendered_pixels.insert(hash);
    SDL_DestroySurface(pixels);
  }
  COCSIM_REQUIRE(rendered_pixels.size() == variants.size());
  COCSIM_REQUIRE(images.failures == 0);
  images.clear();
  SDL_DestroyRenderer(renderer);
  SDL_DestroySurface(target);
  SDL_Quit();
  return 0;
}
