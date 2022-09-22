#pragma once
#include <unordered_map>
#include <filesystem>

namespace ark
{
    class logic_parser
    {
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;

    public:
        logic_parser() noexcept = default;
        ~logic_parser() = default;

        void load(const std::filesystem::path&);
        void save(const std::filesystem::path&);

        std::string get_value(std::string_view section, std::string_view value);
        int set_value(std::string_view section, std::string_view key, std::string_view value);

        int add_section(std::string_view section);
        int add_key_in_section(std::string_view section, std::string_view key);
    };
}
