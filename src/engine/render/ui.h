#pragma once

namespace Asura::ui
{
	void Init();
	void Tick(float dt);
	void Destroy();

	int64_t GetCmdInt(stl::string_view key);
	void push_console_string(stl::string_view str);
}
