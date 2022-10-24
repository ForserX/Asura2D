#pragma once
#include <box2d/box2d.h>

namespace ark
{
	class CollisionLister;
	
	namespace physics
	{
		// 4 bits
		enum class body_type : uint8_t
		{
			invalid = 0,

			static_body,
			dynamic_body,
			kinematic_body
		};

		inline
		b2BodyType 
		get_box2d_body_type(uint8_t type)
		{
			switch (static_cast<body_type>(type)) {
			case body_type::kinematic_body:
				return b2_kinematicBody;
				break;
			case body_type::static_body:
				return b2_staticBody;
				break;
			case body_type::dynamic_body:
				return b2_dynamicBody;
				break;
			default:
				ark_assert(false, "Invalid body type", {});
				break;
			}

			return b2_dynamicBody;
		}

		inline
		body_type 
		get_ark_body_type(b2BodyType type)
		{
			switch (type) {
			case b2_kinematicBody:
				return body_type::kinematic_body;
			case b2_staticBody:
				return body_type::static_body;
			case b2_dynamicBody:
				return body_type::dynamic_body;
			default:
				ark_assert(false, "Invalid body type", {});
				break;
			}

			return body_type::dynamic_body;
		}

		struct body_parameters
		{
			struct packed
			{
				packed(uint8_t in_type, uint8_t in_shape, uint8_t in_mat)
					: type(in_type), shape(in_shape), mat(in_mat) {}

				packed() 
					: type(0), shape(0), mat(0) {}

				uint8_t type : 4;
				uint8_t shape : 4;
				uint8_t mat;
			} packed_type;
			
			float mass = 0.f;
			float angle = 0.f;
			float angular_vel = 0.f;
			math::fvec2 vel;
            math::fvec2 pos;
            math::fvec2 size;
            math::fvec2 mass_center;

			body_parameters() = delete;
			body_parameters(
				float in_angle,
				float in_vel_angle,
				math::fvec2 in_vel,
				math::fvec2 in_pos,
				math::fvec2 in_size,
				body_type in_type,
				material::shape in_shape,
				material::type in_mat,
				float in_mass = 0.f,
				math::fvec2 in_mass_center = {}
			) :
			angle(in_angle),
			angular_vel(in_vel_angle),
			vel(in_vel),
			pos(in_pos),
			size(in_size),
			packed_type(static_cast<uint8_t>(in_type), static_cast<uint8_t>(in_shape), static_cast<uint8_t>(in_mat)),
			mass(in_mass)
			{
				if (in_mass_center.empty()) {
					mass_center = { size.x() / 2, size.y() / 2 };
				} else {
					mass_center = in_mass_center;
				}
			}

			body_parameters(stl::stream_vector& data)
			{
				deserialize(data);
			}

			operator b2BodyDef() const
			{
				b2BodyDef body_def = {};
				body_def.angle = angle;
				body_def.angularVelocity = angular_vel;
                body_def.linearVelocity = b2Vec2(vel);
				body_def.position = b2Vec2(pos);
				body_def.type = get_box2d_body_type(packed_type.type);
				return body_def;
			}

			operator b2MassData() const
			{
				b2MassData mass_data = {};
				mass_data.center = b2Vec2(mass_center);
				mass_data.mass = mass;
				return mass_data;
			}

			template<bool full_serialization = true>
			void serialize(stl::stream_vector& data) const
			{
				stl::push_memory(data, angle);
				stl::push_memory(data, angular_vel);
				stl::push_memory(data, mass);
				stl::push_memory(data, mass_center);
				stl::push_memory(data, vel);
				stl::push_memory(data, pos);
				stl::push_memory(data, size);
				stl::push_memory(data, packed_type);
			}

			template<bool full_serialization = true>
			void deserialize(stl::stream_vector& data)
			{
				stl::read_memory(data, angle);
				stl::read_memory(data, angular_vel);
				stl::read_memory(data, mass);
				stl::read_memory(data, mass_center);
				stl::read_memory(data, vel);
				stl::read_memory(data, pos);
				stl::read_memory(data, size);
				stl::read_memory(data, packed_type);
			}
		};
		
		class physics_body
		{
		private:
			bool created = false;
			bool destroyed = false;
			b2Body* body = nullptr;
			body_parameters parameters;
			
		public:
			physics_body() = delete;
			physics_body(body_parameters in_parameters);
			~physics_body();
			
			bool is_created() const { return created; }
			bool is_destroyed() const { return destroyed; }
			bool is_enabled() const { return (body ? body->IsEnabled() : false); }
			
			b2Body* get_body() const { return body; }
			const body_parameters& get_parameters() const { return parameters; }

		public:
			body_type get_body_type() const;
			float get_mass() const;
            math::fvec2 get_mass_center() const;
			float get_angle() const;
			float get_angular_velocity() const;
			math::fvec2 get_velocity() const;
			math::fvec2 get_position() const;

		public:
			void set_body_type(body_type new_type);
			void set_mass(float new_mass);
			void set_mass_center(const math::fvec2& new_center);
			void set_angle(float new_angle);
			void set_angular_velocity(float new_angular_vel);
			void set_velocity(const math::fvec2& new_vel);
			void set_position(const math::fvec2& new_pos);

			void apply_impulse(const math::fvec2& impulse);
			void apply_angular_impulse(float impulse);

		public:
			body_parameters copy_parameters() const;

		public:
			void create();
			void destroy();
		};
		
		class world
		{
		private:
			bool enable_thread = false;

			std::atomic_bool destroy_thread;
			std::unique_ptr<std::thread> physics_thread;
			
			std::unique_ptr<b2World> world_holder;
			b2Body* ground = nullptr;

			std::unique_ptr<CollisionLister> cl;

			stl::hash_set<physics_body*> scheduled_to_delete_bodies;
			stl::hash_set<physics_body*> bodies;

		private:
			void destroy_all_bodies();
			void pre_tick();
			void debug_joints_tick();
			void internal_tick(float dt);
            math::frect get_real_body_position(b2Body* body);
			
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
			math::frect get_body_position(const physics_body* body);
			
		public:
			physics_body* schedule_creation(body_parameters parameters);
			void schedule_free(physics_body* body);
		};
	}
}

extern float physics_delta;
extern float physics_real_delta;
extern int target_steps_count;
