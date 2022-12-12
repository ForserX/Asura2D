#pragma once

namespace Asura::Physics
{
	void Start();
	void Init();
	void Tick(float dt);
	void Destroy();

	void DestroyWorld();

	b2World& GetWorld();
	PhysicsBody* GetGround();
	
    Math::FRect GetBodyRect(const PhysicsBody* body);
	PhysicsBody* SafeCreation(body_parameters parameters);
	PhysicsJoint* SafeCreation(joint_data&& parameters);
	void SafeFree(PhysicsBody* body);
	
	PhysicsBody* HitTest(Math::FVec2 pos);
}
