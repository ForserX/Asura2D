#include "arkane.h"
#include "game/game.h"

int main(int argc, char** argv)
{
	game::pre_init();
	
	ark::engine::init(argc, argv);
	game::init();

	ark::debug::msg("test");
	ark::debug::msg("test {}", 321);

	ark::engine::destroy();
	return 0;
}
