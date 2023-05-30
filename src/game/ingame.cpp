#include "ingame.h"

using namespace Asura;
using namespace Asura::Systems;

stl::vector<std::unique_ptr<Asura::ISystem>> pre_game_update_systems;
stl::vector<std::unique_ptr<Asura::ISystem>> game_update_systems;
stl::vector<std::unique_ptr<Asura::ISystem>> post_game_update_systems;
stl::vector<std::unique_ptr<Asura::ISystem>> game_physics_systems;
stl::vector<std::unique_ptr<Asura::ISystem>> game_draw_systems;

std::random_device r_device;
static std::mt19937 gen(r_device());

using namespace Entities;
#if 1
class RandomSys final : public Asura::ISystem
{

public:
	RandomSys() = default;

	virtual void Init() override {};
	virtual void Reset() override {};
	virtual void Tick(float) override
	{
		static stl::uniform_dist r_dist(0.f, 1000.f);
		static stl::uniform_dist width_dist(260., 1300.);
		static stl::uniform_dist height_dist(260., 1300.);

		if ((int)r_dist(gen) < 980)
			return;

		Physics::body_parameters RandGenParam
		(
			0.f, 0.f, {},
			{ width_dist(gen), height_dist(gen) },
			{ 25, 25 },
			Physics::BodyType::Dynamic,
			Physics::Material::shape::circle,
			Physics::Material::type::rubber
		);


		const auto& Ent = CreatePhysBody(RandGenParam);
		AddField<drawable_flag>(Ent);
		AddTexture(Ent, "textures/ball.png", false);
	};
};

EntityView TestObject2;
class ContactLister : public Physics::ContatctListerBase
{
public:
	virtual void BeginContact(b2Contact* Contact) override
	{
		static stl::uniform_dist_t<int16_t> r_dist(1, 7);

		auto BodyA = Physics::PhysicsBody(Contact->GetFixtureA()->GetBody());
		auto BodyB = Physics::PhysicsBody(Contact->GetFixtureB()->GetBody());

		auto TryEntt = Entities::TryGet<Entities::physics_body_component>(TestObject2.Get());
		if (TryEntt == nullptr)
			return;

		auto& TryEnttBody = *TryEntt->body;
		if (BodyA == TryEnttBody)
		{
			// Destroy dynamic objects only
			if (BodyB.GetType() == Physics::BodyType::Dynamic)
			{
				std::string Name = "ball\\ball_blob_rr_0" + std::to_string(r_dist(gen));
				Name += ".opus";

				Audio::Start(Name);
				Physics::SafeFree(new Physics::PhysicsBody(std::move(BodyB)));
			}
		}
		else if (BodyB == TryEnttBody)
		{
			// Destroy dynamic objects only
			if (BodyA.GetType() == Physics::BodyType::Dynamic)
			{
				std::string Name = "ball\\ball_blob_rr_0" + std::to_string(r_dist(gen));
				Name += ".opus";

				Audio::Start(Name);

				Physics::SafeFree(new Physics::PhysicsBody(std::move(BodyA)));
			}
		}
	}
};
#endif

void init_systems()
{
	game_update_systems.emplace_back(std::make_unique<RandomSys>());
}

void ingame::pre_init()
{
	init_systems();

	for (auto& system : pre_game_update_systems) 
	{
		Subscribe(system.get(), UpdateType::befor);
	}
	
	for (auto& system : game_update_systems) 
	{
		Subscribe(system.get(), UpdateType::update);
	}

	for (auto& system : post_game_update_systems) 
	{
		Subscribe(system.get(), UpdateType::after);
	}

	for (auto& system : game_physics_systems) 
	{
		Subscribe(system.get(), UpdateType::physics);
	}
	
	for (auto& system : game_draw_systems) 
	{
		Subscribe(system.get(), UpdateType::render);
	}
}

static bool editor = false;
auto editor_key_change = [](int16_t scan_code, Input::State state)
{
	if (scan_code == GLFW_KEY_X)
	{
		if (state == Input::State::Press)
		{
			editor = !editor;
			game::editor(editor);
		}
	}
};

int64_t editor_key_event;

void ingame::init()
{
	using namespace Asura;
	using namespace Entities;
	
	size_t CursorID = UI::CursorManager::Register("textures/cursor/base.png");
	UI::CursorManager::SetActive(CursorID);

#if 0
	TestObject2 = CreatePhysBody(Physics::body_parameters(0.f, 0.f, {}, { 550, 300 }, { 60, 100 }));
	AddField<drawable_flag>(TestObject2);
	AddField<draw_texture_npc_component>(TestObject2);
	AddTexture(TestObject2, "textures/npc_test.png", false);
	holder_type = GamePlay::holder_mode::Player;
	GamePlay::Holder::Player::Bind(TestObject2.Get());
#else 
	Physics::GetWorld().SetContactLister(new ContactLister);
	TestObject2 = CreatePhysBody(Physics::body_parameters(0.f, 0.f, {}, { 350, 100 }, { 100, 10 }));
	AddField<drawable_flag>(TestObject2);

	holder_type = GamePlay::holder_mode::PlayerFree;
	GamePlay::Holder::PlayerFree::Bind(TestObject2.Get());
#endif
	editor_key_event = Input::Emplace(editor_key_change);
}

void ingame::destroy()
{
	Input::Erase(editor_key_event);
}
