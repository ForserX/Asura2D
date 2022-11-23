#pragma once
#include <box2d/box2d.h>

namespace ark
{
	class CollisionLister;
	
	namespace physics
	{
		class world
		{
		private:
			bool enable_thread = false;

			std::atomic_bool destroy_thread;
			std::unique_ptr<std::thread> physics_thread;
			
			std::unique_ptr<b2World> world_holder;
			b2Body* ground = nullptr;

			std::unique_ptr<CollisionLister> cl;

			stl::hash_set<physics_body*> scheduled_to_delete_bodies;
			stl::hash_set<physics_body*> bodies;

		private:
			void destroy_all_bodies();
			void pre_tick();
			void debug_joints_tick();
			void internal_tick(float dt);
            math::frect get_real_body_rect(b2Body* body);
			
		public:
			world();
			~world();

			void start();
			void init();
			void destroy();
			void tick(float dt);
			
			b2World& get_world() const;
			b2Body* get_ground() const;

			void destroy_world();
		
		public:
			math::frect get_body_rect(const physics_body* body);
			
		public:
			physics_body* schedule_creation(body_parameters parameters);
			void schedule_free(physics_body* body);
		};
	}
}

extern float physics_delta;
extern float physics_real_delta;
extern int target_steps_count;
