#pragma once
#include <FileSystem>

namespace Asura::FileSystem
{
	void Init();
	void Destroy();

	const std::filesystem::path& get_working_dir();
	const std::filesystem::path& get_content_dir();
	const std::filesystem::path& get_userdata_dir();

	void create_file(const std::filesystem::path& file_name);
	void create_dir(const std::filesystem::path& dir_name);

	void write_file(const std::filesystem::path& file_name, stl::stream_vector& stream_data);
	void read_file(const std::filesystem::path& file_name, stl::stream_vector& stream_data);
}