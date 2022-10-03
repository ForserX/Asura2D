#pragma once

struct ark_matrix
{
	float x;
	float y;
	float h;
	float w;
};

template<typename T>
struct ark_vec2
{
	T x;
	T y;

	ark_vec2() = default;
	ark_vec2(T dx, T dy) : x(dx), y(dy) {}

	float operator () (int32 i) const
	{
		return (&x)[i];
	}
	
	void operator -= (const ark_vec2<T>& v)
	{
		x -= v.x;
		y -= v.y;
	}

	void operator += (const ark_vec2<T>& v)
	{
		x += v.x;
		y += v.y;
	}

	void operator *= (const ark_vec2<T>& v)
	{
		x *= v.x;
		y *= v.y;
	}

	void operator /= (const ark_vec2<T>& v)
	{
		x /= v.x;
		y /= v.y;
	}

	bool is_null()
	{
		return (x == static_cast<T>(0) && y == static_cast<T>(0));
	}
};

/// Add two vectors component-wise.
template<typename T>
ark_vec2<T> operator+(const ark_vec2<T>& a, const ark_vec2<T>& b)
{
	return ark_vec2<T>(a.x + b.x, a.y + b.y);
}

template<typename T>
ark_vec2<T> operator-(const ark_vec2<T>& a, const ark_vec2<T>& b)
{
	return ark_vec2<T>(a.x - b.x, a.y - b.y);
}

template<typename T>
ark_vec2<T> operator*(const ark_vec2<T>& a, const ark_vec2<T>& b)
{
	return ark_vec2<T>(a.x * b.x, a.y * b.y);
}

template<typename T>
ark_vec2<T> operator/(const ark_vec2<T>& a, const ark_vec2<T>& b)
{
	return ark_vec2<T>(a.x / b.x, a.y / b.y);
}

template<typename T>
bool operator==(const ark_vec2<T>& a, const ark_vec2<T>& b)
{
	return a.x == b.x && a.y == b.y;
}

template<typename T>
bool operator!=(const ark_vec2<T>& a, const ark_vec2<T>& b)
{
	return a.x != b.x || a.y != b.y;
}

class ark_float_vec2 : public b2Vec2
{
public:
	ark_float_vec2() : b2Vec2(0, 0) {};

	ark_float_vec2(float fx, float fy)
	{
		x = fx;
		y = fy;
	}

	ark_float_vec2(ImVec2 vec)
	{
		x = vec.x;
		y = vec.y;
	}
	
	ark_float_vec2(b2Vec2 vec)
	{
		x = vec.x;
		y = vec.y;
	}

	bool empty() const
	{
		return (x == 0.f && y == 0.f);
	}
	
	operator b2Vec2() const
	{
		return { x, y };
	}

	operator ImVec2() const
	{
		return { x, y };
	}
};

using ark_int_vec2 = ark_vec2<int16_t>;

void* ark_alloc(size_t size_to_alloc);
void ark_free(void* ptr);

namespace ark::stl
{
	template <class T>
	using clear_type = std::remove_cv_t<std::remove_reference_t<T>>;
	
	namespace internal {
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args>
		struct detector {
			using value_t = std::false_type;
			using type = Default;
		};
 
		template <class Default, template<class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
			using value_t = std::true_type;
			using type = Op<Args...>;
		};
 
	} // namespace detail
	
	struct nonesuch{};
 
	template <template<class...> class Op, class... Args>
	using is_detected = typename internal::detector<nonesuch, void, Op, Args...>::value_t;
 
	template <template<class...> class Op, class... Args>
	using detected_t = typename internal::detector<nonesuch, void, Op, Args...>::type;

	template <class T>
	struct ark_allocator
	{
		typedef T value_type;

		ark_allocator() = default;
		
		template <class U>
		constexpr ark_allocator(const ark_allocator <U>&) noexcept {}

		[[nodiscard]] T* allocate(std::size_t n) {
			if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
				return nullptr;
			}

			if (auto p = static_cast<T*>(ark_alloc(n * sizeof(T)))) {
				return p;
			}

			return nullptr;
		}

		void deallocate(T* p, std::size_t n) noexcept {
			ark_free(p);
		}
	};
	
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
	
	template<typename K, typename T, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
	using hash_map = std::unordered_map<K, T, Hash, Equal, ark_allocator<std::pair<const K, T>>>;
	
	template<typename K, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
	using hash_set = std::unordered_set<K, Hash, Equal, ark_allocator<K>>;

	template<typename T>
	using function_set = hash_set<T, function_hasher<T>, function_equal<T>>;
	
	template<typename K>
	using vector = std::vector<K, ark_allocator<K>>;
	
	using byte_vector = vector<char>;
	using stream_vector = std::pair<int64_t, byte_vector>;

	void
	write_memory(stream_vector& data, auto& value)
	{
		const size_t idx = data.second.size();
		data.second.resize(idx + sizeof(value));
		std::memcpy(&data.second[idx], &value, sizeof(value));
		data.first += sizeof(value);
	}

	void
	read_memory(stream_vector& data, auto& value)
	{
		
	}

	using string = std::basic_string<char>;
	using string_view = std::basic_string_view<char>;

	template<class... Args>
	using variant = std::variant<Args...>;
}