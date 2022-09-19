
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

add_library("libarkane" STATIC ${ENGINE_SRC} ${EDITOR_SRC})
target_link_libraries("libarkane" PUBLIC "imgui")
target_include_directories("libarkane" PRIVATE "src/engine")

add_executable("arkgame" ${GAME_SRC} "main.cpp")
target_link_libraries("arkgame" PUBLIC "libarkane")