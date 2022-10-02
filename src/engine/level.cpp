#include "pch.h"

using namespace ark;
std::filesystem::path path_level;
stl::vector<entity_view> ent_list;
#undef max
#undef min
namespace ark::level
{
	logic_parser level_data;

	void load(const std::filesystem::path path)
	{
		level_data.load(path);

		float level_x = 0;
		float level_y = 0;
		float level_w = 0;
		float level_h = 0;

		physics::body_type body_type = {};
		physics::body_shape body_shape = {};
		material::type material_type = {};

		// сука, это нужно будет переделать
		for (const auto &[section, kv] : level_data.get_data()) {
			if (section == "background") {
				auto landscape_path = filesystem::get_content_dir();
				landscape_path.append("textures").append(level_data.get_value<std::string>(section, "path"));

				auto& background_ent = ent_list.emplace_back(entities::add_texture(entities::create(), landscape_path.generic_string().c_str()));
				entities::add_field<entities::background_flag>(background_ent);
				continue;
			}
			
			const bool is_drawable = level_data.get_value<bool>(section, "drawable");
			level_x = level_data.get_value<float>(section, "x");
			level_y = level_data.get_value<float>(section, "y");
			level_w = level_data.get_value<float>(section, "w");
			level_h = level_data.get_value<float>(section, "h");

			std::string type_str = level_data.get_value<std::string>(section, "type");
			if (type_str == "dynamic") {
				body_type = physics::body_type::dynamic_body;
			} else if (type_str == "static") {
				body_type = physics::body_type::static_body;
			} else {
				ark_assert(false, "this is bad, bro", {})
			}

			std::string shape_str = level_data.get_value<std::string>(section, "shape");
			if (shape_str == "box") {
				body_shape = physics::body_shape::box_shape;
			} else if (shape_str == "circle") {
				body_shape = physics::body_shape::circle_shape;
			} else {
				ark_assert(false, "this is bad, bro", {})
			}
			
			std::string material_str = level_data.get_value<std::string>(section, "material");
			if (material_str == "solid") {
				material_type = material::type::solid;
			} else if (material_str == "rubber") {
				material_type = material::type::rubber;
			} else {
				ark_assert(false, "this is bad, bro", {})
			}
			
			level_w /= 2;
			level_h /= 2;

			level_x = std::max(level_w, level_x) - std::min(level_w, level_x);
			level_y = std::max(level_h, level_y) - std::min(level_h, level_y);

			auto ent = ent_list.emplace_back(
				entities::add_phys_body(
					entities::create(),
					{},
					{ level_x, level_y },
					{ level_w, level_h },
					body_type,
					body_shape,
					material_type
				)
			);

			if (is_drawable) {
				entities::add_field<entities::drawable_flag>(ent);
			}
		}
	};
};

void level::init()
{
	path_level = filesystem::get_content_dir();
	path_level.append("level");

	ark_assert(std::filesystem::exists(path_level), "Level path not found! Please, reinstall game!", return);

	for (auto& dir_iter : std::filesystem::directory_iterator{ path_level }) {
		load(dir_iter);
	}
}

void level::tick(float dt)
{
}

void level::destroy()
{
}
