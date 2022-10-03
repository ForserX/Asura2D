#include "arkane.h"
#include "game/ingame.h"

int main(int argc, char** argv)
{
	ingame::pre_init();
	ark::window::init();
	ark::engine::init(argc, argv);
	ingame::init();

	ark::engine::start();
	ark::window::loop();

	ark::engine::destroy();
	return 0;
}
