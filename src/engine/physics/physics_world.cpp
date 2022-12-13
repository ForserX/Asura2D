#include "pch.h"

using namespace Asura;

constexpr int32 k_maxContactPoints = 2048;
int target_steps_count = 1;
float target_physics_tps = 60.f;
float target_physics_hertz = 60.f;
float physics_delta = 0.f;
float physics_real_delta = 0.f;

std::mutex physics_lock = {};

class Asura::CollisionLister final : public b2ContactListener
{
	struct ContactPoint
	{
		b2Fixture* fixtureA;
		b2Fixture* fixtureB;
		Math::FVec2 normal;
		Math::FVec2 position;
		b2PointState state;
		float normalImpulse;
		float tangentImpulse;
		float separation;
	};

	int32 m_pointCount = 0;
	ContactPoint m_points[k_maxContactPoints] = {};

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
	{
		const b2Manifold* manifold = contact->GetManifold();

		if (manifold->pointCount == 0)
		{
			return;
		}

		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
		b2GetPointStates(state1, state2, oldManifold, manifold);

		b2WorldManifold worldManifold = {};
		contact->GetWorldManifold(&worldManifold);

		for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
		{
			ContactPoint* cp = m_points + m_pointCount;
			cp->fixtureA = fixtureA;
			cp->fixtureB = fixtureB;
			cp->position[0] = worldManifold.points[i].x;
            cp->position[1] = worldManifold.points[i].y;
			cp->normal[0] = worldManifold.normal.x;
            cp->normal[1] = worldManifold.normal.y;
			cp->state = state2[i];
			cp->normalImpulse = manifold->points[i].normalImpulse;
			cp->tangentImpulse = manifold->points[i].tangentImpulse;
			cp->separation = worldManifold.separations[i];
			++m_pointCount;
		}
	}
};

Physics::PhysicsWorld::PhysicsWorld()
{
}

Physics::PhysicsWorld::~PhysicsWorld()
{
}

void Physics::PhysicsWorld::Start()
{
	enable_thread = true;
}

void Physics::PhysicsWorld::Init()
{
	b2Vec2 gravity(0.0f, -9.8f);
	world_holder = std::make_unique<b2World>(gravity);
	cl = std::make_unique<CollisionLister>();
	world_holder->SetContactListener(cl.get());

	// Debug Draw: Asura System
	world_dbg_draw = std::make_unique<DebugDraw>();
	world_dbg_draw->AppendFlags(b2Draw::e_jointBit | b2Draw::e_aabbBit);
	world_holder->SetDebugDraw(world_dbg_draw.get());

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	ground = world_holder->CreateBody(&groundBodyDef);

	if (use_parallel)
	{
		physics_thread = std::make_unique<std::thread>([this]()
		{
			OPTICK_THREAD("Physics thread")

			// Setup affinity to second thread
			Threads::SetAffinity((void*)physics_thread->native_handle(), 1);
			
			while (!enable_thread) 
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			enable_thread = false;
			std::chrono::nanoseconds begin_physics_time = {};
			std::chrono::nanoseconds end_physics_time = {};

			while (!destroy_thread) 
			{
				OPTICK_EVENT("Physics loop");
				if (use_parallel && !paused) 
				{
					auto temp_physics_time = begin_physics_time;
                    
					{
						OPTICK_EVENT("Physics Destroy");
						is_phys_ticking = true;
						InternalTick(1.f / target_physics_hertz);
						is_phys_ticking = false;
					}

					end_physics_time = begin_physics_time + std::chrono::nanoseconds(static_cast<int64_t>((1.f / target_physics_tps) * 1000000000.f));
					{
						OPTICK_EVENT("Physics wait");
						while (end_physics_time > begin_physics_time) 
						{
							const float milliseconds_to_end = static_cast<float>((end_physics_time - begin_physics_time).count()) / 1000000.f;
							begin_physics_time = std::chrono::steady_clock::now().time_since_epoch();
							if (milliseconds_to_end > 5.f) 
							{
								Threads::Wait();
							} 
							else 
							{
								Threads::SwitchContext();
							}
						}
					}
					end_physics_time = begin_physics_time;
					physics_delta = static_cast<float>((end_physics_time - temp_physics_time).count()) / 1000000000.f;
				}
			}

            destroy_thread = false;
		});
	}
}

void Physics::PhysicsWorld::Destroy()
{
	DestroyWorld();
}

void Physics::PhysicsWorld::DestroyAllBodies()
{
	// At this stage, we're calling destructor in our proxy bodies
	for (const auto body : bodies)
	{
		delete body;
	}
	
	bodies.clear();
}

