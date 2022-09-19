#include "pch.h"

using ark::logic_parser;

void
logic_parser::load(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path);

    std::string line;
    std::string last_section_name;

    ark_assert(file.is_open(), "File not found", return);

    while (std::getline(file, line)) {
        std::erase_if(line, [](unsigned char x) {return std::isspace(x);});

        const size_t comment_start = line.find(';');
        if (comment_start != std::string::npos) {
            line = line.substr(0, comment_start);
        }

        // Skip empty lines
        if (line.length() < 2) {
            continue;
        }

        if (line[0] == '[') {
            last_section_name = line.substr(1, line.find(']') - 1);

            // Parse parrent section
            const size_t parent_start = line.find(':');
            if (parent_start != std::string::npos) {
                for (auto &[data_key, data_value] : data[line.substr(parent_start + 1)]) {
                    data[last_section_name][data_key] = data_value;
                }
            }
        } else {
            const size_t found_equal_char = line.find('=');
            data[last_section_name][line.substr(0, found_equal_char)] = line.substr(found_equal_char + 1);
        }
    }

    file.close();
}

std::string 
logic_parser::get(std::string_view section, std::string_view key)
{
    auto current_data = data[section.data()];
    for (auto& [lkey, lvalue] : current_data)
    {
        if (lkey == key)
        {
            return lvalue;
        }
    }

    ark_assert(false, "Not found", return "");
}