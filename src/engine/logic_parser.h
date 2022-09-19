#pragma once
#include <unordered_map>
#include <filesystem>

namespace ark {
    class logic_parser
    {
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;

    public:
        logic_parser() noexcept = default;
        ~logic_parser() = default;

        void load(std::filesystem::path);
        std::string get(std::string_view section, std::string_view value);
    };
}
