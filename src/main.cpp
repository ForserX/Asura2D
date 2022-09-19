#include "arkane.h"
#include "game/game.h"

#undef main
int main(int argc, char** argv)
{
	game::pre_init();
	ark::window::init();
	ark::engine::init(argc, argv);
	game::init();

	ark::window::loop();

	ark::engine::destroy();
	return 0;
}
