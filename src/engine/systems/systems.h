#pragma once

namespace Asura
{
	class system : entt::process<system, float> 
	{
	public:
		virtual ~system() = default;

	public:
		virtual void Init() = 0;
		virtual void Reset() = 0;
		virtual void Tick(float dt) = 0;

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

			sys->Tick(delta);
			succeed();
		}
	};
}

namespace Asura::Systems
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
	void Init();
	void Destroy();
	
	void pre_tick(float dt);
	void Tick(float dt);
	void post_tick(float dt);
	void draw_tick(float dt);
	void physics_tick(float dt);
}
 