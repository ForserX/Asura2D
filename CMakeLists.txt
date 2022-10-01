cmake_minimum_required(VERSION 3.16)
project(arkane C CXX)

set(CMAKE_CXX_STANDARD 20)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "no tests for box2d" FORCE)

set(ARKANE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ARKANE_FOLDER_PREGEN FOLDER "Pre-generated")
set(ARKANE_FOLDER_THIRD_PARTY FOLDER "3rd-party")
set(ARKANE_FOLDER_ENGINE FOLDER "Engine")
set(ARKANE_FOLDER_GAME FOLDER "Game")

# OPTICK_ENABLED
option(ARKANE_SHIPPING "Shipping build of arkane" OFF)
option(ARKANE_BOX2D_OPTIMIZED "Enable optimized version of Box2D" ON)
option(ARKANE_OPTICK "Enable optick profiler" ON)
option(ARKANE_ASAN "Enable address sanitizer" OFF)
option(ARKANE_MIMALLOC "Enable mimalloc" ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

if (ARKANE_ASAN)
	set(MARL_ASAN ON CACHE BOOL "marl asan" FORCE)
elseif (ARKANE_SHIPPING)
	set(ARKANE_OPTICK OFF CACHE BOOL "force optick" FORCE)
endif()

set(BUILD_SHARED_LIBS FALSE CACHE BOOL "shared libraries" FORCE)
#set(SDL2IMAGE_BUILD_SHARED_LIBS FALSE CACHE BOOL "shared libraries" FORCE)
set(OPTICK_ENABLED ${ARKANE_OPTICK} CACHE BOOL "optick enabled" FORCE)
set(MIMALLOC_ENABLED ${ARKANE_MIMALLOC} CACHE BOOL "mimalloc enabled" FORCE)

if (MIMALLOC_ENABLED)
	add_compile_definitions("MIMALLOC_ENABLED")
endif()

if (OPTICK_ENABLED)
	add_compile_definitions("OPTICK_ENABLED")
endif()

set(MI_BUILD_SHARED OFF CACHE BOOL "mimalloc enabled" FORCE)
set(MI_BUILD_OBJECT OFF CACHE BOOL "mimalloc enabled" FORCE)
set(MI_BUILD_TESTS OFF CACHE BOOL "mimalloc enabled" FORCE)
set(MI_OVERRIDE OFF CACHE BOOL "mimalloc enabled" FORCE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bins/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bins/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bins/bin) 

add_subdirectory("libs/optick")
add_subdirectory("libs/mimalloc")
add_subdirectory("libs/SDL")
add_subdirectory("libs/SDL_image")
add_subdirectory("libs/imgui")
add_subdirectory("libs/entt")
add_subdirectory("libs/marl")

if (ARKANE_BOX2D_OPTIMIZED)
	add_subdirectory("libs/box2d-optimized")
else()
	add_subdirectory("libs/box2d")
endif()


add_subdirectory("src")

