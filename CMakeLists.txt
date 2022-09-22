cmake_minimum_required(VERSION 3.16)
project(arkane C CXX)

set(CMAKE_CXX_STANDARD 20)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "no tests for box2d" FORCE)

set(ARKANE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ARKANE_PREGEN FOLDER "Pre-generated")
set(ARKANE_THIRD_PARTY FOLDER "3rd-party")
set(ARKANE_ENGINE FOLDER "Engine")
set(ARKANE_GAME FOLDER "Game")

# OPTICK_ENABLED
option(ARKANE_SHIPPING "Shipping build of arkane" OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bins/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bins/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bins/bin) 

if (ARKANE_SHIPPING)
	set(OPTICK_ENABLED OFF CACHE BOOL "no tests for box2d" FORCE)
endif ()

add_subdirectory("libs/SDL")
add_subdirectory("libs/imgui")
add_subdirectory("libs/entt")
add_subdirectory("libs/marl")
add_subdirectory("libs/box2d")
add_subdirectory("libs/optick")

add_subdirectory("src")

