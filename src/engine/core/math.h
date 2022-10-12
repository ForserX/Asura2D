#pragma once

namespace ark::math
{
    template<typename T>
    struct mtx_2x2
    {
        T x1 = {};
        T x2 = {};
        T y1 = {};
        T y2 = {};
    };

    using fmtx_2x2 = mtx_2x2<float>;

    template<typename T>
    struct vec2
    {
        T x;
        T y;

        vec2() = default;
        vec2(auto dx, auto dy) : x(dx), y(dy) {}
        vec2(const b2Vec2& vec)
        {
            x = static_cast<T>(vec.x);
            y = static_cast<T>(vec.y);
        }
        
        vec2(const ImVec2& vec)
        {
            x = static_cast<T>(vec.x);
            y = static_cast<T>(vec.y);
        }

        float operator () (int32 i) const
        {
            return (&x)[i];
        }
        
        void operator -= (const vec2<T>& v)
        {
            x -= v.x;
            y -= v.y;
        }

        void operator += (const vec2<T>& v)
        {
            x += v.x;
            y += v.y;
        }

        void operator *= (const vec2<T>& v)
        {
            x *= v.x;
            y *= v.y;
        }

        void operator /= (const vec2<T>& v)
        {
            x /= v.x;
            y /= v.y;
        }
        
        void operator -= (const T v)
        {
            x -= v;
            y -= v;
        }

        void operator += (const T v)
        {
            x += v;
            y += v;
        }

        void operator *= (const T v)
        {
            x *= v;
            y *= v;
        }

        void operator /= (const T v)
        {
            x /= v;
            y /= v;
        }
        
        vec2<T> operator<=>(const vec2<T>&) const = default;
        
        operator ImVec2()
        {
            return ImVec2(static_cast<float>(x), static_cast<float>(y));
        }
        
        operator b2Vec2()
        {
            return b2Vec2(static_cast<float>(x), static_cast<float>(y));
        }

        bool empty() const
        {
            return (x == static_cast<T>(0) && y == static_cast<T>(0));
        }
        
        static vec2<T> min(const vec2<T>& first, const vec2<T>& second)
        {
            return vec2<T>(std::min(first.x, second.x), std::min(first.y, second.y));
        }
        
        static vec2<T> max(const vec2<T>& first, const vec2<T>& second)
        {
            return vec2<T>(std::max(first.x, second.x), std::max(first.y, second.y));
        }
    };

    template<typename T>
    vec2<T> operator+(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x + b.x, a.y + b.y);
    }

    template<typename T>
    vec2<T> operator-(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x - b.x, a.y - b.y);
    }

    template<typename T>
    vec2<T> operator*(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x * b.x, a.y * b.y);
    }

    template<typename T>
    vec2<T> operator/(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x / b.x, a.y / b.y);
    }

    template<typename T>
    bool operator==(const vec2<T>& a, const vec2<T>& b)
    {
        return a.x == b.x && a.y == b.y;
    }

    template<typename T>
    bool operator!=(const vec2<T>& a, const vec2<T>& b)
    {
        return a.x != b.x || a.y != b.y;
    }

    using ivec2 = vec2<int16_t>;
    using fvec2 = vec2<float>;
    
    template<typename T>
    struct rect
    {
        T values[4] = {};
        
        rect() = default;
        rect(vec2<T> in_min, vec2<T> in_max)
        {
            values[0] = in_min.x;
            values[1] = in_min.y;
            values[2] = in_max.x;
            values[3] = in_max.y;
        }
        
        rect(auto x, auto y, auto w, auto h)
        {
            values[0] = static_cast<T>(x);
            values[1] = static_cast<T>(y);
            values[2] = static_cast<T>(x + w);
            values[3] = static_cast<T>(y + h);
        }
        
        T min_x() const
        {
            return values[0];
        }
        
        T min_y() const
        {
            return values[1];
        }
        
        T max_x() const
        {
            return values[2];
        }
        
        T max_y() const
        {
            return values[3];
        }
        
        vec2<T> min() const
        {
            return { min_x(), min_y() };
        }
        
        vec2<T> max() const
        {
            return { max_x(), max_y() };
        }
        
        T width() const
        {
            return max_x() - min_x();
        }
        
        T height() const
        {
            return max_y() - min_y();
        }
    };

    using frect = rect<float>;
    using irect = rect<int32_t>;

    class transform
    {
    private:
        fvec2 pos = {};
        fvec2 rot = {};        // sin - x, cos - y
        
    public:
        transform() = default;
        transform(fvec2 in_pos, fvec2 in_rot)
        {
            pos = in_pos;
            rot = in_rot;
        }

        float angle() const
        {
            float acos = std::acosf(rot.y);
            float angle = (rot.x >= 0) ? acos : -acos;
            return angle;
        }
        
        const fvec2& rotation() const
        {
            return rot;
        }
        
        const fvec2& position() const
        {
            return pos;
        }
        
        void set_angle(float angle)
        {
            rot.x = std::sinf(angle);
            rot.y = std::cosf(angle);
        }
        
        void set_rotation(const fvec2& new_rot)
        {
            rot = new_rot;
        }
        
        void set_position(const fvec2& new_pos)
        {
            pos = new_pos;
        }
    };

    inline float to_degrees(float rad)
    {
        return (rad / M_PI) * 360.f;
    }

    inline float to_radians(float degrees)
    {
        return (degrees / 360.f) * M_PI;
    }

    /*
        void rotate_rect(mtx& rect, const fvec2& center, float angle)
        {
            float x1 = rect.x - center.x;
            float y1 = rect.y - center.y;
            
            
        }
     */
}
