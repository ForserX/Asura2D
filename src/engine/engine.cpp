#include "arkane.h"

using namespace ark;

void
engine::init(int argc, char** argv)
{
	graphics::init();

	systems::init();
}
