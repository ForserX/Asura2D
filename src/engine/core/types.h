#pragma once

void* ark_alloc(size_t size_to_alloc);
void ark_free(void* ptr);

namespace ark::stl
{
#if 1
	template <typename T>
	struct ark_allocator
	{
		using value_type = T;

		constexpr ark_allocator() = default;

		template <class Other>
		constexpr ark_allocator(const ark_allocator<Other>&) noexcept {}

		template <class Other>
		ark_allocator& operator=(const ark_allocator<Other>&) {
			return *this;
		}

		T* allocate(std::size_t n, const void* hint = nullptr)
		{
			
			if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
				return nullptr;
			}

			if (auto p = static_cast<T*>(ark_alloc(n * sizeof(T)))) {
				return p;
			}

			return nullptr;
		}

		[[nodiscard]] constexpr T* allocate(std::size_t n) const
		{
			if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
				return nullptr;
			}

			if (auto p = static_cast<T*>(ark_alloc(n * sizeof(T)))) {
				return p;
			}

			return nullptr;
		}

		void deallocate(T* p, std::size_t n) const noexcept
		{
			ark_free(p);
		}

		void deallocate(void* p, std::size_t n) const noexcept
		{
			ark_free(p);
		}

		void construct(T* p, const T& _Val) { new (p) T(_Val); }

		void construct(T* p, T&& _Val) { new (p) T(std::forward<T>(_Val)); }

		void destroy(T* p) { p->~T(); }
	};
#else
    template <typename T>
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

	using string = std::basic_string<char, std::char_traits<char>, ark_allocator<char>>;
	using string_view = std::basic_string_view<char>;


	inline string to_string(int Value)
	{
		char buf[64] = {0};

#ifdef _WIN32
		_itoa(Value, &buf[0], 10);
#else
		itoa(Value, &buf[0], 10);
#endif
		return string(buf);
	}

	inline string to_string(unsigned int Value)
	{
		char buf[64] = { 0 };
		sprintf(buf, "%u", Value);

		return string(buf);
	}

	inline string to_string(float Value)
	{
		char buf[64] = { 0 };
		sprintf(buf, "%f", Value);

		return string(buf);
	}

	inline string to_string(double Value)
	{
		char buf[64] = { 0 };
		sprintf(buf, "%f", Value);

		return string(buf);
	}

	[[nodiscard]] inline uint64 stoull(const string& str, size_t* idx = nullptr, int base = 10) {
		const char* str_ptr = str.c_str();
		char* end_ptr;
		const unsigned long long ans = std::strtoull(str_ptr, &end_ptr, base);

		if (idx) {
			*idx = static_cast<size_t>(end_ptr - str_ptr);
		}

		return ans;
	}

	[[nodiscard]] inline int64_t stoll(const string& str, size_t* idx = nullptr, int base = 10) {
		const char* str_ptr = str.c_str();
		char* end_ptr;
		const long long ans = std::strtoll(str_ptr, &end_ptr, base);

		if (idx) {
			*idx = static_cast<size_t>(end_ptr - str_ptr);
		}

		return ans;
	}

	[[nodiscard]] inline uint32 stoul(string str, size_t* idx = nullptr)
	{
		return uint32(stoull(str, idx));
	}

	[[nodiscard]] inline double stof(const string& str, size_t* idx = nullptr)
	{
		const char* str_ptr = str.c_str();
		char* end_ptr;

		const float ans = std::strtod(str_ptr, &end_ptr);

		if (idx) {
			*idx = static_cast<size_t>(end_ptr - str_ptr);
		}

		return ans;
	}

	[[nodiscard]] inline float stod(string str, size_t* idx = nullptr)
	{
		return float(stof(str, idx));
	}

	using string_map = stl::hash_map<stl::string, stl::string>;
	using tree_string_map = stl::hash_map<stl::string, string_map>;
}
