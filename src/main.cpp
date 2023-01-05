#include "asura_engine.h"
#include "game/ingame.h"

int main(int argc, char** argv)
{
	ingame::pre_init();
	Asura::Window::Init();
	Asura::engine::Init(argc, argv);
	ingame::init();

	Asura::engine::Start();
	Asura::Window::loop();

	Asura::engine::Destroy();
	return 0;
}
