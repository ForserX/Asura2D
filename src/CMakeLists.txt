
file(GLOB_RECURSE ENGINE_SRC
	"engine/*.cpp"
	"engine/*.h"
)

file(GLOB_RECURSE EDITOR_SRC
	"editor/*.cpp"
	"editor/*.h"
)

add_library("libarkane" STATIC ${ENGINE_SRC} ${EDITOR_SRC})
