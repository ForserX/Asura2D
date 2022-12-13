#pragma once
#include <box2d/box2d.h>

namespace Asura
{
	class CollisionLister;
	
	namespace Physics
	{
		class PhysicsWorld
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
			void DestroyAllBodies();
			void PreTick();
			void InternalTick(float dt);
            Math::FRect GetRealBodyRect(b2Body* body);
			
		public:
			PhysicsWorld();
			~PhysicsWorld();

			void Start();
			void Init();
			void Destroy();
			void Tick(float dt);
			
			b2World& GetWorld() const;
			b2Body* GetGround() const;

			void DestroyWorld();
		
		public:
			Math::FRect GetBodyRect(const PhysicsBody* body);
			
		public:
			PhysicsBody* SafeCreation(body_parameters parameters);
			PhysicsJoint* SafeCreation(joint_data&& parameters);
			void SafeFree(PhysicsBody* body);
		};
	}
}

extern float physics_delta;
extern float physics_real_delta;
extern int target_steps_count;
