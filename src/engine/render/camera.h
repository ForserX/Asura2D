#pragma once

namespace Asura::Camera
{
	enum class cam_move : int8_t 
	{
		left,
		right,
		up,
		down
	};

	void Init();
	void Destroy();
	void Tick(float dt);

	void Move(cam_move move, float point);
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

}
