#pragma once

namespace Asura::material
{
	struct data
	{
		float friction = 0.01f;
		float restitution = 0.0f;
		float density = 2.5f;

		bool ignore_collision = false;
	};

	enum class type : uint8_t
	{
		invalid = 0,

		solid,
		rubber,

		out_of = uint8_t(-1)
	};

	// 4 bits
	enum class shape : uint8_t
	{
		invalid = 0,

		box,
		circle
	};

	void Init();
	void Destroy();

	[[nodiscard]] const data& get(type type);
}