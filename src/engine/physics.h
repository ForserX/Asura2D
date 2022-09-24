#pragma once

namespace ark::physics
{
	void start();
	void init();
	void tick(float dt);
	void destroy();

	void destroy_world();

	marl::Event& get_physics_event();
	b2World& get_world();
	b2Body* get_ground();
	
	ark_matrix get_body_position(physics_body* body);
	physics_body* schedule_creation(body_parameters parameters);
	void schedule_free(physics_body* body);
	
	physics_body* hit_test(ark_float_vec2 pos);
}