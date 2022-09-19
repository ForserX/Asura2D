#pragma once
#include <cstdint>
#include <string_view>

namespace ark::core {

void init();
void destroy();

int64_t get_cmd_int(std::string_view key);
std::string_view get_cmd_string(std::string_view key);
	
}
