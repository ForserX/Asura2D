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