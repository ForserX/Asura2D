#pragma once
#include <box2d/box2d.h>

namespace ark
{
	namespace physics
	{
		enum class joint_type
		{
			base,
			motor, 
			revolute,
			mouse
		};

		struct joint_data
		{
			physics_body* body_first;
			physics_body* body_second;

			float lower;
			float upper;

			joint_type type;

			bool limit;
		};

		class physics_joint
		{
		private:
			bool created = false;
			bool destroyed = false;
			b2Joint* try_joint = nullptr;
			joint_data data;

		public:
			physics_joint() = delete;
			physics_joint(joint_data&& local_data) : data(local_data) {};
			~physics_joint();
			
			inline bool is_created() const { return created; }
			inline bool is_destroyed() const { return destroyed; }
			
			inline b2Joint* get() const { return try_joint; }
		public:
			void create();
			void destroy();
		};
	}
}