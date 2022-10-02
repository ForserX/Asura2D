#pragma once
#include <box2d/box2d.h>

namespace ark
{
	class CollisionLister;
	
	namespace physics
	{
		enum class body_shape : int64_t
		{
			box_shape,
			circle_shape
		};
		
		enum class body_type : int64_t
		{
			static_body,
			dynamic_body,
			kinematic_body
		};
		
		struct body_parameters
		{
			float angle = 0.f;
			float vel_angle = 0.f;
			ark_float_vec2 vel;
			ark_float_vec2 pos;
			ark_float_vec2 size;
			body_type type;
			body_shape shape;
			material::type mat;
			float mass;
			ark_float_vec2 mass_center;
			
			body_parameters() = delete;
			body_parameters(
				float in_angle,
				float in_vel_angle,
				ark_float_vec2 in_vel,
				ark_float_vec2 in_pos,
				ark_float_vec2 in_size,
				body_type in_type,
				body_shape in_shape,
				material::type in_mat,
				float in_mass = 0.f,
				ark_float_vec2 in_mass_center = {}
			) :
			angle(in_angle),
			vel_angle(in_vel_angle),
			vel(in_vel),
			pos(in_pos),
			size(in_size),
			type(in_type),
			shape(in_shape),
			mat(in_mat),
			mass(in_mass)
			{
				if (in_mass_center.empty()) {
					mass_center = { size.x / 2, size.y / 2 };
				} else {
					mass_center = in_mass_center;
				}
			}

			operator b2BodyDef() const
			{
				b2BodyDef body_def = {};
				body_def.angle = angle;
				body_def.angularVelocity = vel_angle;
				body_def.linearVelocity.Set(vel.x, vel.y);
				body_def.position.Set(pos.x, pos.y);
				body_def.type = (type == body_type::kinematic_body) ? b2_kinematicBody : (type == body_type::static_body) ? b2_staticBody : b2_dynamicBody;
				return body_def;
			}

			operator b2MassData() const
			{
				b2MassData mass_data = {};
				mass_data.center = mass_center;
				mass_data.mass = mass;
				return mass_data;
			}
		};
		
		class physics_body
		{
		private:
			bool created = false;
			bool destroyed = false;
			b2Body* body = nullptr;
			body_parameters parameters;

		private:
			ark_float_vec2 proxy_position = {};
			
		public:
			physics_body() = delete;
			physics_body(body_parameters in_parameters);
			~physics_body();
			
			bool is_created() const { return created; }
			bool is_destroyed() const { return destroyed; }
			
			b2Body* get_body() const { return body; }
			const body_parameters& get_parameters() const { return parameters; }
			const ark_float_vec2& get_position();

			body_parameters copy_parameters() const;

			void create();
			void destroy();
		};
		
		class world
		{
		private:
			bool enable_thread = false;

			marl::Event thread_destroyed_event;
			std::atomic_bool destroy_thread;
			std::unique_ptr<std::jthread> physics_thread;
			
			std::unique_ptr<b2World> world_holder;
			std::unique_ptr<DebugDraw> world_dbg_draw;
			b2Body* ground = nullptr;

			std::unique_ptr<CollisionLister> cl;

			stl::hash_set<physics_body*> scheduled_to_delete_bodies;
			stl::hash_set<physics_body*> bodies;

		private:
			void destroy_all_bodies();
			void pre_tick();
			void joints_tick();
			void internal_tick(float dt);
			ark_matrix get_real_body_position(b2Body* body);

		public:
			marl::Event physics_event;
			
		public:
			world();
			~world();

			void start();
			void init();
			void destroy();
			void tick(float dt);
			
			b2World& get_world() const;
			b2Body* get_ground() const;

			void destroy_world();
		
		public:
			ark_matrix get_body_position(const physics_body* body);
			
		public:
			physics_body* schedule_creation(body_parameters parameters);
			void schedule_free(physics_body* body);
		};
	}
}

extern float physics_delta;
extern float physics_real_delta;
extern int target_steps_count;