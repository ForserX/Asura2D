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
	inline ark_float_vec2() : b2Vec2() {};

	inline ark_float_vec2(float fx, float fy)
	{
		x = fx;
		y = fy;
	}

	inline ark_float_vec2(ImVec2 vec)
	{
		x = vec.x;
		y = vec.y;
	}
	inline ark_float_vec2(b2Vec2 vec)
	{
		x = vec.x;
		y = vec.y;
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

	template<typename T>
	using function_set = std::unordered_set<T, function_hasher<T>, function_equal<T>>;

	template<typename K, typename T>
	using hash_map = std::unordered_map<K, T, std::hash<K>, std::equal_to<K>, ark_allocator<std::pair<const K, T>>>;
	
	template<typename K>
	using hash_set = std::unordered_set<K, std::hash<K>, std::equal_to<K>, ark_allocator<K>>;
	
	template<typename K>
	using vector = std::vector<K, ark_allocator<K>>;
	//template<typename K, typename T>
	//using hash_map = std::unordered_map<K, T, std::hash<K>, std::equal_to<K>, 
}