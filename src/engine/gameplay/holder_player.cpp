#include "pch.h"

using namespace Asura;
static bool Attached = false;
static EntityView* CurrentEntity = nullptr;

static int64_t HPInputID = 0;

void GamePlay::Holder::Player::Init()
{
	auto KeyCallback = [](int16_t scan_code, Asura::Input::State state)
	{
		if (!Attached || holder_type != holder_mode::Player)
			return;

		auto TryEntt = Entities::Get<Entities::physics_body_component>(CurrentEntity->Get());
		auto pBody = TryEntt.body;

		auto TryEnttTexInfo = Entities::TryGet<Entities::draw_texture_npc_component>(CurrentEntity->Get());
		if (scan_code == GLFW_KEY_W)
		{
			if (!pBody->IsFlying())
			{
				pBody->ApplyImpulse({ 0, pBody->get_mass() * 15});
			}
		}
		else if(scan_code == GLFW_KEY_A)
		{
			pBody->ApplyImpulse({ pBody->get_mass() * -10.f, 0 });
			TryEnttTexInfo->MirrorX = true;
		}
		else if (scan_code == GLFW_KEY_D)
		{
			pBody->ApplyImpulse({ pBody->get_mass() * 10.f, 0 });
			TryEnttTexInfo->MirrorX = false;
		}
	};

	HPInputID = Input::Emplace(KeyCallback);
}

void GamePlay::Holder::Player::Tick()
{
	if (CurrentEntity && !Entities::IsValid(*CurrentEntity))
	{
		Unbind();
	}
}

void GamePlay::Holder::Player::Destroy()
{
	if (Attached)
	{
		Unbind();
	}

	Input::Erase(HPInputID);
}

void GamePlay::Holder::Player::Bind(EntityView entity)
{
	CurrentEntity = new EntityView(entity);

	Physics::PhysicsBody* TryEntt = Entities::Get<Entities::physics_body_component>(CurrentEntity->Get()).body;
	//TryEntt->BlockRotation(true);

	Attached = true;
	Camera::Attach(entity);
}

void GamePlay::Holder::Player::Unbind()
{
	Physics::PhysicsBody* TryEntt = Entities::Get<Entities::physics_body_component>(CurrentEntity->Get()).body;
	TryEntt->BlockRotation(false);

	delete CurrentEntity;
	CurrentEntity = nullptr;

	Camera::Detach();
	Attached = false;
}