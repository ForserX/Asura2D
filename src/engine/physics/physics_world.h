#pragma once
#include <box2d/box2d.h>

namespace Asura
{
	class CollisionLister;
	
	namespace Physics
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

			stl::hash_set<PhysicsBody*> scheduled_to_delete_bodies;
			stl::hash_set<PhysicsBody*> bodies;
			stl::hash_set<PhysicsJoint*> joints;

		private:
			void destroy_all_bodies();
			void pre_tick();
			void internal_tick(float dt);
            Math::FRect get_real_body_rect(b2Body* body);
			
		public:
			world();
			~world();

			void start();
			void Init();
			void Destroy();
			void Tick(float dt);
			
			b2World& GetWorld() const;
			b2Body* get_ground() const;

			void DestroyWorld();
		
		public:
			Math::FRect get_body_rect(const PhysicsBody* body);
			
		public:
			PhysicsBody* schedule_creation(body_parameters parameters);
			PhysicsJoint* schedule_creation(joint_data&& parameters);
			void schedule_free(PhysicsBody* body);
		};
	}
}

extern float physics_delta;
extern float physics_real_delta;
extern int target_steps_count;
