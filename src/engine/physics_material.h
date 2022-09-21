#pragma once

namespace ark::physics::material
{
	struct material_data
	{
		float friction = 0.4;
		float restitution = 0.f;
		float density = 0.5f;

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