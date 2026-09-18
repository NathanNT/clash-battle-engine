cmake_minimum_required(VERSION 3.23)
project(CoCSim VERSION 0.1.0 LANGUAGES CXX)

# SDL compiles many translation units. /Z7 avoids MSVC's shared-PDB race in
# parallel project builds while retaining usable Debug symbols.
if(MSVC)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")
endif()

option(COCSIM_BUILD_VIEWER "Build the SDL3/ImGui viewer (requires packages installed by caller)" OFF)
option(COCSIM_BUILD_PYTHON "Build pybind11 bindings (requires pybind11 installed)" OFF)
option(COCSIM_BUILD_TESTS "Build C++ tests" ON)
set(COCSIM_INTERNAL_AUDITS_DEFAULT OFF)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/README.md"
   AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/state/current.md"
   AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data/reference")
  set(COCSIM_INTERNAL_AUDITS_DEFAULT ON)
endif()
option(COCSIM_BUILD_INTERNAL_AUDITS "Register private provenance and progress audits when their inputs are available" ${COCSIM_INTERNAL_AUDITS_DEFAULT})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(cocsim_core STATIC
  src/core/content_registry.cpp
  src/core/combat.cpp
  src/core/damage.cpp
  src/core/detail/json_reader.cpp
  src/core/game_data.cpp
  src/core/movement.cpp
  src/core/projectiles.cpp
  src/core/scenario_io.cpp
  src/core/snapshot.cpp
  src/core/spawning.cpp
  src/core/spells.cpp
  src/core/targeting.cpp
  src/core/traps.cpp
  src/core/core.cpp)
