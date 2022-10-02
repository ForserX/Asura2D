#pragma once

namespace ark
{
    class logic_parser
    {
        using data_type = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
        data_type data;
        size_t section_count = 0;

    private:
        std::string get(std::string_view section, std::string_view value);

    public:
        logic_parser() noexcept = default;
        ~logic_parser() = default;

        void load(const std::filesystem::path&);
        void save(const std::filesystem::path&);

        [[nodiscard]] inline size_t get_count() const { return section_count; }
        [[nodiscard]] inline data_type get_data() const { return data; }
        int set_value(std::string_view section, std::string_view key, std::string_view value);

        int add_section(std::string_view section);
        int add_key_in_section(std::string_view section, std::string_view key);

        template <typename T>
        auto get_value(std::string_view section, std::string_view value) {
            if constexpr (std::is_same_v<T, bool>) {
                return "true" == get(section, value);
            }
            else if constexpr (std::is_integral_v<T>) {
                return (T)std::stoll(get(section, value));
            }
            else if constexpr (std::is_floating_point_v<T>) {
                return std::stof(get(section, value));
            }
            else {
                return get(section, value);
            }
        }
    };
}
