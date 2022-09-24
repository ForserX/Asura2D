#include "pch.h"

using namespace ark;

constexpr int32 k_maxContactPoints = 2048;
float target_physics_tps = 60.f;
float target_physics_hertz = 60.f;
float physics_delta = 0.f;

b2MouseJoint* TestMouseJoint = nullptr;
physics::physics_body* ContactBody = nullptr;
physics::physics_body* MoveBody = nullptr;
ark_float_vec2 ContactPoint = {};
// Test 

auto physical_mouse_key_change = [](int16_t scan_code, input::key_state state) {

	if (scan_code == SDL_SCANCODE_MOUSE_X1 && state == input::key_state::press) {
		ark_float_vec2 mousePositionAbsolute = ImGui::GetMousePos();
		mousePositionAbsolute = camera::screen_to_world(mousePositionAbsolute);

		if (ContactBody == nullptr) {
			ContactBody = physics::hit_test(mousePositionAbsolute);
			ContactPoint = mousePositionAbsolute;
		}
		else {
			physics::physics_body* TestBody = physics::hit_test(mousePositionAbsolute);

			if (TestBody != nullptr && TestBody != ContactBody)
			{
				constexpr float frequency_hz = 5.0f;
				constexpr float damping_ratio = 0.7f;

				b2DistanceJointDef jointDef;
				jointDef.Initialize(ContactBody->get_body(), TestBody->get_body(), ContactPoint, mousePositionAbsolute);

				jointDef.collideConnected = true;
				b2LinearStiffness(jointDef.stiffness, jointDef.damping, frequency_hz, damping_ratio, jointDef.bodyA, jointDef.bodyB);

				physics::get_world().CreateJoint(&jointDef);
				TestBody->get_body()->SetAwake(true);

				ContactBody = nullptr;
			}
		}
	}

	if (scan_code == SDL_SCANCODE_MOUSE_LEFT)
	{
		if (state == input::key_state::press) {
			ark_float_vec2 mousePositionAbsolute = ImGui::GetMousePos();
			mousePositionAbsolute = camera::screen_to_world(mousePositionAbsolute);

			if (TestMouseJoint == nullptr) {
				MoveBody = physics::hit_test(mousePositionAbsolute);

				if (MoveBody != nullptr) {
					constexpr float frequency_hz = 5.0f;
					constexpr float damping_ratio = 0.7f;

					b2MouseJointDef jd;
					jd.bodyA = physics::get_ground();
					jd.bodyB = MoveBody->get_body();
					jd.target = mousePositionAbsolute;
					jd.maxForce = 1000.0f * MoveBody->get_body()->GetMass();
					b2LinearStiffness(jd.stiffness, jd.damping, frequency_hz, damping_ratio, jd.bodyA, jd.bodyB);

					TestMouseJoint = dynamic_cast<b2MouseJoint*>(physics::get_world().CreateJoint(&jd));
					MoveBody->get_body()->SetAwake(true);
				}
				else {
					TestMouseJoint = nullptr;
				}
			}
			else {
				if (!MoveBody->is_destroyed()) {
					TestMouseJoint->SetTarget(mousePositionAbsolute);
				}
				else {
					TestMouseJoint = nullptr;
				}
			}
		}

		if (TestMouseJoint != nullptr && state == input::key_state::release) {
			physics::get_world().DestroyJoint(TestMouseJoint);
			TestMouseJoint = nullptr;
			MoveBody = nullptr;
		}
	}
};

class ark::CollisionLister final : public b2ContactListener
{
	struct ContactPoint
	{
		b2Fixture* fixtureA;
		b2Fixture* fixtureB;
		ark_float_vec2 normal;
		ark_float_vec2 position;
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
			cp->position = worldManifold.points[i];
			cp->normal = worldManifold.normal;
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
	ark_float_vec2 gravity(0.0f, -9.8f);
	world_holder = std::make_unique<b2World>(gravity);
	cl = std::make_unique<CollisionLister>();
	world_holder->SetContactListener(cl.get());

	world_dbg_draw = std::make_unique<DebugDraw>();
	world_dbg_draw->AppendFlags(b2Draw::e_jointBit | b2Draw::e_aabbBit);

	world_holder->SetDebugDraw(world_dbg_draw.get());

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	ground = world_holder->CreateBody(&groundBodyDef);

	if (use_parallel) {
		physics_thread = std::make_unique<std::jthread>([this]() {
			OPTICK_THREAD("Physics thread")
			OPTICK_FRAME_EVENT(Optick::FrameType::CPU)
			OPTICK_CATEGORY("physics::tick", Optick::Category::Physics)

			// Setup affinity to second thread
			threads::set_thread_affinity(physics_thread->native_handle(), 1);
			
			while (!enable_thread) {
				std::this_thread::sleep_for(std::chrono::seconds(0));
			}

			enable_thread = false;
			std::chrono::nanoseconds begin_physics_time = {};
			std::chrono::nanoseconds end_physics_time = {};
			while (!destroy_thread) {
				OPTICK_FRAME("Physics")
				OPTICK_EVENT("physics loop")
				if (use_parallel) {
					auto temp_physics_time = begin_physics_time;
					physics_event.clear();
					{
						OPTICK_EVENT("physics tick")
						internal_tick(1.f / target_physics_hertz);
					}
					physics_event.signal();

					end_physics_time = begin_physics_time + std::chrono::nanoseconds(static_cast<int64_t>((1.f / target_physics_tps) * 1000000000.f));

					{
						OPTICK_EVENT("physics wait")
						while (end_physics_time > begin_physics_time) {
							const float milliseconds_to_end = static_cast<float>((end_physics_time - begin_physics_time).count()) / 1000000.f;
							int64_t sleep_time = milliseconds_to_end > 5.f ? 2 : 0;
							begin_physics_time = std::chrono::steady_clock::now().time_since_epoch();
							std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
						}
					}

					end_physics_time = begin_physics_time;
					physics_delta = static_cast<float>((end_physics_time - temp_physics_time).count()) / 1000000000.f;
				}
			}

			thread_destroyed_event.signal();
		});
	}

