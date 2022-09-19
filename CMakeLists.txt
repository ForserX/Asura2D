cmake_minimum_required(VERSION 3.16)
project(arkane C CXX)

set(CMAKE_CXX_STANDARD 20)
set(BOX2D_BUILD_TESTBED OFF)

set(ARKANE_PREGEN FOLDER "Pre-generated")
set(ARKANE_THIRD_PARTY FOLDER "3rd-party")
set(ARKANE_ENGINE FOLDER "Engine")
set(ARKANE_GAME FOLDER "Game")

add_subdirectory("libs/SDL")
add_subdirectory("libs/imgui")
add_subdirectory("libs/entt")
add_subdirectory("libs/marl")
add_subdirectory("libs/box2d")

add_subdirectory("src")

