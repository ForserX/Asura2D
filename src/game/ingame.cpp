#include "ingame.h"

using namespace asura::systems;

asura::stl::vector<std::unique_ptr<asura::system>> pre_game_update_systems;
asura::stl::vector<std::unique_ptr<asura::system>> game_update_systems;
asura::stl::vector<std::unique_ptr<asura::system>> post_game_update_systems;
asura::stl::vector<std::unique_ptr<asura::system>> game_physics_systems;
asura::stl::vector<std::unique_ptr<asura::system>> game_draw_systems;

void init_systems()
{
	game_physics_systems.push_back(std::make_unique<ingame::movement_system>());
}

void ingame::pre_init()
{
	init_systems();

	for (auto& system : pre_game_update_systems) {
		add_system(system.get(), update_type::pre_update_schedule);
	}
	
	for (auto& system : game_update_systems) {
		add_system(system.get(), update_type::update_schedule);
	}

	for (auto& system : post_game_update_systems) {
		add_system(system.get(), update_type::post_update_schedule);
	}

	for (auto& system : game_physics_systems) {
		add_system(system.get(), update_type::physics_schedule);
	}
	
	for (auto& system : game_draw_systems) {
		add_system(system.get(), update_type::draw_schedule);
	}
}

auto camera_mouse_key_change = [](int16_t scan_code, asura::input::key_state state) {
	switch (scan_code) {
	case SDL_SCANCODE_MOUSE_LEFT: {
        if (asura::input::is_key_pressed(SDL_SCANCODE_LCTRL)) {
            if (state == asura::input::key_state::hold) {
                const auto& mouse_delta = asura::input::get_mouse_delta();
                asura::camera::move(asura::camera::cam_move::left, (mouse_delta.x * 0.05f));
                asura::camera::move(asura::camera::cam_move::up, (mouse_delta.y * 0.05f));
            }
        }
		break;
	}
	case SDL_SCANCODE_LEFT:
		asura::camera::move(asura::camera::cam_move::left, 1.f);
		break;
	case SDL_SCANCODE_RIGHT:
		asura::camera::move(asura::camera::cam_move::right, 1.f);
		break;
	case SDL_SCANCODE_UP:
		asura::camera::move(asura::camera::cam_move::up, 1.f);
		break;
	case SDL_SCANCODE_DOWN:
		asura::camera::move(asura::camera::cam_move::down, 1.f);
		break;
	default:
		break;
	}
};

static bool editor = false;
auto editor_key_change = [](int16_t scan_code, asura::input::key_state state) {
	if (scan_code == SDL_SCANCODE_X)
	{
		if (state == asura::input::key_state::press) {
			editor = !editor;
			asura::game::editor(editor);
		}
	}
};

auto camera_mouse_wheel_change = [](int16_t scan_code, float state) {
	switch (scan_code) {
	case SDL_SCANCODE_MOUSEWHEEL:
		asura::camera::zoom((-1.f * state) * 2.f);
		break;
	default:
		break;
	}
};

asura::input::on_key_change camera_mouse_key_event;
asura::input::on_key_change editor_key_event;
asura::input::on_input_change camera_camera_mouse_wheel_event;

asura::entity_view TestObject;
asura::entity_view TestObject2;

asura::stl::vector<asura::entity_view> circles;

void ingame::init()
{
	using namespace asura;
	using namespace entities;
	
#if 0
	TestObject = add_phys_body(create(), {}, { 50, 50 }, { 20, 10 });
	TestObject2 = add_phys_body(create(), {}, { 350, 100 }, { 200, 10 });

	add_field<drawable_flag>(TestObject);
	add_field<drawable_flag>(TestObject2);
	
	std::random_device r_device;
	std::mt19937 gen(r_device());
	for (size_t i = 0; i < 2000; i++) {
		std::uniform_real_distribution width_dist(260., 1300.);
		std::uniform_real_distribution height_dist(260., 1300.);
		const auto& ent = circles.emplace_back(
			add_phys_body(
				create(),
				{},
				{ static_cast<float>(width_dist(gen)), static_cast<float>(height_dist(gen)) },
				{ 25, 25 },
				physics::body_type::dynamic_body,
				material::shape::circle,
				material::type::rubber
			)
		);

		add_field<drawable_flag>(ent);
	}

#endif
	add_phys_body_preset(create(), {100, 30}, "Teeter.ini");

	editor_key_event = asura::input::subscribe_key_event(editor_key_change);
	camera_mouse_key_event = asura::input::subscribe_key_event(camera_mouse_key_change);
	camera_camera_mouse_wheel_event = asura::input::subscribe_input_event(camera_mouse_wheel_change);
}

void ingame::destroy()
{
	asura::input::unsubscribe_input_event(camera_camera_mouse_wheel_event);
	asura::input::unsubscribe_key_event(camera_mouse_key_event);
	asura::input::unsubscribe_key_event(editor_key_event);
}
