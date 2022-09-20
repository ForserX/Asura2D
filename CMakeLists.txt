cmake_minimum_required(VERSION 3.16)
project(arkane C CXX)

set(CMAKE_CXX_STANDARD 20)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "no tests for box2d" FORCE)

set(ARKANE_PREGEN FOLDER "Pre-generated")
set(ARKANE_THIRD_PARTY FOLDER "3rd-party")
set(ARKANE_ENGINE FOLDER "Engine")
set(ARKANE_GAME FOLDER "Game")

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

add_subdirectory("libs/SDL")
add_subdirectory("libs/imgui")
add_subdirectory("libs/entt")
add_subdirectory("libs/marl")
add_subdirectory("libs/box2d")

add_subdirectory("src")

