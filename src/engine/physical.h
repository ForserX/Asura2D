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

	namespace physics
	{
		class world
		{
			std::unique_ptr<b2World> world_holder;
			std::unique_ptr<CollisionLister> cl;

		public:
			world();
			~world();
			
			void init();
			void destroy();
			void tick(float dt) const;

			fmatrix get_body_position(b2Body* body);
			void destroy_world();

			b2Body* create_static(b2Vec2 pos, b2Vec2 shape) const;
			b2Body* create_dynamic(b2Vec2 pos, b2Vec2 shape) const;
			b2Body* create_around(b2Vec2 pos, b2Vec2 shape) const;
		};
	}
}