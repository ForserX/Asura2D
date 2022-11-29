#include "asura_engine.h"
#include "game/ingame.h"

int main(int argc, char** argv)
{
	ingame::pre_init();
	asura::window::init();
	asura::engine::init(argc, argv);
	ingame::init();

	asura::engine::start();
	asura::window::loop();

	asura::engine::destroy();
	return 0;
}
