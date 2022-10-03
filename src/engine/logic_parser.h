#pragma once

namespace ark
{
    class logic_parser
    {
        using data_type = stl::hash_map<stl::string, stl::hash_map<stl::string, stl::string>>;
        data_type data;
        size_t section_count = 0;

    private:
        const stl::string& get(stl::string_view section, stl::string_view value) const;

    public:
        logic_parser() noexcept = default;
        ~logic_parser() = default;

        void load(const std::filesystem::path&);
        void save(const std::filesystem::path&) const;

        [[nodiscard]] size_t get_count() const { return section_count; }
        [[nodiscard]] data_type get_data() const { return data; }
        int set_value(stl::string_view section, stl::string_view key, stl::string_view value);

        int add_section(stl::string_view section);
        int add_key_in_section(stl::string_view section, stl::string_view key);

        template<typename T>
        T get(stl::string_view section, stl::string_view value)
        {
            const auto& value_string = get(section, value);
            if constexpr (std::is_floating_point_v<T>) {
                return static_cast<T>(std::stod(value_string));
            } else if constexpr (std::is_same_v<T, bool>) {
                return value_string == "true";
            } else if constexpr (std::is_integral_v<T>) {
                if constexpr (std::is_unsigned_v<T>) {
                    return static_cast<T>(std::stoull(value_string));
                } else {
                    return static_cast<T>(std::stoll(value_string));
                }
            } else if constexpr (std::is_enum_v<T>) {
                return static_cast<T>(std::stoll(value_string));
            } else {
                return value_string.data();
            }
        }
    };
}
