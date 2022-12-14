#pragma once
#include <box2d/box2d.h>

namespace Asura::Physics
{
	enum class JointTypes
	{
		Base,
		Motor,
		Revolute,
		Mouse
	};

	struct joint_data
	{
		PhysicsBody* body_first;
		PhysicsBody* body_second;

		float lower;
		float upper;

		JointTypes type;

		bool limit;
	};

	class PhysicsJoint
	{
	private:
		bool created = false;
		bool destroyed = false;
		b2Joint* try_joint = nullptr;
		joint_data data;

	public:
		PhysicsJoint() = delete;
		PhysicsJoint(joint_data&& local_data) : data(local_data) {};
		~PhysicsJoint();

		inline bool IsCreated() const { return created; }
		inline bool IsDestroyed() const { return destroyed; }

		inline b2Joint* Get() const { return try_joint; }
	public:
		void Create();
		void Destroy();
	};
}