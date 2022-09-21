#include "arkane.h"
#include <fstream>
#include <set>

using namespace ark;

static std::filesystem::path working_dir;
static std::filesystem::path content_dir;
static std::filesystem::path userdata_dir;

static std::set<std::filesystem::path> file_list;

const std::filesystem::path& 
filesystem::get_working_dir()
{
	return working_dir;
}

void 
filesystem::init()
{
	working_dir = std::filesystem::current_path();
	content_dir = std::filesystem::current_path().append("content");
	userdata_dir = std::filesystem::current_path().append("userdata");

	for (const std::filesystem::directory_entry& dir : std::filesystem::recursive_directory_iterator{ working_dir }) {
		file_list.emplace(dir.path());
	}

	if (!std::filesystem::exists(userdata_dir)) {
		create_dir(userdata_dir);
	}

}

void
filesystem::destroy()
{
	file_list.clear();
	userdata_dir.clear();
	content_dir.clear();
	working_dir.clear();
}

const std::filesystem::path& 
filesystem::get_content_dir()
{
	return working_dir;
}

const std::filesystem::path& 
filesystem::get_userdata_dir()
{
	return userdata_dir;
}

void
filesystem::create_file(const std::filesystem::path& file_name)
{
	const auto file_iter = file_list.find(file_name);
	if (file_iter != file_list.end()) {
		std::filesystem::remove(file_name);
	} else {
		file_list.emplace(file_name);
	}

	std::ofstream outfile(file_name);
	outfile.close();
}

void
filesystem::create_dir(const std::filesystem::path& dir_name)
{
	const auto file_iter = file_list.find(dir_name);
	if (file_iter != file_list.end()) {
		return;
	}

	file_list.emplace(dir_name);
	
	const bool can_create = std::filesystem::create_directories(dir_name);
	ark_assert(can_create, "file creating error", std::terminate());
}
