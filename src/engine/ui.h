#pragma once

namespace ark::ui
{
	void init();
	void tick(float dt);
	void destroy();

	void push_console_string(std::string_view str);
}
