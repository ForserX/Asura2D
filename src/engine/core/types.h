#pragma once

void* ark_alloc(size_t size_to_alloc);
void ark_free(void* ptr);

namespace ark::stl
{
#if 0
	template <class T>
	struct ark_allocator
	{
		typedef T value_type;

		ark_allocator() = default;
		
		template <class U>
		constexpr ark_allocator(const ark_allocator <U>&) noexcept {}

		[[nodiscard]] T* allocate(std::size_t n) 
		{
			if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
				return nullptr;
			}

			if (auto p = static_cast<T*>(ark_alloc(n * sizeof(T)))) {
				return p;
			}

			return nullptr;
		}

		void deallocate(T* p, std::size_t n) noexcept
		{
			ark_free(p);
		}
	};
#else
    template <class T>
    using ark_allocator = std::allocator<T>;
#endif
	
#if 0
	template<typename K, typename T, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
	using hash_map = std::unordered_map<K, T, Hash, Equal, ark_allocator<std::pair<const K, T>>>;
	
	template<typename K, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
	using hash_set = std::unordered_set<K, Hash, Equal, ark_allocator<K>>;
#else
    template<typename K, typename T, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
    using hash_map = entt::dense_map<K, T, Hash, Equal, ark_allocator<std::pair<const K, T>>>;

    template<typename K, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
    using hash_set = entt::dense_set<K, Hash, Equal, ark_allocator<K>>;
#endif

	template<typename T>
	using function_set = hash_set<T, function_hasher<T>, function_equal<T>>;
	
	template<typename K>
	using vector = std::vector<K, ark_allocator<K>>;

	using byte_vector = vector<char>;
	using stream_vector = std::pair<int64_t, byte_vector>;

	template<class... Args>
	using variant = std::variant<Args...>;

	template<class... Args>
	using tuple = std::tuple<Args...>;

	using string = std::basic_string<char>;
	using string_view = std::basic_string_view<char>;

	using string_map = stl::hash_map<stl::string, stl::string>;
	using tree_string_map = stl::hash_map<stl::string, string_map>;
}
