#include "logic_parser.h"
#include "debug_core.h"

#include <string>
#include <string_view>
#include <fstream>

using ark::logic_parser;

void
logic_parser::load(std::filesystem::path file_path)
{
    std::ifstream file(file_path);

    std::string line = "";
    std::string last_section_name = "";

    assert(file.is_open(), "File not found", return);

    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char x) {return std::isspace(x);}), line.end());

        size_t comment_start = line.find(';');

        if (comment_start != std::string::npos) {
            line = line.substr(0, comment_start);
        }

        // Skip empty lines
        if (line.length() < 2) {
            continue;
        }

        if (line[0] == '[') {
            last_section_name = std::move(line.substr(1, line.find(']') - 1));

            // Parse parrent section
            size_t parrent_start = line.find(':');
            if (parrent_start != std::string::npos) {
                for (auto &[lkey, lvalue] : data[line.substr(parrent_start + 1)]) {
                    data[last_section_name][lkey] = lvalue;
                }
            }

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

    assert(false, "Not found", return "");
}