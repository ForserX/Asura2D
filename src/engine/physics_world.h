#pragma once
#include <box2d/box2d.h>

namespace ark
{
	class CollisionLister;
	
	namespace physics
	{
		enum class body_type
		{
			static_body,
			around_body,
			dynamic_body
		};
		
		struct body_parameters
		{
			ark_float_vec2 pos;
			ark_float_vec2 size;
			body_type type;
			material::type mat;

			body_parameters() = delete;
			body_parameters(ark_float_vec2 in_pos, ark_float_vec2 in_size, body_type in_type, material::type in_mat = material::type::solid)
				: pos(in_pos), size(in_size), type(in_type), mat(in_mat) {}
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

		private:
			void create_static();
			void create_around();
			void create_dynamic();
			
		public:
			physics_body() = delete;
			physics_body(body_parameters in_parameters);
			~physics_body();
			
			bool is_created() const { return created; }
			bool is_destroyed() const { return destroyed; }
			
			b2Body* get_body() const { return body; }
			const body_parameters& get_parameters() const { return parameters; }
			const ark_float_vec2& get_position();

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

			std::unordered_set<physics_body*> scheduled_to_delete_bodies;
			std::unordered_set<physics_body*> bodies;

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