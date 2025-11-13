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

include(FetchContent)
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.26
)
FetchContent_MakeAvailable(SDL3)

set(TACTICS_SOURCES
  src/main.cpp
  src/Core/Engine.cpp)

add_executable(tactics ${TACTICS_SOURCES})

target_include_directories(tactics PRIVATE include)
target_link_libraries(tactics PRIVATE SDL3::SDL3)
