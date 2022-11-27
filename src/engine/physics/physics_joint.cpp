#include "pch.h"

using namespace ark;

ark::physics::physics_joint::~physics_joint()
{
	destroy();
}

void ark::physics::physics_joint::create()
{
	switch (data.type)
	{
		case ark::physics::joint_type::base:
			break;
		case ark::physics::joint_type::mouse:
			break;
		case ark::physics::joint_type::motor:
			break;
		case ark::physics::joint_type::revolute:
		{
			b2RevoluteJointDef jdata = {};
			jdata.lowerAngle = data.lower;
			jdata.upperAngle = data.upper;
			jdata.enableLimit = data.limit;

			jdata.Initialize(data.body_first->get_body(), data.body_second->get_body(), data.body_second->get_body()->GetPosition());
			try_joint = physics::get_world().CreateJoint(&jdata);
			break;
		}
		default:
			break;
	}

	created = true;
}

void ark::physics::physics_joint::destroy()
{
	if (destroyed)
		return;

	get_world().DestroyJoint(try_joint);
	destroyed = true;
}