target_include_directories(cocsim_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_definitions(cocsim_core PRIVATE COCSIM_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(cocsim_core PUBLIC cxx_std_20)
set_target_properties(cocsim_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(MSVC)
  target_compile_options(cocsim_core PRIVATE /W4 /permissive-)
else()
  target_compile_options(cocsim_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

add_executable(cocsim src/cli/main.cpp)
target_link_libraries(cocsim PRIVATE cocsim_core)

if(COCSIM_BUILD_TESTS)
  enable_testing()
  function(add_cocsim_test target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE cocsim_core)
    add_test(NAME ${target} COMMAND ${target})
  endfunction()
  add_cocsim_test(cocsim_tests tests/core_tests.cpp)
  add_cocsim_test(cocsim_content_registry_tests tests/content_registry_tests.cpp)
  add_cocsim_test(cocsim_scenario_io_tests tests/scenario_io_tests.cpp)
  add_cocsim_test(cocsim_determinism_tests tests/determinism_tests.cpp)
  add_cocsim_test(cocsim_deployment_replay_tests tests/deployment_replay_tests.cpp)
  add_cocsim_test(cocsim_targeting_tests tests/targeting_tests.cpp)
  add_cocsim_test(cocsim_permanent_building_tests tests/permanent_building_tests.cpp)
  add_cocsim_test(cocsim_air_defense_tests tests/air_defense_tests.cpp)
  add_cocsim_test(cocsim_monolith_tests tests/monolith_tests.cpp)
  add_cocsim_test(cocsim_super_wall_breaker_tests tests/super_wall_breaker_tests.cpp)
  add_cocsim_test(cocsim_super_barbarian_tests tests/super_barbarian_tests.cpp)
  add_cocsim_test(cocsim_super_giant_tests tests/super_giant_tests.cpp)
  add_cocsim_test(cocsim_super_archer_tests tests/super_archer_tests.cpp)
  add_cocsim_test(cocsim_rocket_balloon_tests tests/rocket_balloon_tests.cpp)
  add_cocsim_test(cocsim_inferno_dragon_tests tests/inferno_dragon_tests.cpp)
  add_cocsim_test(cocsim_super_wizard_tests tests/super_wizard_tests.cpp)
  add_cocsim_test(cocsim_super_minion_tests tests/super_minion_tests.cpp)
  add_cocsim_test(cocsim_super_bowler_tests tests/super_bowler_tests.cpp)
  add_cocsim_test(cocsim_super_dragon_tests tests/super_dragon_tests.cpp)
  add_cocsim_test(cocsim_performance_smoke_tests tests/performance_smoke_tests.cpp)
  set_tests_properties(cocsim_performance_smoke_tests PROPERTIES TIMEOUT 10)
  # This target uses the SDL-free simulation bridge directly. It proves that
  # the GUI's asynchronous ownership boundary can be compiled and exercised
  # in every headless test build, independently of viewer libraries.
  add_executable(cocsim_viewer_thread_tests tests/viewer_thread_tests.cpp)
  target_include_directories(cocsim_viewer_thread_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/viewer)
  target_link_libraries(cocsim_viewer_thread_tests PRIVATE cocsim_core)
  add_test(NAME cocsim_viewer_thread_tests COMMAND cocsim_viewer_thread_tests)
  # Selection is a pure read-model operation: keep its geometry and stable
  # entity-id tie break testable without SDL or a visible window.
  add_executable(cocsim_viewer_selection_tests tests/viewer_selection_tests.cpp)
  target_include_directories(cocsim_viewer_selection_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/viewer)
  target_link_libraries(cocsim_viewer_selection_tests PRIVATE cocsim_core)
  add_test(NAME cocsim_viewer_selection_tests COMMAND cocsim_viewer_selection_tests)
  add_executable(cocsim_viewer_projectile_visual_tests tests/viewer_projectile_visual_tests.cpp)
  target_include_directories(cocsim_viewer_projectile_visual_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/viewer)
  target_link_libraries(cocsim_viewer_projectile_visual_tests PRIVATE cocsim_core)
  add_test(NAME cocsim_viewer_projectile_visual_tests COMMAND cocsim_viewer_projectile_visual_tests)
  # Keep the built-in Viewer scenario SDL-free and independently testable. A
  # malformed default layout must fail in CTest, never through an uncaught GUI
  # construction exception at application launch.
  add_executable(cocsim_viewer_demo_scenario_tests
    tests/viewer_demo_scenario_tests.cpp
    src/viewer/demo_scenario.cpp)
  target_include_directories(cocsim_viewer_demo_scenario_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/viewer)
  target_link_libraries(cocsim_viewer_demo_scenario_tests PRIVATE cocsim_core)
  add_test(NAME cocsim_viewer_demo_scenario_tests COMMAND cocsim_viewer_demo_scenario_tests)
  # Keep the headless moving-projectile fixture parseable by the same CLI that
  # users invoke for replay and diagnosis.
  add_test(NAME projectile_fixture_validation
    COMMAND cocsim validate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/seeking-air-mine-projectile.json)
  # The public headless CLI must construct, target and destroy the passive
  # permanent buildings through the same scenario codec as Viewer and RL.
  add_test(NAME permanent_building_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/permanent-passive-buildings.json
            --deploy barbarian 6 10 --deploy barbarian 6 10 --deploy barbarian 6 10
            --deploy barbarian 6 10 --deploy barbarian 6 10)
  # This fixture crosses the public CLI boundary with a flying target.  It
  # protects Air Defense's air-only route without involving the Viewer clock.
  add_test(NAME air_defense_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/air-defense-air-target.json
            --deploy dragon 6.5 10.5 --deploy dragon 6.5 10.5)
  add_test(NAME monolith_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/monolith-max-hp.json
            --deploy dragon 6.5 10.5)
  add_test(NAME super_wall_breaker_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-wall-breaker-wall.json
            --deploy super_wall_breaker 6.5 10.5)
  add_test(NAME super_barbarian_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-barbarian-rage.json
            --deploy super_barbarian 1.5 10.5)
  add_test(NAME super_giant_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-giant-wall.json
            --deploy super_giant 1.5 10.5)
  add_test(NAME super_archer_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-archer-piercing.json
            --deploy super_archer 1.5 10.5)
  add_test(NAME rocket_balloon_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/rocket-balloon-boost.json
            --deploy rocket_balloon 1.5 10.5)
  add_test(NAME inferno_dragon_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/inferno-dragon-ramp.json
            --deploy inferno_dragon 1.5 10.5)
  add_test(NAME super_wizard_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-wizard-chain.json
            --deploy super_wizard 1.5 10.5)
  add_test(NAME super_minion_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-minion-long-shot.json
            --deploy super_minion 1.5 10.5)
  add_test(NAME super_bowler_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-bowler-triple-strike.json
            --deploy super_bowler 1.5 10.5)
  add_test(NAME super_dragon_headless_cli_validation
    COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/super-dragon-aggregate.json
            --deploy super_dragon 1.5 10.5)
  find_package(Python3 COMPONENTS Interpreter QUIET)
  if(Python3_Interpreter_FOUND AND COCSIM_BUILD_INTERNAL_AUDITS)
    add_test(NAME th18_reference_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_th18_reference.py)
    add_test(NAME th18_reference_verification_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_th18_reference_verification.py)
    add_test(NAME th18_active_reference_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_th18_active_reference.py)
    add_test(NAME th18_mechanics_audit_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_th18_mechanics_audit.py)
    add_test(NAME super_wizard_mechanics_audit_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_super_wizard_mechanics_audit.py)
    add_test(NAME super_troop_mechanics_audits_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_super_troop_mechanics_audits.py)
    add_test(NAME raw_catalogue_consistency_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_raw_catalogue_consistency.py)
    add_test(NAME th18_documentation_delta_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_th18_documentation_delta.py)
    add_test(NAME th18_permanent_scope_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_th18_permanent_scope.py)
    add_test(NAME th18_rl_catalogue_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_rl_catalogue.py)
    add_test(NAME ranged_classification_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_ranged_classification.py)
    add_test(NAME projectile_audit_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_projectile_audit.py)
    add_test(NAME damage_classification_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_damage_classification.py)
    add_test(NAME viewer_asset_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_viewer_assets.py)
    add_test(NAME headless_gui_isolation_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_headless_gui_isolation.py)
    add_test(NAME project_structure_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_project_structure.py)
    # The audit succeeds when it can report all gaps. It must never convert a
    # non-conforming catalogue entry into a false green result.
    add_test(NAME th18_conformity_audit
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_th18_conformity.py)
  endif()
endif()

if(COCSIM_BUILD_VIEWER)
  include(FetchContent)
  # Exact commits, never a branch/tag: compatible SDL 3.2.8 + Dear ImGui 1.91.9b.
  FetchContent_Declare(sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG f6864924f76e1a0b4abaefc76ae2ed22b1a8916e
    GIT_SHALLOW TRUE)
  FetchContent_Declare(imgui_source
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG f5befd2d29e66809cd1110a152e375a7f1981f06
    GIT_SHALLOW TRUE)
  FetchContent_Declare(sdl3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG 11154afb7855293159588b245b446a4ef09e574f
    GIT_SHALLOW TRUE)
  # Viewer assets are PNG only. Keeping the stb backend avoids pulling codec
  # toolchains (notably NASM through AVIF) into a normal GUI build.
  set(SDLIMAGE_VENDORED ON CACHE BOOL "Use SDL_image's pinned dependency sources" FORCE)
  set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "Use stb for PNG loading" FORCE)
  foreach(format AVIF BMP GIF JPG JXL LBM PCX PNM QOI SVG TGA TIF WEBP XCF XPM XV)
    set(SDLIMAGE_${format} OFF CACHE BOOL "Disabled: CoCSim viewer uses PNG assets only" FORCE)
  endforeach()
  set(SDLIMAGE_PNG ON CACHE BOOL "Enable PNG viewer assets" FORCE)
  # The pinned public asset source uses .png paths but serves WebP-encoded
  # payloads. SDL_image detects the file signature, provided WebP support is
  # compiled in; do not rely on the misleading filename extension.
  set(SDLIMAGE_WEBP ON CACHE BOOL "Decode WebP payloads used by viewer assets" FORCE)
  FetchContent_MakeAvailable(sdl3 imgui_source sdl3_image)
  add_library(cocsim_imgui STATIC
    ${imgui_source_SOURCE_DIR}/imgui.cpp
    ${imgui_source_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_source_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_source_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_source_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_source_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp)
  target_include_directories(cocsim_imgui PUBLIC ${imgui_source_SOURCE_DIR} ${imgui_source_SOURCE_DIR}/backends)
  target_link_libraries(cocsim_imgui PUBLIC SDL3::SDL3)
  add_executable(cocsim_viewer
    src/viewer/demo_scenario.cpp
    src/viewer/main.cpp)
  target_link_libraries(cocsim_viewer PRIVATE cocsim_core SDL3::SDL3 SDL3_image::SDL3_image cocsim_imgui)
  # Resolve the bundled assets independently from the process working directory
  # (double-clicking the executable otherwise starts it in build/viewer/Debug).
  target_compile_definitions(cocsim_viewer PRIVATE COCSIM_PROJECT_ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/assets")
  add_custom_command(TARGET cocsim_viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:cocsim_viewer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_image::SDL3_image> $<TARGET_FILE_DIR:cocsim_viewer>)
  if(TARGET webp)
    add_custom_command(TARGET cocsim_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:webp> $<TARGET_FILE_DIR:cocsim_viewer>)
  endif()
  if(TARGET webpdemux)
    add_custom_command(TARGET cocsim_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:webpdemux> $<TARGET_FILE_DIR:cocsim_viewer>)
  endif()
endif()

if(COCSIM_BUILD_PYTHON)
  # pybind11's modern CMake integration delegates extension creation to
  # FindPython. Request the development module explicitly so this target can
  # be configured from a clean checkout instead of relying on a prior cache.
  find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)
  find_package(pybind11 CONFIG REQUIRED)
  pybind11_add_module(_cocsim bindings/module.cpp)
  target_link_libraries(_cocsim PRIVATE cocsim_core)
  install(TARGETS _cocsim LIBRARY DESTINATION . RUNTIME DESTINATION .)
  if(COCSIM_BUILD_TESTS)
    add_test(NAME th18_rl_runtime_validation
      COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/validate_rl_runtime.py
              --module-dir $<TARGET_FILE_DIR:_cocsim>)
  endif()
endif()
