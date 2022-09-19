#pragma once
#include <box2d/box2d.h>

namespace ark
{
	class merry_boar
	{
		std::unique_ptr<b2World> world;
		b2Body* ground = nullptr;

	public:
		merry_boar();
		~merry_boar() = default;

		void create_ground(b2Vec2 pos, b2Vec2 shape);
		void destroy_world();
		b2Body* create_body(b2Vec2 pos, b2Vec2 shape);
	};

	using physics = merry_boar;
}