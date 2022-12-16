#pragma once

namespace Asura::Physics
{
	class ContatctListerBase
	{
	public:
		ContatctListerBase() = default;
		virtual ~ContatctListerBase() = default;

		virtual void BeginContact(b2Contact* contact);
		virtual void EndContact(b2Contact* contact);

		virtual void BeginContact(PhysicsBody* contact);
		virtual void EndContact(PhysicsBody* contact);
	};
}
