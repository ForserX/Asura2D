#pragma once

namespace Asura
{
    class CfgParser
    {
        using data_type = stl::tree_string_map;
        data_type data;
        size_t section_count = 0;

    private:
        const stl::string& Get(stl::string_view section, stl::string_view value) const;

    public:
        CfgParser() noexcept = default;
        ~CfgParser() = default;

        void Load(const std::filesystem::path& file);
        void Save(const std::filesystem::path& file) const;

        [[nodiscard]] size_t Count() const { return section_count; }
        [[nodiscard]] const data_type& Data() const { return data; }
        int SetValue(stl::string_view section, stl::string_view key, stl::string_view value);

        int AddSection(stl::string_view section);
        int PushKey(stl::string_view section, stl::string_view key);

        inline void Swap(data_type& data_to_swap)
        {
            data.swap(data_to_swap);
        }

        template<typename T>
        T Get(stl::string_view section, stl::string_view value)
        {
            return stl::unstringify<T>(Get(section, value));
        }
    };
}
