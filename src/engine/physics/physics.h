#pragma once

namespace Asura::Physics
{
	void start();
	void Init();
	void Tick(float dt);
	void Destroy();

	void DestroyWorld();

	b2World& GetWorld();
	PhysicsBody* get_ground();
	
    Math::FRect get_body_rect(const PhysicsBody* body);
	PhysicsBody* schedule_creation(body_parameters parameters);
	PhysicsJoint* schedule_creation(joint_data&& parameters);
	void schedule_free(PhysicsBody* body);
	
	PhysicsBody* hit_test(Math::FVec2 pos);
}
