#include "pch.h"

using namespace Asura;
static bool Attached = false;
static EntityView* CurrentEntity = nullptr;

static input::on_key_change hp_key_click_event;

void GamePlay::Holder::player::Init()
{
	auto editor_key = [](int16_t scan_code, Asura::input::key_state state)
	{
		if (!Attached || holder_type != holder_mode::player)
			return;

		auto TryEntt = Entities::get<Entities::physics_body_component>(CurrentEntity->Get());
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

	hp_key_click_event = input::subscribe_key_event(editor_key);
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

	input::unsubscribe_key_event(hp_key_click_event);
}

void GamePlay::Holder::player::Attach(EntityBase entity)
{
	CurrentEntity = new EntityView(entity);
	Attached = true;
	Camera::attach(entity);
}

void Asura::GamePlay::Holder::player::Detach()
{
	delete CurrentEntity;
	CurrentEntity = nullptr;

	Camera::detach();
	Attached = false;
}