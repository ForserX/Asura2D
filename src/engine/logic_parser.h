#pragma once
#include <unordered_map>
#include <filesystem>

namespace ark
{
    class logic_parser
    {
        using data_type = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
        data_type data;
        size_t section_count = 0;

    public:
        logic_parser() noexcept = default;
        ~logic_parser() = default;

        void load(const std::filesystem::path&);
        void save(const std::filesystem::path&);

        [[nodiscard]] inline size_t get_count() const { return section_count; }
        [[nodiscard]] inline data_type get_data() const { return data; }
        std::string get_value(std::string_view section, std::string_view value);
        int set_value(std::string_view section, std::string_view key, std::string_view value);

        int add_section(std::string_view section);
        int add_key_in_section(std::string_view section, std::string_view key);
    };
}
