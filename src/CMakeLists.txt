################################################
#
# Define engine source files
#
################################################
file(GLOB ARK_ENGINE_CORE_SRC
	"engine/core/*.cpp"
	"engine/core/*.h"
)

file(GLOB ARK_ENGINE_ECES_SRC
	"engine/eces/*.cpp"
	"engine/eces/*.h"
)

file(GLOB ARK_ENGINE_ENGINE_SRC
	"engine/engine/*.cpp"
	"engine/engine/*.h"
)

file(GLOB ARK_ENGINE_NET_SRC
	"engine/net/*.cpp"
	"engine/net/*.h"
)

file(GLOB ARK_ENGINE_PHYSICS_SRC
	"engine/physics/*.cpp"
	"engine/physics/*.h"
)

file(GLOB ARK_ENGINE_RENDER_SRC
	"engine/render/*.cpp"
	"engine/render/*.h"
)

file(GLOB ARK_ENGINE_SYSTEMS_SRC
	"engine/systems/*.cpp"
	"engine/systems/*.h"
)

file(GLOB_RECURSE ARK_EDITOR_SRC
	"editor/*.cpp"
	"editor/*.h"
)

file(GLOB_RECURSE ARK_GAME_SRC
	"game/*.cpp"
	"game/*.h"
)

################################################
#
# Define engine source files directories
#
################################################
source_group("engine" FILES ${ARK_ENGINE_ENGINE_SRC})
source_group("engine\\core" FILES ${ARK_ENGINE_CORE_SRC})
source_group("engine\\eces" FILES ${ARK_ENGINE_ECES_SRC})
source_group("engine\\network" FILES ${ARK_ENGINE_NET_SRC})
source_group("engine\\physics" FILES ${ARK_ENGINE_PHYSICS_SRC})
source_group("engine\\render" FILES ${ARK_ENGINE_RENDER_SRC})
source_group("engine\\systems" FILES ${ARK_ENGINE_SYSTEMS_SRC})

set(ENGINE_SRC_FILES)
list(APPEND ENGINE_SRC_FILES ${ARK_ENGINE_ENGINE_SRC})
list(APPEND ENGINE_SRC_FILES ${ARK_ENGINE_CORE_SRC})
list(APPEND ENGINE_SRC_FILES ${ARK_ENGINE_ECES_SRC})
list(APPEND ENGINE_SRC_FILES ${ARK_ENGINE_NET_SRC})
list(APPEND ENGINE_SRC_FILES ${ARK_ENGINE_PHYSICS_SRC})
list(APPEND ENGINE_SRC_FILES ${ARK_ENGINE_RENDER_SRC})
list(APPEND ENGINE_SRC_FILES ${ARK_ENGINE_SYSTEMS_SRC})
list(APPEND ENGINE_SRC_FILES ${EDITOR_SRC})
list(APPEND ENGINE_SRC_FILES "pch.h")
list(APPEND ENGINE_SRC_FILES "arkane.h")
list(APPEND ENGINE_SRC_FILES "arkane_engine.h")

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
list(APPEND LIBRARIES_TO_LINK "OptickCore")
list(APPEND LIBRARIES_TO_LINK "SDL2-static")
list(APPEND LIBRARIES_TO_LINK "SDL2main")
list(APPEND LIBRARIES_TO_LINK "SDL2_image")
if (MIMALLOC_ENABLED)
	list(APPEND LIBRARIES_TO_LINK "mimalloc-static")
endif()

################################################
#
# Define engine and link libraries to it
#
################################################
add_library("libarkane" STATIC ${ENGINE_SRC_FILES})
target_link_libraries("libarkane" PUBLIC ${LIBRARIES_TO_LINK})

if (ARKANE_ASAN)
	target_compile_options("libarkane" PUBLIC "-fsanitize=address")
    target_link_libraries("libarkane" PUBLIC "-fsanitize=address")
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
target_include_directories("libarkane" PUBLIC "${ARKANE_ROOT}/libs")
target_include_directories("libarkane" PUBLIC "engine")
target_include_directories("libarkane" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Enable pch.h for engine
target_precompile_headers("libarkane" PUBLIC "pch.h")	

# Enable optimized version of box2d
if (ARKANE_BOX2D_OPTIMIZED)
    target_compile_definitions("libarkane" PUBLIC ARKANE_BOX2D_OPTIMIZED)
endif()

################################################
#
# Define game executable
#
################################################
if (WIN32)
	add_executable("arkgame" WIN32 ${ARK_GAME_SRC} "main.cpp")
else()
	add_executable("arkgame" ${ARK_GAME_SRC} "main.cpp")
endif()

if (APPLE)
	set_target_properties("arkgame" PROPERTIES XCODE_GENERATE_SCHEME TRUE)
	set_target_properties("arkgame" PROPERTIES XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")
endif()

target_link_libraries("arkgame" PUBLIC "libarkane")

################################################
#
# Setup project folders
#
################################################
set_target_properties("imgui" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
set_target_properties("SDL2-static" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
set_target_properties("SDL2_image" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
set_target_properties("SDL2_test" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
set_target_properties("SDL2main" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
#set_target_properties("marl" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
set_target_properties("box2d" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
set_target_properties("mimalloc-static" PROPERTIES ${ARKANE_FOLDER_THIRD_PARTY})
set_target_properties("OptickCore" PROPERTIES ${ARKANE_FOLDER_PROFILR})

set_target_properties("sdl_headers_copy" PROPERTIES ${ARKANE_FOLDER_PREGEN})
set_target_properties("uninstall" PROPERTIES ${ARKANE_FOLDER_PREGEN})

set_target_properties("libarkane" PROPERTIES ${ARKANE_FOLDER_ENGINE})
set_target_properties("arkgame" PROPERTIES ${ARKANE_FOLDER_GAME})

set_target_properties("arkgame" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")

#set_property(DIRECTORY ${ARKANE_FOLDER_GAME} PROPERTY VS_STARTUP_PROJECT "arkgame")