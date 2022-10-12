#pragma once

void* ark_alloc(size_t size_to_alloc);
void ark_free(void* ptr);

namespace ark::stl
{
	template <class T>
	using clear_type = std::remove_cv_t<std::remove_reference_t<T>>;
	
	namespace internal 
	{
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args>
		struct detector 
		{
			using value_t = std::false_type;
			using type = Default;
		};
 
		template <class Default, template<class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> 
		{
			using value_t = std::true_type;
			using type = Op<Args...>;
		};
	} 
	
	struct nonesuch{};
 
	template <template<class...> class Op, class... Args>
	using is_detected = typename internal::detector<nonesuch, void, Op, Args...>::value_t;
 
	template <template<class...> class Op, class... Args>
	using detected_t = typename internal::detector<nonesuch, void, Op, Args...>::type;

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
	
	template <class T>
	void hash_combine(std::int64_t& s, const T& v)
	{
		std::hash<T> h;
		s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
	}

	template<typename T>
	struct function_equal
	{
		using callback_type = T;
			
		constexpr bool operator()(const callback_type& left, const callback_type& right) const
		{
			return left.get_id() == right.get_id();
		}
	};

	template<typename T>
	struct function_hasher
	{
		using callback_type = T;
			
		std::int64_t operator()(const callback_type& s) const
		{
			std::int64_t res = 0;
			hash_combine(res, s.get_id());
			return res;
		}
	};
	
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

	template<typename T>
	void
	write_memory(stream_vector& data, T& value)
	{
		char* ptr = data.second.data();
		std::memcpy(reinterpret_cast<void*>(&ptr[data.first]), reinterpret_cast<const void*>(&value), sizeof(value));
		data.first += sizeof(value);
	}

	template<typename T>
	void
	read_memory(stream_vector& data, T& value)
	{
		const char* ptr = data.second.data();
		std::memcpy(reinterpret_cast<char*>(&value), &ptr[data.first], sizeof(T));
		data.first += sizeof(value);
	}

	template<typename T>
	void
	push_memory(stream_vector& data, T& value)
	{
		using U = stl::clear_type<T>;

		const size_t idx = data.second.size();
		data.second.resize(data.second.size() + sizeof(T));

		char* ptr = data.second.data();
		ptr = &ptr[idx];
		U* data_ptr = reinterpret_cast<U*>(ptr);
		*data_ptr = value;
	}

	inline
	void
	prealloc_memory(stream_vector& data, size_t memory_count)
	{
		const size_t idx = data.second.capacity();
		data.second.reserve(idx + memory_count);
	}

	using string = std::basic_string<char>;
	using string_view = std::basic_string_view<char>;

	template<class... Args>
	using variant = std::variant<Args...>;

	template<class... Args>
	using tuple = std::tuple<Args...>;

	using string_map = stl::hash_map<stl::string, stl::string>;
	using tree_string_map = stl::hash_map<stl::string, string_map>;
}
