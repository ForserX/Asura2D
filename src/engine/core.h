#pragma once

namespace ark::core
{
	void init();
	void destroy();

	int64_t get_cmd_int(stl::string_view key);
	stl::string_view get_cmd_string(stl::string_view key);
}
