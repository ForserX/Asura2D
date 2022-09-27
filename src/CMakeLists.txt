
file(GLOB_RECURSE ENGINE_SRC
	"engine/*.cpp"
	"engine/*.h"
)

file(GLOB_RECURSE EDITOR_SRC
	"editor/*.cpp"
	"editor/*.h"
)

file(GLOB_RECURSE GAME_SRC
	"game/*.cpp"
	"game/*.h"
)

set_target_properties("imgui" PROPERTIES ${ARKANE_THIRD_PARTY})
set_target_properties("SDL2" PROPERTIES ${ARKANE_THIRD_PARTY})
set_target_properties("SDL2-static" PROPERTIES ${ARKANE_THIRD_PARTY})
set_target_properties("SDL2_test" PROPERTIES ${ARKANE_THIRD_PARTY})
set_target_properties("SDL2main" PROPERTIES ${ARKANE_THIRD_PARTY})
set_target_properties("marl" PROPERTIES ${ARKANE_THIRD_PARTY})
set_target_properties("box2d" PROPERTIES ${ARKANE_THIRD_PARTY})

set_target_properties("sdl_headers_copy" PROPERTIES ${ARKANE_PREGEN})
set_target_properties("uninstall" PROPERTIES ${ARKANE_PREGEN})
set_target_properties("unit_test" PROPERTIES ${ARKANE_PREGEN})
#set_target_properties("ZERO_CHECK" PROPERTIES ${ARKANE_PREGEN})
#set_target_properties("ALL_BUILD" PROPERTIES ${ARKANE_PREGEN})
#set_target_properties("INSTALL" PROPERTIES ${ARKANE_PREGEN})

add_library("libarkane" STATIC ${ENGINE_SRC} ${EDITOR_SRC} "pch.h")
if (ARKANE_ASAN)
	target_compile_options("libarkane" PUBLIC "-fsanitize=address")
    target_link_libraries("libarkane" PUBLIC "-fsanitize=address")

	target_compile_options("box2d" PUBLIC "-fsanitize=address")
    target_link_libraries("box2d" PUBLIC "-fsanitize=address")

	target_compile_options("imgui" PUBLIC "-fsanitize=address")
    target_link_libraries("imgui" PUBLIC "-fsanitize=address")

	target_compile_options("SDL2" PUBLIC "-fsanitize=address")
    target_link_libraries("SDL2" PUBLIC "-fsanitize=address")

	target_compile_options("SDL2-static" PUBLIC "-fsanitize=address")
    target_link_libraries("SDL2-static" PUBLIC "-fsanitize=address")

	target_compile_options("SDL2-static" PUBLIC "-fsanitize=address")
    target_link_libraries("SDL2main" PUBLIC "-fsanitize=address")


	target_link_libraries("libarkane" PUBLIC "imgui" EnTT::EnTT marl box2d OptickCore "libsancov.lib")
else()
	target_link_libraries("libarkane" PUBLIC "imgui" EnTT::EnTT marl box2d OptickCore)
endif()

target_include_directories("libarkane" PUBLIC "${ARKANE_ROOT}/libs")
target_include_directories("libarkane" PUBLIC "engine")
target_include_directories("libarkane" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_precompile_headers("libarkane" PUBLIC "pch.h")	
set_target_properties("libarkane" PROPERTIES ${ARKANE_ENGINE})

if (ARKANE_BOX2D_OPTIMIZED)
    target_compile_definitions("libarkane" PUBLIC ARKANE_BOX2D_OPTIMIZED)
endif()

project("arkgame")
add_executable("arkgame" ${GAME_SRC} "main.cpp")
target_link_libraries("arkgame" PUBLIC "libarkane")
set_target_properties("arkgame" PROPERTIES ${ARKANE_GAME})
