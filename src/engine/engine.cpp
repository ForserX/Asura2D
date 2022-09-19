#include "arkane.h"

using namespace ark;

void
engine::init(int argc, char** argv)
{
	filesystem::init();
	debug::init();

	graphics::init();
	systems::init();
}

void
engine::destroy()
{
	graphics::destroy();
	debug::destroy();
}
