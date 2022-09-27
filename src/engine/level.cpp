#include "pch.h"

using namespace ark;
std::filesystem::path path_level;
std::vector<ark::entity_view> ent_list;
#undef max
#undef min
namespace ark::level
{
	logic_parser level_data;

	void load(std::filesystem::path path) {
		level_data.load(path);

		float level_x = 0;
		float level_y = 0;
		float level_w = 0;
		float level_h = 0;

		physics::body_type body_type = physics::body_type::static_body;

		for (const auto &[section, kv] : level_data.get_data()) {
			
			level_x = std::stof(level_data.get_value(section, "x"));
			level_y = std::stof(level_data.get_value(section, "y"));
			level_w = std::stof(level_data.get_value(section, "w"));
			level_h = std::stof(level_data.get_value(section, "h"));

			std::string type_str = level_data.get_value(section, "type");
			if (type_str == "dynamic") {
				body_type = physics::body_type::dynamic_body;
			} 
			else if (type_str == "circle") {
				body_type = physics::body_type::around_body;
			}

			level_w /= 2;
			level_h /= 2;

			level_x = std::max(level_w, level_x) - std::min(level_w, level_x);
			level_y = std::max(level_h, level_y) - std::min(level_h, level_y);

			ent_list.emplace_back(
				entities::create_phys_body(true, { level_x, level_y }, { level_w, level_h }, body_type)
			);
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
