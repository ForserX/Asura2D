#pragma once

namespace ark
{
	class registry
	{
	private:
		entt::registry base_registry;

	public:
		entt::registry& get() { return base_registry; }
	};
		
	class system
	{
	public:
		virtual ~system() = default;

	public:
		virtual void init() = 0;
		virtual void tick(registry& reg, float dt) = 0;
	};
}

namespace ark::systems
{
	enum class update_type
	{
		pre_update_schedule,
		update_schedule,
		post_update_schedule
	};
		
	void delete_system(system* system_to_delete, update_type type);
	void add_system(system* system_to_add, update_type type);
		
	void init();
	void destroy();
	void tick(float dt);
}
 