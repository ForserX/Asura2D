#pragma once

namespace asura::physics
{
	void start();
	void init();
	void tick(float dt);
	void destroy();

	void destroy_world();

	b2World& get_world();
	b2Body* get_ground();
	
    math::frect get_body_rect(const physics_body* body);
	physics_body* schedule_creation(body_parameters parameters);
	physics_joint* schedule_creation(joint_data&& parameters);
	void schedule_free(physics_body* body);
	
	physics_body* hit_test(math::fvec2 pos);
}
