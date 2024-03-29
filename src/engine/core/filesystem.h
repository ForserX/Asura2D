﻿#pragma once

#undef CreateFile

namespace Asura::FileSystem
{
	using Path = std::filesystem::path;

	void Init();
	void Destroy();

	const Path& WorkingDir();
	const Path& ContentDir();
	const Path& UserdataDir();

	void CreateFile(const Path& file_name);
	void CreateDir(const Path& dir_name);

	void write_file(const Path& file_name, stl::stream_vector& stream_data);
	void read_file(const Path& file_name, stl::stream_vector& stream_data);
}

namespace Asura
{
	inline stl::string ExtractPath(const FileSystem::Path& Path)
	{
		return stl::string(Path.generic_string());
	};
}

namespace Asura::FileSystem::Platform
{
	void NormalizePath(stl::string& Path);
	void NormalizePath(FileSystem::Path& Path);
}