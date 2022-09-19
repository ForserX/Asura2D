#pragma once
#include <box2d/box2d.h>

namespace ark
{
	class merry_boar
	{
		std::unique_ptr<b2World> world;
		std::unique_ptr<b2Body> ground;

	public:
		merry_boar();
		~merry_boar() = default;

		void create_ground(b2Vec2 base, b2Vec2 shape);
	};

	using physics = merry_boar;
}