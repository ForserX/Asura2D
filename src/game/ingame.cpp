#include "ingame.h"

using namespace ark::systems;

ark::stl::vector<std::unique_ptr<ark::system>> pre_game_update_systems;
ark::stl::vector<std::unique_ptr<ark::system>> game_update_systems;
ark::stl::vector<std::unique_ptr<ark::system>> post_game_update_systems;
ark::stl::vector<std::unique_ptr<ark::system>> game_physics_systems;
ark::stl::vector<std::unique_ptr<ark::system>> game_draw_systems;

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

auto camera_mouse_key_change = [](int16_t scan_code, ark::input::key_state state) {
	switch (scan_code) {
	case SDL_SCANCODE_MOUSE_MIDDLE: {
		if (state == ark::input::key_state::hold) {
			const auto& mouse_delta = ark::input::get_mouse_delta();
			ark::camera::move(ark::camera::cam_move::left, (mouse_delta.x * 0.05f));
			ark::camera::move(ark::camera::cam_move::up, (mouse_delta.y * 0.05f));
		}
		break;
	}
	case SDL_SCANCODE_LEFT:
		ark::camera::move(ark::camera::cam_move::left, 1.f);
		break;
	case SDL_SCANCODE_RIGHT:
		ark::camera::move(ark::camera::cam_move::right, 1.f);
		break;
	case SDL_SCANCODE_UP:
		ark::camera::move(ark::camera::cam_move::up, 1.f);
		break;
	case SDL_SCANCODE_DOWN:
		ark::camera::move(ark::camera::cam_move::down, 1.f);
		break;
	default:
		break;
	}
};

auto camera_mouse_wheel_change = [](int16_t scan_code, float state) {
	switch (scan_code) {
	case SDL_SCANCODE_MOUSEWHEEL:
		ark::camera::zoom((-1.f * state) * 2.f);
		break;
	default:
		break;
	}
};

ark::input::on_key_change camera_mouse_key_event;
ark::input::on_input_change camera_camera_mouse_wheel_event;

ark::entity_view TestObject;
ark::entity_view TestObject2;

ark::stl::vector<ark::entity_view> circles;

void ingame::init()
{
	using namespace ark;
	using namespace entities;
	
	TestObject = add_phys_body(create(), {}, { 50, 50 }, { 20, 10 });
	TestObject2 = add_phys_body(create(), {}, { 350, 100 }, { 200, 10 });

	add_field<drawable_flag>(TestObject);
	add_field<drawable_flag>(TestObject2);
	
	std::random_device r_device;
	std::mt19937 gen(r_device());
	for (size_t i = 0; i < 400; i++) {
		std::uniform_real_distribution width_dist(260., 800.);
		std::uniform_real_distribution height_dist(260., 800.);
		auto& ent = circles.emplace_back(
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

	camera_mouse_key_event = ark::input::subscribe_key_event(camera_mouse_key_change);
	camera_camera_mouse_wheel_event = ark::input::subscribe_input_event(camera_mouse_wheel_change);
}

void ingame::destroy()
{
	ark::input::unsubscribe_input_event(camera_camera_mouse_wheel_event);
	ark::input::unsubscribe_key_event(camera_mouse_key_event);
}
