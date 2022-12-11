#include "pch.h"

using namespace Asura;

Asura::Physics::PhysicsJoint::~PhysicsJoint()
{
	Destroy();
}

void Asura::Physics::PhysicsJoint::Create()
{
	switch (data.type)
	{
		case Asura::Physics::joint_type::base:
			break;
		case Asura::Physics::joint_type::mouse:
			break;
		case Asura::Physics::joint_type::motor:
			break;
		case Asura::Physics::joint_type::revolute:
		{
			b2RevoluteJointDef jdata = {};
			jdata.lowerAngle = data.lower;
			jdata.upperAngle = data.upper;
			jdata.enableLimit = data.limit;

			jdata.Initialize(data.body_first->get_body(), data.body_second->get_body(), data.body_second->get_body()->GetPosition());
			try_joint = Physics::GetWorld().CreateJoint(&jdata);
			break;
		}
		default:
			break;
	}

	created = true;
}

void Asura::Physics::PhysicsJoint::Destroy()
{
	if (destroyed)
		return;

	GetWorld().DestroyJoint(try_joint);
	destroyed = true;
}