#pragma once

namespace ark
{
	class system
	{
	public:
		virtual ~system() = default;

	public:
		virtual void init() = 0;
		virtual void reset() = 0;
		virtual void tick(registry& reg, float dt) = 0;
	};
}

namespace ark::systems
{
	enum class update_type
	{
		pre_update_schedule,
		update_schedule,
		post_update_schedule,
		draw_schedule,
		physics_schedule
	};
	
	void delete_system(system* system_to_delete, update_type type);
	void add_system(system* system_to_add, update_type type);
		
	void pre_init();
	void init();
	void destroy();
	
	void pre_tick(float dt);
	void tick(float dt);
	void post_tick(float dt);
	void draw_tick(float dt);
	void physics_tick(float dt);
}
 