	input::subscribe_key_event(physical_mouse_key_change);
}

void
physics::world::destroy()
{
	input::unsubscribe_key_event(physical_mouse_key_change);
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
	for (const auto body : bodies) {
		if (!body->is_created()) {
			body->create();
		}
	}
	
	for (const auto body : scheduled_to_delete_bodies) {
		body->destroy();
		if (bodies.contains(body)) {
			bodies.erase(body);
		}
	}

	scheduled_to_delete_bodies.clear();
}

void
physics::world::joints_tick()
{
}

void
physics::world::internal_tick(float dt)
{
	if (window::is_destroyed()) {
		return;
	}

	{
		OPTICK_EVENT("physics pre tick")
		pre_tick();
	}

	{
		OPTICK_EVENT("physics joints tick")
		joints_tick();
	}

	{
		OPTICK_EVENT("physics step")
		world_holder->Step(dt, 6, 2);
	}

	{
		OPTICK_EVENT("physics systems tick")
		systems::physics_tick(dt);
	}
}

ark_matrix
physics::world::get_real_body_position(b2Body* body)
{
	ark_matrix pos = {};
	b2AABB aabb = {};
	b2Transform t = {};
	
	t.SetIdentity();
	b2Fixture* fixture = body->GetFixtureList();
	while (fixture != nullptr) {
		const b2Shape* shape = fixture->GetShape();
		const int childCount = shape->GetChildCount();
		for (int child = 0; child < childCount; ++child) {
			b2AABB shapeAABB = {};
			shape->ComputeAABB(&shapeAABB, t, child);
			aabb.Combine(shapeAABB);
		}
		
		fixture = fixture->GetNext();
	}

	pos.x = body->GetPosition().x;
	pos.y = body->GetPosition().y;
	pos.w = aabb.upperBound.x;
	pos.h = aabb.upperBound.y;

	return pos;
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

	{
		OPTICK_EVENT("physics substepping")
		world_holder->ClearForces();
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
	thread_destroyed_event.wait();
	thread_destroyed_event.clear();
	destroy_thread = false;

	physics_thread.reset();

	destroy_all_bodies();
	world_holder.reset();
}

ark_matrix
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
	const auto& [key, value] = bodies.insert(new physics_body(parameters));
	return *key;
}

void
physics::world::schedule_free(physics_body* body)
{
	scheduled_to_delete_bodies.emplace(body);
}

void
physics::physics_body::create_around()
{
	OPTICK_EVENT("physics create around body")
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(parameters.pos.x, parameters.pos.y);
	body = get_world().CreateBody(&bodyDef);

	b2CircleShape circle;
	circle.m_p.Set(parameters.size.x, parameters.size.y);
	circle.m_radius = parameters.size.x / 2;

	b2FixtureDef fixtureDef;
	const auto& [friction, restitution, density, ignore_collision] = get(parameters.mat);
	fixtureDef.shape = &circle;
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.isSensor = ignore_collision;
	body->CreateFixture(&fixtureDef);
}

void
physics::physics_body::create_static()
{
	OPTICK_EVENT("physics create static body")
	
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(parameters.pos.x, parameters.pos.y);

	body = get_world().CreateBody(&groundBodyDef);

	const auto& [friction, restitution, density, ignore_collision] = get(parameters.mat);
	b2PolygonShape groundBox;
	groundBox.SetAsBox(parameters.size.x, parameters.size.y);
	body->CreateFixture(&groundBox, density);
}

void
physics::physics_body::create_dynamic()
{
	OPTICK_EVENT("physics create dynamic body")
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(parameters.pos.x, parameters.pos.y);
	body = get_world().CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(parameters.size.x, parameters.size.y);

	b2FixtureDef fixtureDef;
	b2MassData mass_data = {};
	mass_data.center = { parameters.size.x / 2, parameters.size.y / 2 };
	mass_data.mass = 5;
	
	const auto& [friction, restitution, density, ignore_collision] = get(parameters.mat);
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = density;
	fixtureDef.friction = friction;
	fixtureDef.restitution = restitution;
	fixtureDef.isSensor = ignore_collision;
	body->CreateFixture(&fixtureDef);
	//body->SetMassData(&mass_data);
}

physics::physics_body::physics_body(body_parameters in_parameters)
	: parameters(in_parameters)
{
}

physics::physics_body::~physics_body()
{

}

const ark_float_vec2& 
physics::physics_body::get_position()
{
	if (body != nullptr) {
		proxy_position = body->GetPosition();
	}

	return proxy_position;
}

void
physics::physics_body::create()
{
	switch (parameters.type) {
	case body_type::around_body:
		create_around();
		break;
	case body_type::dynamic_body:
		create_dynamic();
		break;
	case body_type::static_body:
		create_static();
		break;
	}

	created = true;
	destroyed = false;
}

void
physics::physics_body::destroy()
{
	destroyed = true;
	for (auto joint = body->GetJointList(); joint != nullptr; ) {
		const auto next_joint = joint->next;
		get_world().DestroyJoint(joint->joint);
		joint = next_joint;
	}
	
	get_world().DestroyBody(body);
	body = nullptr;
}
