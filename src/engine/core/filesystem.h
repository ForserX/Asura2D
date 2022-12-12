#pragma once

namespace Asura::FileSystem
{
	void Init();
	void Destroy();

	const std::filesystem::path& WorkingDir();
	const std::filesystem::path& ContentDir();
	const std::filesystem::path& UserdataDir();

	void create_file(const std::filesystem::path& file_name);
	void create_dir(const std::filesystem::path& dir_name);

	void write_file(const std::filesystem::path& file_name, stl::stream_vector& stream_data);
	void read_file(const std::filesystem::path& file_name, stl::stream_vector& stream_data);
}