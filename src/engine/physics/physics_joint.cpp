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
		case Asura::Physics::JointTypes::Base:
			break;
		case Asura::Physics::JointTypes::Mouse:
			break;
		case Asura::Physics::JointTypes::Motor:
			break;
		case Asura::Physics::JointTypes::Revolute:
		{
			b2RevoluteJointDef jdata = {};
			jdata.lowerAngle = data.lower;
			jdata.upperAngle = data.upper;
			jdata.enableLimit = data.limit;

			jdata.Initialize(data.body_first->get_body(), data.body_second->get_body(), data.body_second->get_body()->GetPosition());
			try_joint = Physics::GetWorld().GetWorld().CreateJoint(&jdata);
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

	GetWorld().GetWorld().DestroyJoint(try_joint);
	destroyed = true;
}