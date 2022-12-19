#pragma once

namespace Asura::GamePlay::Camera
{
	void Init();
	void Destroy();
	void Tick(float dt);

	void Move(MoveWays move, float point);
	void Zoom(float value);

	bool IsAttached();
	void Attach(EntityView entity);
	void Detach();
	
	void ResetView();
	void ResetHW();

	const Math::FVec2& Position();
    Math::FVec2 Screen2World(const Math::FVec2& screenPoint);
    Math::FVec2 World2Screen(const Math::FVec2& worldPoint);
	float ScaleFactor(float in);

	bool CanSee(Math::FVec2 Pos);
}
