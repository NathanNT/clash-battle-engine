cmake_minimum_required(VERSION 3.23)
project(CoCSim VERSION 0.2.0 LANGUAGES CXX)
if(MSVC)
  add_compile_options(/FS)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(COCSIM_BUILD_VIEWER "Build the SDL3/ImGui viewer" ON)
option(COCSIM_BUILD_PYTHON "Build the Python adapter" OFF)
option(COCSIM_BUILD_TESTS "Build infrastructure tests" ON)
add_library(cocsim_core STATIC
  src/core/core.cpp
  src/core/detail/json_reader.cpp
  src/core/game_data.cpp
  src/core/movement.cpp
  src/core/scenario_io.cpp
  src/core/snapshot.cpp)
target_include_directories(cocsim_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(cocsim_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(MSVC)
  target_compile_options(cocsim_core PRIVATE /W4 /permissive-)
else()
  target_compile_options(cocsim_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(cocsim src/cli/main.cpp)
target_link_libraries(cocsim PRIVATE cocsim_core)
if(COCSIM_BUILD_TESTS)
  enable_testing()
  add_executable(cocsim_engine_tests tests/empty_engine_tests.cpp)
  target_link_libraries(cocsim_engine_tests PRIVATE cocsim_core)
  add_test(NAME cocsim_engine_tests COMMAND cocsim_engine_tests)
  add_test(NAME cocsim_empty_cli COMMAND cocsim simulate ${CMAKE_CURRENT_SOURCE_DIR}/scenarios/empty.json --until 1600)
  add_test(NAME cocsim_old_content_rejected COMMAND cocsim validate ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/old-content.json)
  set_tests_properties(cocsim_old_content_rejected PROPERTIES WILL_FAIL TRUE)
  add_executable(cocsim_viewer_bridge_tests tests/viewer_bridge_tests.cpp)
  target_include_directories(cocsim_viewer_bridge_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/viewer)
  target_link_libraries(cocsim_viewer_bridge_tests PRIVATE cocsim_core)
  add_test(NAME cocsim_viewer_bridge_tests COMMAND cocsim_viewer_bridge_tests)
  add_test(NAME cocsim_cli_roundtrip COMMAND ${CMAKE_COMMAND}
    -Dexe=$<TARGET_FILE:cocsim>
    -Dscenario=${CMAKE_CURRENT_SOURCE_DIR}/scenarios/empty.json
    -Doutput=${CMAKE_CURRENT_BINARY_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/cli_roundtrip.cmake)
endif()
if(COCSIM_BUILD_VIEWER)
  include(FetchContent)
  FetchContent_Declare(sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG f6864924f76e1a0b4abaefc76ae2ed22b1a8916e
    GIT_SHALLOW TRUE)
  FetchContent_Declare(imgui_source
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG f5befd2d29e66809cd1110a152e375a7f1981f06
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(sdl3 imgui_source)
  add_library(cocsim_imgui STATIC
    ${imgui_source_SOURCE_DIR}/imgui.cpp
    ${imgui_source_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_source_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_source_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_source_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_source_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp)
  target_include_directories(cocsim_imgui PUBLIC ${imgui_source_SOURCE_DIR} ${imgui_source_SOURCE_DIR}/backends)
  target_link_libraries(cocsim_imgui PUBLIC SDL3::SDL3)
  add_executable(cocsim_viewer src/viewer/main.cpp)
  target_link_libraries(cocsim_viewer PRIVATE cocsim_core cocsim_imgui SDL3::SDL3)
  add_custom_command(TARGET cocsim_viewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:cocsim_viewer>)
  if(COCSIM_BUILD_TESTS)
    add_test(NAME cocsim_viewer_startup COMMAND cocsim_viewer --smoke-test)
    set_tests_properties(cocsim_viewer_startup PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy")
  endif()
endif()
if(COCSIM_BUILD_PYTHON)
  set(PYBIND11_FINDPYTHON ON)
  find_package(pybind11 CONFIG REQUIRED)
  pybind11_add_module(_cocsim bindings/module.cpp)
  target_link_libraries(_cocsim PRIVATE cocsim_core)
endif()
