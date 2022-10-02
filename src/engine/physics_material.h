#pragma once

namespace ark::material
{
	struct data
	{
		float friction = 0.01f;
		float restitution = 0.0f;
		float density = 2.5f;

		bool ignore_collision = false;
	};

	enum class type : int64_t
	{
		solid,
		rubber,

		out_of
	};

	void init();
	void destroy();

	[[nodiscard]] const data& get(type type);
}