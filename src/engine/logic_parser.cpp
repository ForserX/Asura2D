#include "pch.h"

using ark::logic_parser;

void
logic_parser::load(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path);
	ark_assert(file.is_open(), "File not found", return);

    std::string line;
    std::string last_section_name;

   
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

            section_count++;
        } else {
            const size_t found_equal_char = line.find('=');
            data[last_section_name][line.substr(0, found_equal_char)] = line.substr(found_equal_char + 1);
        }
    }

    file.close();
}
void 
logic_parser::save(const std::filesystem::path& file_path)
{
	std::ofstream file(file_path, std::ios_base::out);
    
	ark_assert(file.is_open(), "No access to file", return);

    for (auto &fIter : data)
    {
        std::string section_name = fIter.first;

        file << "[" + section_name + "]" << std::endl;
        for (auto &sIter : fIter.second)
        {
            file << sIter.first + " = " + sIter.second << std::endl;
        }

    } 

    file.flush();
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

int
logic_parser::set_value(std::string_view section, std::string_view key, std::string_view value)
{
    ark_assert(section.empty(), "section can't be null", return -1);
    ark_assert(key.empty(), "key can't be null", return -2);

    bool section_was_founded = false;
    for (auto& fIter : data)
    {       
        size_t section_founded = fIter.first.compare(section);
        if (section_founded != -1)
        {
            for (auto& sIter : fIter.second)
            {  
               if (sIter.first.compare(key) != -1)
                {
                   sIter.second = value;
                }
            }
            section_was_founded = true;
            break;
        }        
    }
    
    return 0;
}

int 
logic_parser::add_section(std::string_view section)
{
   
    for (auto& fIter : data)
    {       
        ark_assert(fIter.first != section, "section already created", return -1)
    }

    data[section.data()]["temp"] = "temp";
    
    return 0;
}

int 
logic_parser::add_key_in_section(std::string_view section, std::string_view key)
{

	for (auto& fIter : data)
	{
		for (auto & sIter : fIter.second)
		{
            ark_assert(sIter.first != key, "key in section already created", return -1)
            
		}
	}

	data[section.data()][key.data()] = "temp";


    return 0;
}
