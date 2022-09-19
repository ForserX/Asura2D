#include "arkane.h"
#include "game/game.h"

int main(int argc, char** argv)
{
	game::pre_init();
	
	ark::engine::init(argc, argv);
	game::init();
	
	return 0;
}
