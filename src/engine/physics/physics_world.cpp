#include "pch.h"

using namespace ark;

constexpr int32 k_maxContactPoints = 2048;
int target_steps_count = 1;
float target_physics_tps = 60.f;
float target_physics_hertz = 60.f;
float physics_delta = 0.f;
float physics_real_delta = 0.f;

b2MouseJoint* TestMouseJoint = nullptr;
physics::physics_body* ContactBody = nullptr;
physics::physics_body* MoveBody = nullptr;
math::fvec2 ContactPoint = {};

std::mutex physics_lock = {};
// Test 

class ark::CollisionLister final : public b2ContactListener
{
	struct ContactPoint
	{
		b2Fixture* fixtureA;
		b2Fixture* fixtureB;
		math::fvec2 normal;
		math::fvec2 position;
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

		if (manifold->pointCount == 0) {
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

physics::world::world()
{
}

physics::world::~world()
{
}

void
physics::world::start()
{
	enable_thread = true;
}

void
physics::world::init()
{
	b2Vec2 gravity(0.0f, -9.8f);
	world_holder = std::make_unique<b2World>(gravity);
	cl = std::make_unique<CollisionLister>();
	world_holder->SetContactListener(cl.get());

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	ground = world_holder->CreateBody(&groundBodyDef);

	if (use_parallel) {
		physics_thread = std::make_unique<std::thread>([this]() {
			OPTICK_THREAD("Physics thread")

			// Setup affinity to second thread
			threads::set_thread_affinity(physics_thread->native_handle(), 1);
			
			while (!enable_thread) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			enable_thread = false;
			std::chrono::nanoseconds begin_physics_time = {};
			std::chrono::nanoseconds end_physics_time = {};
			while (!destroy_thread) {
				OPTICK_EVENT("physics loop")
				if (use_parallel && !paused) {
					auto temp_physics_time = begin_physics_time;
                    
					{
						OPTICK_EVENT("physics tick")
						is_phys_ticking = true;
						internal_tick(1.f / target_physics_hertz);
						is_phys_ticking = false;
					}

					end_physics_time = begin_physics_time + std::chrono::nanoseconds(static_cast<int64_t>((1.f / target_physics_tps) * 1000000000.f));
					{
						OPTICK_EVENT("physics wait")
						while (end_physics_time > begin_physics_time) {
							const float milliseconds_to_end = static_cast<float>((end_physics_time - begin_physics_time).count()) / 1000000.f;
							begin_physics_time = std::chrono::steady_clock::now().time_since_epoch();
							if (milliseconds_to_end > 5.f) {
								std::this_thread::sleep_for(std::chrono::milliseconds(2));
							} else {
								threads::switch_context();
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

void
physics::world::destroy()
{
	destroy_world();
}

void
physics::world::destroy_all_bodies()
{
	// At this stage, we're calling destructor in our proxy bodies
	for (const auto body : bodies) {
		delete body;
	}
	
	bodies.clear();
}

void
physics::world::pre_tick()
{
	std::scoped_lock<std::mutex> scope_lock(physics_lock);
	for (const auto body : bodies) {
		if (!body->is_created()) {
			body->create();
		}
	}

#ifdef ARKANE_BOX2D_OPTIMIZED
	stl::hash_set<b2Contact*> contacts;
	for (const auto body : scheduled_to_delete_bodies) {
		const auto phys_body = body->get_body();
		if (phys_body != nullptr) {
			// In this case, we're trying to write temp pointer to this element to
			// delete after searching this contact in all bodies.
			contacts.reserve(std::max(contacts.size(), static_cast<size_t>(phys_body->GetContactCount())));
			for (int i = 0; i < phys_body->GetContactCount(); i++) {
				const auto contact = phys_body->GetContact(i);
				if (contact != nullptr) {
					contacts.insert(contact);
				}
			}

			// Second - try to delete all referenced links in body
			phys_body->ClearContacts();
		}
	}

	// Yep, this is most interesting part: we're deleting all founded by iterating
	// contacts and free it after processing
	for	(const auto contact : contacts) {
		world_holder->GetContactManager().Destroy(contact);
	}
#endif
	
	for (const auto body : scheduled_to_delete_bodies) {
		body->get_body()->ClearContacts();
		body->destroy();
		if (bodies.contains(body)) {
			bodies.erase(body);
		}

		delete body;
	}

	scheduled_to_delete_bodies.clear();
}

void
physics::world::debug_joints_tick()
{
    if (input::is_focused_on_ui()) {
        return;
    }

    if (input::is_key_pressed(SDL_SCANCODE_MOUSE_X1)) {
        math::fvec2 mouse_position_absolute = ImGui::GetMousePos();
        mouse_position_absolute = camera::screen_to_world(mouse_position_absolute);
        if (ContactBody == nullptr) {
            ContactBody = hit_test(mouse_position_absolute);
            ContactPoint = mouse_position_absolute;
        }
        else {
            const physics_body* test_body = hit_test(mouse_position_absolute);
            if (test_body != nullptr && test_body != ContactBody && test_body->get_body_type() != body_type::static_body) {
                constexpr float frequency_hz = 5.0f;
                constexpr float damping_ratio = 0.7f;

                b2DistanceJointDef jointDef;
                jointDef.Initialize(ContactBody->get_body(), test_body->get_body(), ContactPoint, mouse_position_absolute);

                jointDef.collideConnected = true;
                b2LinearStiffness(jointDef.stiffness, jointDef.damping, frequency_hz, damping_ratio, jointDef.bodyA, jointDef.bodyB);

                physics::get_world().CreateJoint(&jointDef);
                test_body->get_body()->SetAwake(true);
            }

            ContactBody = nullptr;
        }
    }

    if (input::is_key_pressed(SDL_SCANCODE_MOUSE_LEFT)) {
        math::fvec2 mouse_position_absolute = ImGui::GetMousePos();
        mouse_position_absolute = camera::screen_to_world(mouse_position_absolute);
        if (TestMouseJoint == nullptr) {
            MoveBody = hit_test(mouse_position_absolute);
            if (MoveBody != nullptr && MoveBody->get_body_type() != body_type::static_body) {
                constexpr float frequency_hz = 60.0f;
                constexpr float damping_ratio = 1.f;
                b2MouseJointDef jd;
                jd.bodyA = ground;
                jd.bodyB = MoveBody->get_body();
                jd.target = mouse_position_absolute;
                jd.maxForce = 1000.0f * MoveBody->get_body()->GetMass();
                b2LinearStiffness(jd.stiffness, jd.damping, frequency_hz, damping_ratio, jd.bodyA, jd.bodyB);

                TestMouseJoint = dynamic_cast<b2MouseJoint*>(physics::get_world().CreateJoint(&jd));
                MoveBody->get_body()->SetAwake(true);
            } else {
                TestMouseJoint = nullptr;
            }
        } else {
            if (!MoveBody->is_destroyed()) {
                TestMouseJoint->SetTarget(mouse_position_absolute);
            } else {
                TestMouseJoint = nullptr;
            }
        }
    }
    
    if (TestMouseJoint != nullptr && !input::is_key_pressed(SDL_SCANCODE_MOUSE_LEFT)) {
        physics::get_world().DestroyJoint(TestMouseJoint);
        TestMouseJoint = nullptr;
        MoveBody = nullptr;
    }
}

void
physics::world::internal_tick(float dt)
{
	const auto begin_real_time = std::chrono::steady_clock::now().time_since_epoch();
	if (window::is_destroyed()) {
		return;
	}

	{
		OPTICK_EVENT("physics pre tick")
		pre_tick();
	}

	{
		OPTICK_EVENT("physics debug joints tick")
		debug_joints_tick();
	}

	for (int i = 0; i < target_steps_count; i++) {
		OPTICK_EVENT("physics step")
		world_holder->Step(dt, 6, 2);
	}

	{
		OPTICK_EVENT("physics substepping")
		world_holder->ClearForces();
	}

	{
		OPTICK_EVENT("physics systems tick")
		systems::physics_tick(dt);
	}

	const auto end_real_time = std::chrono::steady_clock::now().time_since_epoch();
	physics_real_delta = static_cast<float>((end_real_time - begin_real_time).count()) / 1000000000.f;
}

math::frect
physics::world::get_real_body_position(b2Body* body)
{
	b2AABB aabb = {};
	b2Transform t = {};
	
	t.SetIdentity();
	b2Fixture* fixture = body->GetFixtureList();
	while (fixture != nullptr) {
		const b2Shape* shape = fixture->GetShape();
#ifdef ARKANE_BOX2D_OPTIMIZED
		b2AABB shapeAABB = {};
		shape->ComputeAABB(&shapeAABB, t);
		aabb.Combine(shapeAABB);
#else
		const int childCount = shape->GetChildCount();
		for (int child = 0; child < childCount; ++child) {
			b2AABB shapeAABB = {};
			shape->ComputeAABB(&shapeAABB, t, child);
			aabb.Combine(shapeAABB);
		}
#endif		
		fixture = fixture->GetNext();
	}

	return math::frect(aabb.lowerBound, aabb.upperBound);
}

void
physics::world::tick(float dt)
{
	if (!use_parallel) {
		static float phys_accum = 0.f;
		phys_accum += dt;
		if (phys_accum >= 1.f / target_physics_tps) {
			physics_delta = phys_accum;
			internal_tick(1.f / target_physics_tps);
			phys_accum = 0.f;
		}
	}
}

b2World& 
physics::world::get_world() const
{
	return *world_holder;
}

b2Body*
physics::world::get_ground() const
{
	return ground;
}

void
physics::world::destroy_world()
{
	OPTICK_EVENT("physics destroy world")
	destroy_thread = true;

    physics_thread->join();
	physics_thread.reset();

	destroy_all_bodies();
	world_holder.reset();
}

math::frect
physics::world::get_body_position(const physics_body* body)
{
	if (body != nullptr) {
		return get_real_body_position(body->get_body());
	}

	// return proxy position, until our project hasn't created
	return {};
}

physics::physics_body*
physics::world::schedule_creation(body_parameters parameters)
{
	std::scoped_lock<std::mutex> scope_lock(physics_lock);
	const auto& [key, value] = bodies.insert(new physics_body(parameters));
	return *key;
}

void
physics::world::schedule_free(physics_body* body)
{
    std::scoped_lock<std::mutex> scope_lock(physics_lock);
	scheduled_to_delete_bodies.emplace(body);
}

physics::physics_body::physics_body(body_parameters in_parameters)
	: parameters(in_parameters)
{
}

physics::physics_body::~physics_body()
{

}

physics::body_type
physics::physics_body::get_body_type() const
{
	if (body != nullptr) {
		return get_ark_body_type(body->GetType());
	}
	
	return static_cast<physics::body_type>(parameters.packed_type.type);
}

float
physics::physics_body::get_mass() const
{
	if (body != nullptr) {
		return body->GetMass();
	}

	return parameters.mass;
}

float
physics::physics_body::get_angle() const
{
	if (body != nullptr) {
		return body->GetAngle();
	}

	return parameters.angle;
}

float 
physics::physics_body::get_angular_velocity() const
{
	if (body != nullptr) {
		return body->GetAngularVelocity();
	}

	return parameters.angular_vel;
}

math::fvec2
physics::physics_body::get_position() const
{
	if (body != nullptr) {
		auto temp_pos = body->GetPosition();
        return { temp_pos.x, temp_pos.y };
	}

	return parameters.pos;
}

math::fvec2
physics::physics_body::get_mass_center() const
{
	if (body != nullptr) {
		b2MassData mass_data = {};
		body->GetMassData(&mass_data);
        return { mass_data.center.x, mass_data.center.y};
	}

	return parameters.pos;
}

void 
physics::physics_body::set_body_type(body_type new_type)
{
	if (body != nullptr) {
		body->SetType(get_box2d_body_type(static_cast<uint8_t>(new_type)));
	}

	parameters.packed_type.type = static_cast<uint8_t>(new_type);
}

void
physics::physics_body::set_mass(float new_mass)
{
	if (body != nullptr) {
		b2MassData mass_data = {};
		body->GetMassData(&mass_data);
		mass_data.mass = new_mass;
		body->SetMassData(&mass_data);
	}

	parameters.mass = new_mass;
}

void
physics::physics_body::set_mass_center(math::fvec2& new_center)
{
	if (body != nullptr) {
		b2MassData massData = {};
		body->GetMassData(&massData);
        massData.center = b2Vec2(new_center);
		body->SetMassData(&massData);
	}

	parameters.mass_center = new_center;
}

void
physics::physics_body::set_angle(float new_angle)
{
	if (body != nullptr) {
		body->SetTransform(body->GetPosition(), new_angle);
	}

	parameters.angle = new_angle;
}

void
physics::physics_body::set_angular_velocity(float new_angular_vel)
{
	if (body != nullptr) {
		body->SetAngularVelocity(new_angular_vel);
	}

	parameters.angular_vel = new_angular_vel;
}

void
physics::physics_body::set_position(const math::fvec2& new_pos)
{
	if (body != nullptr) {
        body->SetTransform({new_pos.x(), new_pos.y()}, body->GetAngle());
	}

	parameters.pos = new_pos;
}

physics::body_parameters
physics::physics_body::copy_parameters() const
{
	body_parameters params = parameters;
	if (body != nullptr) {
		params.angle = body->GetAngle();
		params.angular_vel = body->GetAngularVelocity();
		params.vel = body->GetLinearVelocity();
		params.pos = body->GetPosition();
	}

	return params;
}

void
physics::physics_body::create()
{
	const b2BodyDef body_def = parameters;
	const b2MassData mass_data = parameters;
	body = get_world().CreateBody(&body_def);

	b2FixtureDef fixtureDef;
	b2PolygonShape poly_shape = {};
	b2CircleShape circle_shape = {};

	switch (static_cast<material::shape>(parameters.packed_type.shape)) {
	case material::shape::box:
		poly_shape.SetAsBox(parameters.size.x(), parameters.size.y());
		fixtureDef.shape = &poly_shape;
		break;
	case material::shape::circle:
		circle_shape.m_p.Set(parameters.size.x(), parameters.size.y());
		circle_shape.m_radius = parameters.size.x() / 2;
		fixtureDef.shape = &circle_shape;
		break;
    default:
        break;
	}

	const auto& [friction, restitution, density, ignore_collision] = material::get(static_cast<material::type>(parameters.packed_type.mat));
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.isSensor = ignore_collision;
	body->CreateFixture(&fixtureDef);

	if (mass_data.mass != 0.f) {
		body->SetMassData(&mass_data);
	}
	
	created = true;
	destroyed = false;
}

void
physics::physics_body::destroy()
{
	destroyed = true;
	if (body != nullptr) {
		for (auto joint = body->GetJointList(); joint != nullptr; ) {
			const auto next_joint = joint->next;
			get_world().DestroyJoint(joint->joint);
			joint = next_joint;
		}

		get_world().DestroyBody(body);
		body = nullptr;
	}
}