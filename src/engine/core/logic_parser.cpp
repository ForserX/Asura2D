#include "pch.h"

using namespace ark;

void
config_parser::load(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path);
	ark_assert(file.is_open(), "File not found", return);

    stl::string line;
    stl::string last_section_name;
    
    while (std::getline(file, line)) {
        std::erase_if(line, [](unsigned char x) {return std::isspace(x);});

        const size_t comment_start = line.find(';');
        if (comment_start != stl::string::npos) {
            line = line.substr(0, comment_start);
        }

        // Skip empty lines
        if (line.length() < 2) {
            continue;
        }

        if (line[0] == '[') {
            last_section_name = line.substr(1, line.find(']') - 1);

            // Parse parent section
            const size_t parent_start = line.find(':');
            if (parent_start != stl::string::npos) {
                for (const auto &[data_key, data_value] : data[line.substr(parent_start + 1)]) {
                    data[last_section_name][data_key] = data_value;
                }
            }

            section_count++;
        } else {
            const size_t found_equal_char = line.find('=');
            data[last_section_name][line.substr(0, found_equal_char)] = line.substr(found_equal_char + 1);
        }
    }

    file.close();
}
void 
config_parser::save(const std::filesystem::path& file_path) const
{
	std::ofstream file(file_path, std::ios_base::out);
    
	ark_assert(file.is_open(), "No access to file", return);

    for (const auto &fIter : data) {
        stl::string section_name = fIter.first;
        file << "[" + section_name + "]" << std::endl;
        for (const auto &sIter : fIter.second) {
            file << sIter.first + " = " + sIter.second << std::endl;
        }
    } 

    file.flush();
	file.close();
}

const stl::string&
config_parser::get(stl::string_view section, stl::string_view key) const
{
    static const stl::string empty_string;
    auto& current_data = data.at(section.data());
    for (const auto& [data_key, data_value] : current_data) {
        if (data_key == key) {
            return data_value;
        }
    }

    ark_assert(false, "Not found", return empty_string);
}

int
config_parser::set_value(stl::string_view section, stl::string_view key, stl::string_view value)
{
    ark_assert(section.empty(), "section can't be null", return -1);
    ark_assert(key.empty(), "key can't be null", return -2);

    for (const auto& [data_key, data_value] : data) {
        if (data_key == section) {
            for (const auto& [section_key, section_value] : data_value) {
                if (section_key == key) {
                    data_value[section_key] = value;
                }
            }
        }
    }
    
    return 0;
}

int 
config_parser::add_section(stl::string_view section)
{
    for (const auto& fIter : data) {
        ark_assert(fIter.first != section, "section already created", return -1)
    }

    data[section.data()]["temp"] = "temp";
    return 0;
}

int 
config_parser::add_key_in_section(stl::string_view section, stl::string_view key)
{
	for (const auto& fIter : data) {
		for (const auto & sIter : fIter.second) {
            ark_assert(sIter.first != key, "key in section already created", return -1)
		}
	}

	data[section.data()][key.data()] = "temp";
    return 0;
}
