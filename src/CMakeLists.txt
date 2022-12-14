################################################
#
# Define engine source files
#
################################################
file(GLOB ASURA_ENGINE_CORE_SRC
	"engine/core/*.cpp"
	"engine/core/*.h"
)

file(GLOB ASURA_ENGINE_ECES_SRC
	"engine/eces/*.cpp"
	"engine/eces/*.h"
)

file(GLOB ASURA_ENGINE_ENGINE_SRC
	"engine/engine/*.cpp"
	"engine/engine/*.h"
	"engine/asura_global_vars.h"
	"pch.h"
	"asura.h"
	"asura_config.h"
	"asura_engine.h"
)

file(GLOB ASURA_ENGINE_NET_SRC
	"engine/net/*.cpp"
	"engine/net/*.h"
)

file(GLOB ASURA_ENGINE_PHYSICS_SRC
	"engine/physics/*.cpp"
	"engine/physics/*.h"
)

file(GLOB ASURA_ENGINE_GAMEPLAY_SRC
	"engine/gameplay/*.cpp"
	"engine/gameplay/*.h"
)

file(GLOB ASURA_ENGINE_RENDER_SRC
	"engine/render/*.cpp"
	"engine/render/*.h"
)

file(GLOB ASURA_ENGINE_SYSTEMS_SRC
	"engine/systems/*.cpp"
	"engine/systems/*.h"
)

file(GLOB ASURA_ENGINE_AUDIO_SRC
	"engine/audio/*.cpp"
	"engine/audio/*.h"
)

file(GLOB_RECURSE ASURA_EDITOR_SRC
	"editor/*.cpp"
	"editor/*.h"
)

file(GLOB_RECURSE ASURA_GAME_SRC
	"game/*.cpp"
	"game/*.h"
)

################################################
#
# Define engine source files directories
#
################################################
source_group("editor" FILES ${ASURA_EDITOR_SRC})
source_group("kernel" FILES ${ASURA_ENGINE_ENGINE_SRC})
source_group("engine\\core" FILES ${ASURA_ENGINE_CORE_SRC})
source_group("engine\\audio" FILES ${ASURA_ENGINE_AUDIO_SRC})
source_group("engine\\eces" FILES ${ASURA_ENGINE_ECES_SRC})
source_group("engine\\network" FILES ${ASURA_ENGINE_NET_SRC})
source_group("engine\\physics" FILES ${ASURA_ENGINE_PHYSICS_SRC})
source_group("engine\\render" FILES ${ASURA_ENGINE_RENDER_SRC})
source_group("engine\\systems" FILES ${ASURA_ENGINE_SYSTEMS_SRC})
source_group("gameplay" FILES ${ASURA_ENGINE_GAMEPLAY_SRC})

set(ENGINE_SRC_FILES)
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_ENGINE_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_CORE_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_GAMEPLAY_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_ECES_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_AUDIO_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_NET_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_EDITOR_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_PHYSICS_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_RENDER_SRC})
list(APPEND ENGINE_SRC_FILES ${ASURA_ENGINE_SYSTEMS_SRC})
list(APPEND ENGINE_SRC_FILES ${EDITOR_SRC})
list(APPEND ENGINE_SRC_FILES "pch.h")
list(APPEND ENGINE_SRC_FILES "asura.h")
list(APPEND ENGINE_SRC_FILES "asura_engine.h")

################################################
#
# Define list libraries to link
#
################################################
set(LIBRARIES_TO_LINK)
list(APPEND LIBRARIES_TO_LINK "imgui")
list(APPEND LIBRARIES_TO_LINK "EnTT::EnTT")
#list(APPEND LIBRARIES_TO_LINK "marl")
list(APPEND LIBRARIES_TO_LINK "box2d")

if (OPTICK_ENABLED)
	list(APPEND LIBRARIES_TO_LINK "OptickCore")
endif()

