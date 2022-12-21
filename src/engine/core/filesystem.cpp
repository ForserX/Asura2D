#include "pch.h"

using namespace Asura;

static FileSystem::Path working_dir = {};
static FileSystem::Path content_dir = {};
static FileSystem::Path userdata_dir = {};

static stl::hash_set<std::string> file_list = {};

const FileSystem::Path& FileSystem::WorkingDir()
{
	return working_dir;
}

void FileSystem::Init()
{
	auto CurPath = std::filesystem::current_path();

	auto Check = [&CurPath]()
	{
		if (!std::filesystem::exists(CurPath.generic_string() + ("/content")))
		{
			CurPath = CurPath.parent_path();
			return false;
		}
		else
		{
			return true;
		}
	};

	for (int It = 0; It < 5; It++)
	{
		if (Check())
			break;
	}

	working_dir = CurPath;
	content_dir = CurPath;
	userdata_dir = CurPath;

	content_dir = content_dir / ("content");
	userdata_dir = userdata_dir / ("userdata");

	for (const std::filesystem::directory_entry& dir : std::filesystem::recursive_directory_iterator{ working_dir }) 
	{
		file_list.emplace(dir.path().generic_string());
	}

	if (!std::filesystem::exists(userdata_dir)) 
	{
		CreateDir(userdata_dir);
	}

}

void FileSystem::Destroy()
{
	file_list.clear();
	userdata_dir.clear();
	content_dir.clear();
	working_dir.clear();
}

const FileSystem::Path& FileSystem::ContentDir()
{
	return content_dir;
}

const FileSystem::Path& FileSystem::UserdataDir()
{
	return userdata_dir;
}

void FileSystem::CreateFile(const FileSystem::Path& file_name)
{
	const auto file_iter = file_list.find(file_name.generic_string());
	if (file_iter != file_list.end()) {
		std::filesystem::remove(file_name);
	} else {
		file_list.emplace(file_name.generic_string());
	}

	std::ofstream outfile(file_name);
	outfile.close();
}

void FileSystem::CreateDir(const FileSystem::Path& dir_name)
{
	const auto file_iter = file_list.find(dir_name.generic_string());
	if (file_iter != file_list.end()) 
	{
		return;
	}

	file_list.emplace(dir_name.generic_string());
	
	const bool can_create = std::filesystem::create_directories(dir_name);
	game_assert(can_create, "file creating error", std::terminate());
}

void FileSystem::write_file(const FileSystem::Path& file_name, stl::stream_vector& stream_data)
{
	const auto file_iter = file_list.find(file_name.generic_string());
	if (file_iter != file_list.end()) {
		std::filesystem::remove(file_name);
	} 
	else 
	{
		file_list.emplace(file_name.generic_string());
	}

	std::fstream out_stream(file_name, std::fstream::binary | std::fstream::trunc | std::fstream::out);
	out_stream.write(stream_data.second.data(), stream_data.second.size());
	out_stream.close();
}

void FileSystem::read_file(const FileSystem::Path& file_name, stl::stream_vector& stream_data)
{
	std::fstream in_stream(file_name, std::fstream::binary | std::fstream::in);
	stream_data.second.resize(std::filesystem::file_size(file_name));
	in_stream.read(stream_data.second.data(), stream_data.second.size());
	in_stream.close();
}
