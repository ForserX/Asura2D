#include "pch.h"

using namespace asura;

asura::physics::physics_joint::~physics_joint()
{
	destroy();
}

void asura::physics::physics_joint::create()
{
	switch (data.type)
	{
		case asura::physics::joint_type::base:
			break;
		case asura::physics::joint_type::mouse:
			break;
		case asura::physics::joint_type::motor:
			break;
		case asura::physics::joint_type::revolute:
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

void asura::physics::physics_joint::destroy()
{
	if (destroyed)
		return;

	get_world().DestroyJoint(try_joint);
	destroyed = true;
}