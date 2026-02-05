cmake_minimum_required(VERSION 3.28)
project(tactics
  VERSION 0.1.0
  DESCRIPTION "2D RPG Engine"
  HOMEPAGE_URL "https://github.com/hmatheisen/tactics"
  LANGUAGES CXX)

option(TACTICS_ENABLE_SANITIZERS "Enable sanitizers" OFF)
if(TACTICS_ENABLE_SANITIZERS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -fsanitize=leak")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

include(FetchContent)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.26
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.3
)
FetchContent_MakeAvailable(Catch2)

# SQLite3 is required
# Install with: brew install sqlite3
find_package(SQLite3 REQUIRED)

# Core library (shared between main executable and tests)
set(CORE_SOURCES
  src/Core/Tile.cpp
  src/Core/Grid.cpp
  src/Core/Logger.cpp
  src/Core/Camera.cpp
  src/Core/SQLiteGridRepository.cpp
  src/Core/MapGenerator.cpp
)

add_library(tactics_core ${CORE_SOURCES})
target_include_directories(tactics_core PUBLIC include)
target_link_libraries(tactics_core PUBLIC SQLite::SQLite3 SDL3::SDL3)

# Main executable
set(TACTICS_SOURCES
  src/Core/CameraController.cpp
  src/Core/Cursor.cpp
  src/Core/CursorController.cpp
  src/Core/Engine.cpp
  src/Core/EventBus.cpp
  src/Core/InputManager.cpp
  src/Core/SceneManager.cpp
  src/Core/Sprite.cpp
  src/Core/Texture.cpp
  src/Core/TimeManager.cpp
  src/Core/ZoomController.cpp
  src/Core/Unit.cpp
  src/Core/UnitController.cpp
  src/Scenes/GridScene.cpp
  src/main.cpp
)

add_executable(tactics ${TACTICS_SOURCES})

target_include_directories(tactics PRIVATE include)
target_link_libraries(tactics PRIVATE tactics_core SDL3::SDL3)

# Tests
enable_testing()

set(TEST_SOURCES
  tests/Core/Vector2Test.cpp
  tests/Core/RectTest.cpp
  tests/Core/GridRepositoryTest.cpp
  tests/Core/MapGeneratorTest.cpp
)

add_executable(tactics_tests ${TEST_SOURCES})

target_include_directories(tactics_tests PRIVATE include)
target_link_libraries(tactics_tests PRIVATE tactics_core Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tactics_tests)
