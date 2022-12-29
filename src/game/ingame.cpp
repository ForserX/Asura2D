#include "ingame.h"

using namespace Asura;
using namespace Asura::Systems;

stl::vector<std::unique_ptr<Asura::system>> pre_game_update_systems;
stl::vector<std::unique_ptr<Asura::system>> game_update_systems;
stl::vector<std::unique_ptr<Asura::system>> post_game_update_systems;
stl::vector<std::unique_ptr<Asura::system>> game_physics_systems;
stl::vector<std::unique_ptr<Asura::system>> game_draw_systems;

stl::vector<EntityView> circles;
std::random_device r_device;
static std::mt19937 gen(r_device());

using namespace Entities;

class random_a final : public Asura::system
{

public:
	random_a() = default;
	random_a(random_a&&) = default;

	virtual void Init() override {};
	virtual void Reset() override {};
	virtual void Tick(float) override
	{
		static std::uniform_real_distribution r_dist(0.f, 50.f);
		static std::uniform_real_distribution width_dist(260., 1300.);
		static std::uniform_real_distribution height_dist(260., 1300.);

		if ((float)r_dist(gen) < 47)
			return;

		Physics::body_parameters RandGenParam
		(
			0.f, 0.f, {},
			{ static_cast<float>(width_dist(gen)), static_cast<float>(height_dist(gen)) },
			{ 25, 25 },
			Physics::body_type::ph_dynamic,
			Physics::Material::shape::circle,
			Physics::Material::type::rubber
		);


		const auto& ent = circles.emplace_back(CreatePhysBody(RandGenParam));
		AddField<drawable_flag>(ent);
	};
};

void init_systems()
{
	game_update_systems.emplace_back(std::make_unique<random_a>());
}


void ingame::pre_init()
{
	init_systems();

	for (auto& system : pre_game_update_systems) 
	{
		add_system(system.get(), update_type::pre_update_schedule);
	}
	
	for (auto& system : game_update_systems) 
	{
		add_system(system.get(), update_type::update_schedule);
	}

	for (auto& system : post_game_update_systems) 
	{
		add_system(system.get(), update_type::post_update_schedule);
	}

	for (auto& system : game_physics_systems) 
	{
		add_system(system.get(), update_type::physics_schedule);
	}
	
	for (auto& system : game_draw_systems) 
	{
		add_system(system.get(), update_type::draw_schedule);
	}
}

static bool editor = false;
auto editor_key_change = [](int16_t scan_code, Input::key_state state)
{
	if (scan_code == SDL_SCANCODE_X)
	{
		if (state == Input::key_state::press)
		{
			editor = !editor;
			game::editor(editor);
		}
	}
};

int64_t editor_key_event;

EntityView TestObject2;

class ContactLister : public Physics::ContatctListerBase
{
public:
	virtual void BeginContact(b2Contact* Contact) override
	{
		static std::uniform_real_distribution r_dist(1.f, 7.f);

		auto BodyA = Physics::PhysicsBody(Contact->GetFixtureA()->GetBody());
		auto BodyB = Physics::PhysicsBody(Contact->GetFixtureB()->GetBody());

		auto TryEntt = Entities::TryGet<Entities::physics_body_component>(TestObject2.Get());
		if (TryEntt == nullptr)
			return;

		auto& TryEnttBody = *TryEntt->body;
		if (BodyA == TryEnttBody)
		{
			// Destroy dynamic objects only
			if (BodyB.GetType() == Physics::body_type::ph_dynamic)
			{
				std::string Name = "ball\\ball_blob_rr_0" + std::to_string((int)r_dist(gen));
				Name += ".opus";

				Audio::Start(Name);
				Physics::SafeFree(new Physics::PhysicsBody(std::move(BodyB)));
			}
		}
		else if (BodyB == TryEnttBody)
		{
			// Destroy dynamic objects only
			if (BodyA.GetType() == Physics::body_type::ph_dynamic)
			{
				std::string Name = "ball\\ball_blob_rr_0" + std::to_string((int)r_dist(gen));
				Name += ".opus";

				Audio::Start(Name);

				Physics::SafeFree(new Physics::PhysicsBody(std::move(BodyA)));
			}
		}
	}
};

void ingame::init()
{
	using namespace Asura;
	using namespace Entities;
	
	Physics::GetWorld().SetContactLister(new ContactLister);
#if 1

	TestObject2 = CreatePhysBody(Physics::body_parameters(0.f, 0.f, {}, { 350, 100 }, { 100, 10 }));

	AddField<drawable_flag>(TestObject2);

#else
	AddPhysBodyPreset(Create(), {100, 30}, "Teeter.ini");
#endif

	holder_type = GamePlay::holder_mode::PlayerFree;

	GamePlay::Holder::PlayerFree::Bind(TestObject2.Get());

	editor_key_event = Input::Emplace(editor_key_change);
}

void ingame::destroy()
{
	Input::Erase(editor_key_event);
}
