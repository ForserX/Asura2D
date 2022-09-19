#include "logic_parser.h"

#include <string>
#include <string_view>
#include <fstream>

#define assert(code, error_code)

using ark::logic_parser;

void
logic_parser::load(std::filesystem::path file_path)
{
    std::ifstream file(file_path);

    std::string line = "";
    std::string last_section_name = "";

    assert(file.is_open(), "File not found");

    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char x) {return std::isspace(x);}), line.end());

        // Skip empty lines
        if (line.length() < 2) {
            continue;
        }

        if (line[0] == '[') {
            last_section_name = std::move(line.substr(1, line.find(']') - 1));
            continue;
        }
        else {
            size_t finded_equal_char = line.find('=');
            data[last_section_name][line.substr(0, finded_equal_char)] = std::move(line.substr(finded_equal_char + 1));
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

    assert(false, "Not found")
}