#include "asura_engine.h"
#include "game/ingame.h"

#ifdef OS_WINDOWS
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int) //Windows signature since creating WIN32 application without console
#else
int main()
#endif
{
	ingame::pre_init();
	Asura::Window::Init();
	Asura::engine::Init();
	ingame::init();

	Asura::engine::Start();
	Asura::Window::loop();

	Asura::engine::Destroy();
	return 0;
}
