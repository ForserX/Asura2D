#include "pch.h"

using namespace Asura;

Physics::PhysicsBody::PhysicsBody(body_parameters in_parameters)
	: parameters(in_parameters)
{
}

Physics::PhysicsBody::~PhysicsBody()
{

}

Physics::body_type Physics::PhysicsBody::get_body_type() const
{
	if (body != nullptr)
	{
		return get_ark_body_type(body->GetType());
	}

	return static_cast<Physics::body_type>(parameters.packed_type.type);
}

float Physics::PhysicsBody::get_mass() const
{
	if (body != nullptr)
	{
		return body->GetMass();
	}

	return parameters.mass;
}

float Physics::PhysicsBody::get_angle() const
{
	if (body != nullptr)
	{
		return body->GetAngle();
	}

	return parameters.angle;
}

float Physics::PhysicsBody::get_angular_velocity() const
{
	if (body != nullptr)
	{
		return body->GetAngularVelocity();
	}

	return parameters.angular_vel;
}

Math::FVec2 Asura::Physics::PhysicsBody::get_velocity() const
{
	if (body != nullptr)
	{
		return body->GetLinearVelocity();
	}

	return parameters.vel;
}

Math::FVec2 Physics::PhysicsBody::get_position() const
{
	if (body != nullptr) {
		auto temp_pos = body->GetPosition();
		return { temp_pos.x, temp_pos.y };
	}

	return parameters.pos;
}

Math::FRect Physics::PhysicsBody::get_rect() const
{
	// #TODO: OPTIMIZE
	return Physics::GetBodyRect(this);
}

Math::FVec2 Physics::PhysicsBody::get_mass_center() const
{
	if (body != nullptr)
	{
		b2MassData mass_data = {};
		body->GetMassData(&mass_data);
		return { mass_data.center.x, mass_data.center.y };
	}

	return parameters.pos;
}

void Physics::PhysicsBody::set_body_type(body_type new_type)
{
	if (body != nullptr)
	{
		body->SetType(get_box2d_body_type(static_cast<uint8_t>(new_type)));
	}

	parameters.packed_type.type = static_cast<uint8_t>(new_type);
}

void Physics::PhysicsBody::set_mass(float new_mass)
{
	if (body != nullptr)
	{
		b2MassData mass_data = {};
		body->GetMassData(&mass_data);
		mass_data.mass = new_mass;
		body->SetMassData(&mass_data);
	}

	parameters.mass = new_mass;
}

void Physics::PhysicsBody::set_mass_center(const Math::FVec2& new_center)
{
	if (body != nullptr)
	{
		b2MassData massData = {};
		body->GetMassData(&massData);
		massData.center = b2Vec2(new_center);
		body->SetMassData(&massData);
	}

	parameters.mass_center = new_center;
}

void Physics::PhysicsBody::set_angle(float new_angle)
{
	if (body != nullptr)
	{
		body->SetTransform(body->GetPosition(), new_angle);
	}

	parameters.angle = new_angle;
}

void Physics::PhysicsBody::set_angular_velocity(float new_angular_vel)
{
	if (body != nullptr)
	{
		body->SetAngularVelocity(new_angular_vel);
	}

	parameters.angular_vel = new_angular_vel;
}

void Physics::PhysicsBody::set_velocity(const Math::FVec2& new_vel)
{
	if (body != nullptr)
	{
		body->SetLinearVelocity(new_vel);
	}

	parameters.vel = new_vel;
}

void Physics::PhysicsBody::set_position(const Math::FVec2& new_pos)
{
	if (body != nullptr) 
	{
		body->SetTransform({ new_pos.x, new_pos.y }, body->GetAngle());
	}

	parameters.pos = new_pos;
}

void Physics::PhysicsBody::apply_impulse(const Math::FVec2& impulse)
{
	if (body != nullptr) 
	{
		body->ApplyLinearImpulseToCenter(impulse);
	}
}

void Physics::PhysicsBody::apply_angular_impulse(float impulse)
{
	if (body != nullptr) 
	{
		body->ApplyAngularImpulse(impulse);
	}
}

Physics::body_parameters Physics::PhysicsBody::copy_parameters() const
{
	body_parameters params = parameters;
	if (body != nullptr) 
	{
		params.angle = body->GetAngle();
		params.angular_vel = body->GetAngularVelocity();
		params.vel = body->GetLinearVelocity();
		params.pos = body->GetPosition();
	}

	return params;
}

void Physics::PhysicsBody::Create()
{
	const b2BodyDef body_def = parameters;
	const b2MassData mass_data = parameters;
	body = GetWorld().CreateBody(&body_def);

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

	if (mass_data.mass != 0.f) 
	{
		body->SetMassData(&mass_data);
	}

	created = true;
	destroyed = false;
}

void Physics::PhysicsBody::Destroy()
{
	destroyed = true;

	if (body != nullptr)
	{
		for (auto joint = body->GetJointList(); joint != nullptr; )
		{
			const auto next_joint = joint->next;
			GetWorld().DestroyJoint(joint->joint);
			joint = next_joint;
		}

		GetWorld().DestroyBody(body);
		body = nullptr;
	}
}
