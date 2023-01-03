cmake_minimum_required(VERSION 3.16)
project(asura C CXX)
if(APPLE)
    enable_language(OBJC)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ASURA_PROJECT_NAME "example")
set(ASURA_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ASURA_FOLDER_PREGEN FOLDER "Pre-generated")
set(ASURA_FOLDER_THIRD_PARTY FOLDER "3rd-party")
set(ASURA_FOLDER_PROFILR FOLDER "profiler")
set(ASURA_FOLDER_ENGINE FOLDER "Engine")
set(ASURA_FOLDER_GAME FOLDER "Game")

option(ASURA_SHIPPING "Shipping build of asura" OFF)
option(ASURA_BOX2D_OPTIMIZED "Enable optimized version of Box2D" OFF)
option(ASURA_OPTICK "Enable optick profiler" OFF)
option(ASURA_ASAN "Enable address sanitizer" OFF)
option(ASURA_MIMALLOC "Enable mimalloc" ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

if (ASURA_ASAN)
elseif (ASURA_SHIPPING)
	set(ASURA_OPTICK OFF CACHE BOOL "force optick" FORCE)
endif()

option(BOX2D_BUILD_UNIT_TESTS "Build the Box2D unit tests" ON)
option(BOX2D_BUILD_TESTBED "Build the Box2D testbed and benchmarks" ON)

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "shared libraries" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "shared libraries" FORCE)
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "shared libraries" FORCE)
set(OPTICK_ENABLED ${ASURA_OPTICK} CACHE BOOL "optick enabled" FORCE)

if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
	include_directories(/usr/local/include)
	set(SDL_VIDEO_DRIVER_X11 ON CACHE BOOL "SDL3 FreeBSD X11 hack" FORCE)
	set(SDL_VIDEO_DRIVER_X11_SUPPORTS_GENERIC_EVENTS ON CACHE BOOL "SDL3 FreeBSD X11 Generic Events hack" FORCE)
endif()

if (NOT ASURA_ASAN)
	set(MIMALLOC_ENABLED ${ASURA_MIMALLOC} CACHE BOOL "mimalloc enabled" FORCE)
else()
	set(MIMALLOC_ENABLED OFF CACHE BOOL "mimalloc enabled" FORCE)
endif()

set(BUILD_TESTING OFF CACHE BOOL "shared libraries" FORCE)

set(ALSOFT_EXAMPLES OFF CACHE BOOL "shared libraries" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "shared libraries" FORCE)
set(ALSOFT_UPDATE_BUILD_VERSION OFF CACHE BOOL "shared libraries" FORCE)
set(AL_LIBTYPE_STATIC ON CACHE BOOL "shared libraries" FORCE)
set(SDL_TEST OFF CACHE BOOL "shared libraries" FORCE)
set(OPUS_BUILD_SHARED_LIBRARY OFF CACHE BOOL "shared libraries" FORCE)
set(OPUS_X86_MAY_HAVE_SSE4_1 ON CACHE BOOL "shared libraries" FORCE)
set(OPUS_X86_PRESUME_AVX ON CACHE BOOL "shared libraries" FORCE)
set(OP_DISABLE_DOCS ON CACHE BOOL "shared libraries" FORCE)
set(OP_DISABLE_EXAMPLES ON CACHE BOOL "shared libraries" FORCE)
set(OP_DISABLE_HTTP ON CACHE BOOL "shared libraries" FORCE)

add_compile_definitions("ENTT_USE_ATOMIC")

if (ASURA_SHIPPING)
	add_compile_definitions("ASURA_SHIPPING")
endif()

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

# Create ImGui CMake file
FILE(WRITE libs/imgui/CMakeLists.txt
"cmake_minimum_required(VERSION 3.16)\n"
"project(asura C CXX)\n\n"
"file(GLOB IMGUI_SRC\n"
"	\"imgui.h\"\n"
"	\"imconfig.h\"\n"
"	\"imgui_internal.h\"\n"
"	\"imgui.cpp\"\n"
"	\"imgui_demo.cpp\"\n"
"	\"imgui_draw.cpp\"\n"
"	\"imgui_tables.cpp\"\n"
"	\"imgui_widgets.cpp\"\n"
"	\"imgui_rectpack.cpp\"\n"
"	\"imgui_textedit.cpp\"\n"
"	\"imgui_truetype.cpp\"\n"
")\n\n"
"file(GLOB IMGUI_BACKEND_SRC\n"
"	\"backends/imgui_impl_sdl.h\"\n"
"	\"backends/imgui_impl_sdlrenderer.h\"\n"
"	\"backends/imgui_impl_sdl.cpp\"\n"
"	\"backends/imgui_impl_sdlrenderer.cpp\"\n"
")\n\n"
"add_library(\"imgui\" STATIC \${IMGUI_SRC} \${IMGUI_BACKEND_SRC})\n"
"target_link_libraries(\"imgui\" PUBLIC SDL2::SDL2-static)\n\n"
"target_include_directories(\"imgui\" PUBLIC \"\")\n"
"target_include_directories(\"imgui\" PUBLIC \"backends\")\n"
)

if (OPTICK_ENABLED)
	add_subdirectory("libs/optick")
endif()

add_subdirectory("libs/mimalloc")
add_subdirectory("libs/SDL")
add_subdirectory("libs/SDL_image")
add_subdirectory("libs/imgui")
add_subdirectory("libs/entt")
add_subdirectory("libs/ogg")
add_subdirectory("libs/opus")
add_subdirectory("libs/opusfile")
add_subdirectory("libs/openal-soft")

if (UNIX) 
	add_subdirectory("libs/fmt")
endif()

if (ASURA_BOX2D_OPTIMIZED)
	add_subdirectory("libs/box2d-opt")
else()
	add_subdirectory("libs/box2d")
endif()

add_subdirectory("src")

FILE(REMOVE libs/imgui/CMakeLists.txt)