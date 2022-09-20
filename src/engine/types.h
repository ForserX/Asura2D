#pragma once

struct ark_int_vec2
{
	union
	{
		struct
		{
			int16 x;
			int16 y;
		};
		int16 xy[2];
	};
};