void Physics::PhysicsWorld::PreTick()
{
	std::scoped_lock<std::mutex> scope_lock(physics_lock);
	for (const auto body : bodies) 
	{
		if (!body->IsCreated()) 
		{
			body->Create();
		}
	}

	for (const auto joint : joints)
	{
		if (!joint->IsCreated())
		{
			joint->Create();
		}
	}

#ifdef ASURA_BOX2D_OPTIMIZED
	stl::hash_set<b2Contact*> contacts;
	for (const auto body : scheduled_to_delete_bodies)
	{
		const auto phys_body = body->get_body();
		if (phys_body != nullptr) 
		{
			// In this case, we're trying to write temp pointer to this element to
			// delete after searching this contact in all bodies.
			contacts.reserve(std::max(contacts.size(), static_cast<size_t>(phys_body->GetContactCount())));
			for (int i = 0; i < phys_body->GetContactCount(); i++)
			{
				const auto contact = phys_body->GetContact(i);
				if (contact != nullptr) 
				{
					contacts.insert(contact);
				}
			}

			// Second - try to delete all referenced links in body
			phys_body->ClearContacts();
		}
	}

	// Yep, this is most interesting part: we're deleting all founded by iterating
	// contacts and free it after processing
	for	(const auto contact : contacts)
	{
		world_holder->GetContactManager().Destroy(contact);
	}
#endif
	
	for (const auto body : scheduled_to_delete_bodies) 
	{
		body->get_body()->ClearContacts();
		body->Destroy();

		if (bodies.contains(body))
		{
			bodies.erase(body);
		}

		delete body;
	}

	scheduled_to_delete_bodies.clear();
}

void Physics::PhysicsWorld::InternalTick(float dt)
{
	const auto begin_real_time = std::chrono::steady_clock::now().time_since_epoch();
	if (window::IsDestroyed()) 
	{
		return;
	}

	{
		OPTICK_EVENT("Physics pre Destroy");
		PreTick();
	}

	if (is_editor_mode)
	{
		return;
	}

	for (int i = 0; i < target_steps_count; i++) 
	{
		OPTICK_EVENT("Physics step");
		world_holder->Step(dt, 6, 2);
	}

	{
		OPTICK_EVENT("Physics substepping");
		world_holder->ClearForces();
	}

	{
		OPTICK_EVENT("Physics Systems Destroy");
		Systems::physics_tick(dt);
	}

	{
		OPTICK_EVENT("Physics Debug joints Destroy");

		if (holder_type == GamePlay::holder_mode::free)
		{
			GamePlay::Holder::free::Tick();
		}
	}

	const auto end_real_time = std::chrono::steady_clock::now().time_since_epoch();
	physics_real_delta = static_cast<float>((end_real_time - begin_real_time).count()) / 1000000000.f;
}

Math::FRect Physics::PhysicsWorld::GetRealBodyRect(b2Body* body)
{
	if (body == nullptr)
	{
		return {};
	}

	b2AABB aabb = {};
	b2Transform t = {};
	
	t.SetIdentity();
	b2Fixture* fixture = body->GetFixtureList();
	while (fixture != nullptr) 
	{
		const b2Shape* shape = fixture->GetShape();
#ifdef ASURA_BOX2D_OPTIMIZED
		b2AABB shapeAABB = {};
		shape->ComputeAABB(&shapeAABB, t);
		aabb.Combine(shapeAABB);
#else
		const int childCount = shape->GetChildCount();
		for (int child = 0; child < childCount; ++child) 
		{
			b2AABB shapeAABB = {};
			shape->ComputeAABB(&shapeAABB, t, child);
			aabb.Combine(shapeAABB);
		}
#endif		
		fixture = fixture->GetNext();
	}

	return Math::FRect(aabb.lowerBound, aabb.upperBound);
}

void Physics::PhysicsWorld::Tick(float dt)
{
	if (!use_parallel && !is_editor_mode)
	{
		static float phys_accum = 0.f;
		phys_accum += dt;

		if (phys_accum >= 1.f / target_physics_tps)
		{
			physics_delta = phys_accum;
			InternalTick(1.f / target_physics_tps);
			phys_accum = 0.f;
		}
	}
}

b2World& Physics::PhysicsWorld::GetWorld() const
{
	return *world_holder;
}

b2Body* Physics::PhysicsWorld::GetGround() const
{
	return ground;
}

void Physics::PhysicsWorld::DestroyWorld()
{
	OPTICK_EVENT("Physics Destroy world")
	destroy_thread = true;

    physics_thread->join();
	physics_thread.reset();

	DestroyAllBodies();
	world_holder.reset();
}

Math::FRect Physics::PhysicsWorld::GetBodyRect(const PhysicsBody* body)
{
	if (body != nullptr) {
		return GetRealBodyRect(body->get_body());
	}

	// return proxy position, until our object doesn't created
	return {};
}

Physics::PhysicsBody* Physics::PhysicsWorld::SafeCreation(body_parameters parameters)
{
	std::scoped_lock<std::mutex> scope_lock(physics_lock);
	const auto& [key, value] = bodies.insert(new PhysicsBody(parameters));
	return *key;
}

Physics::PhysicsJoint* Asura::Physics::PhysicsWorld::SafeCreation(joint_data&& parameters)
{
	std::scoped_lock<std::mutex> scope_lock(physics_lock);
	auto value = joints.emplace(new PhysicsJoint(std::move(parameters)));
	return *(value.first);
}

void Physics::PhysicsWorld::SafeFree(PhysicsBody* body)
{
    std::scoped_lock<std::mutex> scope_lock(physics_lock);
	scheduled_to_delete_bodies.emplace(body);
}