list(APPEND LIBRARIES_TO_LINK "SDL2-static")
list(APPEND LIBRARIES_TO_LINK "SDL2main")
list(APPEND LIBRARIES_TO_LINK "SDL2_image")
list(APPEND LIBRARIES_TO_LINK "OpenAL")
list(APPEND LIBRARIES_TO_LINK "vorbis")
list(APPEND LIBRARIES_TO_LINK "vorbisfile")
list(APPEND LIBRARIES_TO_LINK "ogg")

target_include_directories("vorbis" PUBLIC  
    $<BUILD_INTERFACE:${ASURA_ROOT}/libs/ogg/include>  
    $<INSTALL_INTERFACE:/libs/ogg/include>  # <prefix>/include/mylib
)

if (UNIX) 
	list(APPEND LIBRARIES_TO_LINK "fmt")
endif()

if (MIMALLOC_ENABLED)
	list(APPEND LIBRARIES_TO_LINK "mimalloc-static")
endif()

################################################
#
# Define engine and link libraries to it
#
################################################
add_library("libasura" STATIC ${ENGINE_SRC_FILES})
target_link_libraries("libasura" PUBLIC ${LIBRARIES_TO_LINK})

if (ASURA_ASAN)
	target_compile_options("libasura" PUBLIC "-fsanitize=address")
    target_link_libraries("libasura" PUBLIC "-fsanitize=address")
	target_compile_options("box2d" PUBLIC "-fsanitize=address")
    target_link_libraries("box2d" PUBLIC "-fsanitize=address")

    if (WIN32)
		list(APPEND LIBRARIES_TO_LINK "libsancov.lib")
	endif()
endif()

################################################
#
# Public include pathes for engine library
#
################################################
target_include_directories("libasura" PUBLIC "${ASURA_ROOT}/libs")
target_include_directories("libasura" PUBLIC "engine")
target_include_directories("libasura" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Enable pch.h for engine
target_precompile_headers("libasura" PUBLIC "pch.h")	

# Enable optimized version of box2d
if (ASURA_BOX2D_OPTIMIZED)
    target_compile_definitions("libasura" PUBLIC ASURA_BOX2D_OPTIMIZED)
endif()

################################################
#
# Define game executable
#
################################################
if (WIN32)
	add_executable("example" WIN32 ${ASURA_GAME_SRC} "main.cpp")
else()
	add_executable("example" ${ASURA_GAME_SRC} "main.cpp")
endif()

if (APPLE)
	set_target_properties("example" PROPERTIES XCODE_GENERATE_SCHEME TRUE)
	set_target_properties("example" PROPERTIES XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")
endif()

target_link_libraries("example" PUBLIC "libasura")

################################################
#
# Setup project folders
#
################################################
set_target_properties("imgui" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("SDL2-static" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("SDL2_image" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("SDL2_test" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("SDL2main" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
#set_target_properties("marl" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("box2d" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("ogg" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("vorbis" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("vorbisenc" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("vorbisfile" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("OpenAL" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("common" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
set_target_properties("ex-common" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})

if (UNIX) 
	set_target_properties("fmt" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})
endif()

set_target_properties("mimalloc-static" PROPERTIES ${ASURA_FOLDER_THIRD_PARTY})

if (OPTICK_ENABLED)
	set_target_properties("OptickCore" PROPERTIES ${ASURA_FOLDER_PROFILR})
endif()

set_target_properties("sdl_headers_copy" PROPERTIES ${ASURA_FOLDER_PREGEN})
set_target_properties("uninstall" PROPERTIES ${ASURA_FOLDER_PREGEN})

set_target_properties("libasura" PROPERTIES ${ASURA_FOLDER_ENGINE})
set_target_properties("example" PROPERTIES ${ASURA_FOLDER_GAME})

set_target_properties("example" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")

#set_property(DIRECTORY ${ASURA_FOLDER_GAME} PROPERTY VS_STARTUP_PROJECT "example")