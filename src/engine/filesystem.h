#pragma once
#include <filesystem>

namespace ark::filesystem
{
	void init();
	void destroy();

	const std::filesystem::path& get_working_dir();
	const std::filesystem::path& get_content_dir();
	const std::filesystem::path& get_userdata_dir();

	void create_file(const std::filesystem::path& file_name);
	void create_dir(const std::filesystem::path& dir_name);
}