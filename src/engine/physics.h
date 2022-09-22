#pragma once

namespace ark::physics
{
	void start();
	void init();
	void tick(float dt);
	void destroy();

	void destroy_world();

	marl::mutex& get_physics_mutex();
	b2World& get_world();
	
	ark_matrix get_body_position(physics_body* body);
	physics_body* schedule_creation(body_parameters parameters);
	void schedule_free(physics_body* body);
	
	b2Body* hit_test(ImVec2 pos);
}