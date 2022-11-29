#pragma once

namespace asura::ui
{
	void init();
	void tick(float dt);
	void destroy();

	int64_t get_cmd_int(stl::string_view key);
	void push_console_string(stl::string_view str);
}
