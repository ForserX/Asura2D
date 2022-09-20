#pragma once

namespace ark::entities
{
	struct primitive_component
	{
		bool is_collidable = false;
		bool is_drawable = true;
		std::vector<ark_int_vec2> points;
	};
}
