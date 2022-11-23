#pragma once

namespace ark
{
    class config_parser
    {
        using data_type = stl::tree_string_map;
        data_type data;
        size_t section_count = 0;

    private:
        const stl::string& get(stl::string_view section, stl::string_view value) const;

    public:
        config_parser(const data_type& in_data) : data(in_data), section_count(in_data.size()) {}
        config_parser() noexcept = default;
        ~config_parser() = default;

        void load(const std::filesystem::path& file);
        void save(const std::filesystem::path& file) const;

        [[nodiscard]] size_t get_count() const { return section_count; }
        [[nodiscard]] const data_type& get_data() const { return data; }
        int set_value(stl::string_view section, stl::string_view key, stl::string_view value);

        int add_section(stl::string_view section);
        int add_key_in_section(stl::string_view section, stl::string_view key);

        template<typename T>
        T get(stl::string_view section, stl::string_view value)
        {
            return stl::unstringify<T>(get(section, value));
        }
    };
}
