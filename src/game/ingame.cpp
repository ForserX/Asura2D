#include "ingame.h"

using namespace Asura::Systems;

Asura::stl::vector<std::unique_ptr<Asura::system>> pre_game_update_systems;
Asura::stl::vector<std::unique_ptr<Asura::system>> game_update_systems;
Asura::stl::vector<std::unique_ptr<Asura::system>> post_game_update_systems;
Asura::stl::vector<std::unique_ptr<Asura::system>> game_physics_systems;
Asura::stl::vector<std::unique_ptr<Asura::system>> game_draw_systems;

void init_systems()
{
	game_physics_systems.push_back(std::make_unique<ingame::movement_system>());
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

auto camera_mouse_key_change = [](int16_t scan_code, Asura::input::key_state state)
{
	switch (scan_code) {
	case SDL_SCANCODE_MOUSE_LEFT: {
        if (Asura::input::is_key_pressed(SDL_SCANCODE_LCTRL)) {
            if (state == Asura::input::key_state::hold) {
                const auto& mouse_delta = Asura::input::get_mouse_delta();
                Asura::Camera::move(Asura::Camera::cam_move::left, (mouse_delta.x * 0.05f));
                Asura::Camera::move(Asura::Camera::cam_move::up, (mouse_delta.y * 0.05f));
            }
        }
		break;
	}
	case SDL_SCANCODE_LEFT:
		Asura::Camera::move(Asura::Camera::cam_move::left, 1.f);
		break;
	case SDL_SCANCODE_RIGHT:
		Asura::Camera::move(Asura::Camera::cam_move::right, 1.f);
		break;
	case SDL_SCANCODE_UP:
		Asura::Camera::move(Asura::Camera::cam_move::up, 1.f);
		break;
	case SDL_SCANCODE_DOWN:
		Asura::Camera::move(Asura::Camera::cam_move::down, 1.f);
		break;
	default:
		break;
	}
};

static bool editor = false;
auto editor_key_change = [](int16_t scan_code, Asura::input::key_state state) 
{
	if (scan_code == SDL_SCANCODE_X)
	{
		if (state == Asura::input::key_state::press)
		{
			editor = !editor;
			Asura::game::editor(editor);
		}
	}
};

auto camera_mouse_wheel_change = [](int16_t scan_code, float state)
{
	switch (scan_code) 
	{
	case SDL_SCANCODE_MOUSEWHEEL:
		Asura::Camera::zoom((-1.f * state) * 2.f);
		break;
	default:
		break;
	}
};

Asura::input::on_key_change camera_mouse_key_event;
Asura::input::on_key_change editor_key_event;
Asura::input::on_input_change camera_camera_mouse_wheel_event;

Asura::EntityView TestObject;
Asura::EntityView TestObject2;

Asura::stl::vector<Asura::EntityView> circles;

void ingame::init()
{
	using namespace Asura;
	using namespace Entities;
	
#if 1
	TestObject = AddPhysBody(Create(), {}, { 50, 50 }, { 20, 10 });
	TestObject2 = AddPhysBody(Create(), {}, { 350, 100 }, { 200, 10 });

	add_field<drawable_flag>(TestObject);
	add_field<drawable_flag>(TestObject2);
	
	std::random_device r_device;
	std::mt19937 gen(r_device());
	for (size_t i = 0; i < 2000; i++) {
		std::uniform_real_distribution width_dist(260., 1300.);
		std::uniform_real_distribution height_dist(260., 1300.);
		const auto& ent = circles.emplace_back(
			AddPhysBody(
				Create(),
				{},
				{ static_cast<float>(width_dist(gen)), static_cast<float>(height_dist(gen)) },
				{ 25, 25 },
				Physics::body_type::ph_dynamic,
				Physics::Material::shape::circle,
				Physics::Material::type::rubber
			)
		);

		add_field<drawable_flag>(ent);
	}

#else
	AddPhysBodyPreset(Create(), {100, 30}, "Teeter.ini");
#endif
	editor_key_event = Asura::input::subscribe_key_event(editor_key_change);
	camera_mouse_key_event = Asura::input::subscribe_key_event(camera_mouse_key_change);
	camera_camera_mouse_wheel_event = Asura::input::subscribe_input_event(camera_mouse_wheel_change);
}

void ingame::destroy()
{
	Asura::input::unsubscribe_input_event(camera_camera_mouse_wheel_event);
	Asura::input::unsubscribe_key_event(camera_mouse_key_event);
	Asura::input::unsubscribe_key_event(editor_key_event);
}
