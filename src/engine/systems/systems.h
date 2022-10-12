#pragma once

namespace ark
{
	class system : entt::process<system, float> 
	{
	public:
		virtual ~system() = default;

	public:
		virtual void init() = 0;
		virtual void reset() = 0;
		virtual void tick(float dt) = 0;

	};

	class updater : entt::process<system, float>
	{
		void update(delta_type delta, void* ptr) 
		{
			system* sys = static_cast<system*>(ptr);
			if (sys == nullptr) {
				fail();
				return;
			}

			sys->tick(delta);
			succeed();
		}
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
 