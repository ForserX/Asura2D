/*	
	// Don't forget to add this define in game code
	VISITABLE_STRUCT(struct, members);
*/

namespace valued
{
	struct some_flag
	{
		static constexpr ark::net::flag_t flag = 1 << (ark::entities::last_flag_index + 1);
	};
}

#define DECLARE_GAME_SERIALIZABLE_FLAGS \
	valued::some_flag

#define DECLARE_GAME_SERIALIZABLE_TYPES
#define DECLARE_GAME_NON_SERIALIZABLE_ENTITY_TYPES
