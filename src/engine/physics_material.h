#pragma once

namespace ark::physics::material
{
	struct material_data
	{
		float friction = 0.01f;
		float restitution = 0.0f;
		float density = 2.5f;

		bool ignore_collision = false;
	};

	enum class material_type : size_t
	{
		solid,
		rubber,

		out_of
	};

	void init();
	void destroy();

	[[nodiscard]] const material_data& get(material_type type);
}