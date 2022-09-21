#pragma once
#include <box2d/box2d.h>

namespace ark
{
	class CollisionLister;

	struct fmatrix
	{
		float x;
		float y;
		float h;
		float w;
	};

	class merry_boar
	{
		std::unique_ptr<b2World> world;
		std::unique_ptr<CollisionLister> cl;

		b2Body* ground = nullptr;

	public:
		merry_boar();
		~merry_boar() = default;

		void tick(float dt);

		fmatrix get_body_position(b2Body* body);

		b2Body* create_ground(b2Vec2 pos, b2Vec2 shape);
		void destroy_world();
		b2Body* create_body(b2Vec2 pos, b2Vec2 shape);
	};

	extern merry_boar physical;
	using physics = merry_boar;
}