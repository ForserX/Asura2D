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

void ark::engine::destroy()
{
	debug::destroy();
}
