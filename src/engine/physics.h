#pragma once

namespace ark::physics
{
	void init();
	void tick(float dt);
	void destroy();

	void destroy_world();
	b2World& get_world();
	fmatrix get_body_position(b2Body* body);
	
	b2Body* create_static(b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat = physics::material::material_type::solid);
	b2Body* create_dynamic(b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat = physics::material::material_type::solid);
	b2Body* create_dynamic_cricle(b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat = physics::material::material_type::solid);

	b2Body* hit_test(ImVec2 pos);
}