#pragma once

namespace ark::ui
{
	void init();
	void tick(float dt);
	void destroy();

	int64_t get_cmd_int(std::string_view key);
	void push_console_string(std::string_view str);
}
