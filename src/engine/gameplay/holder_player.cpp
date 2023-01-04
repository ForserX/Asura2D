#include "pch.h"

using namespace Asura;
static bool Attached = false;
static EntityView* CurrentEntity = nullptr;

static int64_t HPInputID = 0;

void GamePlay::Holder::player::Init()
{
	auto KeyCallback = [](int16_t scan_code, Asura::Input::State state)
	{
		if (!Attached || holder_type != holder_mode::Player)
			return;

		auto TryEntt = Entities::Get<Entities::physics_body_component>(CurrentEntity->Get());
		auto pBody = TryEntt.body;

		if (scan_code == SDL_SCANCODE_W)
		{
			if (!pBody->IsFlying())
			{
				pBody->ApplyImpulse({ 0, pBody->get_mass() * 15});
			}
		}
		else if(scan_code == SDL_SCANCODE_A)
		{
			pBody->ApplyImpulse({ -700, 0 });
		}
		else if (scan_code == SDL_SCANCODE_D)
		{
			pBody->ApplyImpulse({ 700, 0 });
		}
	};

	HPInputID = Input::Emplace(KeyCallback);
}

void GamePlay::Holder::player::Tick()
{
	if (CurrentEntity && !Entities::IsValid(*CurrentEntity))
	{
		Detach();
	}
}

void GamePlay::Holder::player::Destroy()
{
	if (Attached)
	{
		Detach();
	}

	Input::Erase(HPInputID);
}

void GamePlay::Holder::player::Attach(EntityBase entity)
{
	CurrentEntity = new EntityView(entity);

	Physics::PhysicsBody* TryEntt = Entities::Get<Entities::physics_body_component>(CurrentEntity->Get()).body;
	TryEntt->BlockRotation(true);

	Attached = true;
	Camera::Attach(entity);
}

void GamePlay::Holder::player::Detach()
{
	Physics::PhysicsBody* TryEntt = Entities::Get<Entities::physics_body_component>(CurrentEntity->Get()).body;
	TryEntt->BlockRotation(false);

	delete CurrentEntity;
	CurrentEntity = nullptr;

	Camera::Detach();
	Attached = false;
}