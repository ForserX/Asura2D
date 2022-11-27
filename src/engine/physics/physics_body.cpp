#include "pch.h"

using namespace ark;

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

math::fvec2 ark::physics::physics_body::get_velocity() const
{
	if (body != nullptr) {
		return body->GetLinearVelocity();
	}

	return parameters.vel;
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

math::frect 
physics::physics_body::get_rect() const
{
	// #TODO: OPTIMIZE
	return physics::get_body_rect(this);
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
physics::physics_body::set_mass_center(const math::fvec2& new_center)
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
physics::physics_body::set_velocity(const math::fvec2& new_vel)
{
	if (body != nullptr) {
		body->SetLinearVelocity(new_vel);
	}

	parameters.vel = new_vel;
}

void
physics::physics_body::set_position(const math::fvec2& new_pos)
{
	if (body != nullptr) {
        body->SetTransform({new_pos.x, new_pos.y}, body->GetAngle());
	}

	parameters.pos = new_pos;
}

void 
physics::physics_body::apply_impulse(const math::fvec2& impulse)
{
	if (body != nullptr) {
		body->ApplyLinearImpulseToCenter(impulse);
	}
}

void 
physics::physics_body::apply_angular_impulse(float impulse)
{
	if (body != nullptr) {
		body->ApplyAngularImpulse(impulse);
	}
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

	switch (static_cast<material::shape>(parameters.packed_type.shape)) 
	{
		case material::shape::box:
			poly_shape.SetAsBox(parameters.size.x, parameters.size.y);
			fixtureDef.shape = &poly_shape;
			break;
		case material::shape::circle:
			circle_shape.m_p.Set(parameters.size.x, parameters.size.y);
			circle_shape.m_radius = parameters.size.x / 2;
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
