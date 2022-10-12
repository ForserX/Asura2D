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
    private:
        T data[2] = {};

    public:
        vec2() = default;
        vec2(auto dx, auto dy)
        {
            data[0] = dx;
            data[1] = dy;
        }
        
        vec2(const b2Vec2& vec)
        {
            data[0] = static_cast<T>(vec.x);
            data[1] = static_cast<T>(vec.y);
        }
        
        vec2(const ImVec2& vec)
        {
            data[0] = static_cast<T>(vec.x);
            data[1] = static_cast<T>(vec.y);
        }

        const T& x() const
        {
            return data[0];
        }
        
        const T& y() const
        {
            return data[1];
        }
        
        const T& operator()(int32 i) const
        {
            return data[i];
        }
        
        T& operator[](int32 i)
        {
            return data[i];
        }
        
        void operator-=(const vec2<T>& v)
        {
            data[0] -= v.x;
            data[1] -= v.y;
        }

        void operator+=(const vec2<T>& v)
        {
            data[0] += v.x;
            data[1] += v.y;
        }

        void operator*=(const vec2<T>& v)
        {
            data[0] *= v.x;
            data[1] *= v.y;
        }

        void operator/=(const vec2<T>& v)
        {
            data[0] /= v.x;
            data[1] /= v.y;
        }
        
        void operator-=(const T v)
        {
            data[0] -= v;
            data[1] -= v;
        }

        void operator+=(const T v)
        {
            data[0] += v;
            data[1] += v;
        }

        void operator*=(const T v)
        {
            data[0] *= v;
            data[1] *= v;
        }

        void operator/=(const T v)
        {
            data[0] /= v;
            data[1] /= v;
        }
        
        vec2<T> operator<=>(const vec2<T>&) const = default;
        
        operator ImVec2() const
        {
            return ImVec2(static_cast<float>(data[0]), static_cast<float>(data[1]));
        }
        
        operator b2Vec2() const
        {
            return b2Vec2(static_cast<float>(data[0]), static_cast<float>(data[1]));
        }

        bool empty() const
        {
            return (data[0] == static_cast<T>(0) && data[1] == static_cast<T>(0));
        }
        
        static vec2<T> min(const vec2<T>& first, const vec2<T>& second)
        {
            return vec2<T>(std::min(first.x(), second.x()), std::min(first.y(), second.y()));
        }
        
        static vec2<T> max(const vec2<T>& first, const vec2<T>& second)
        {
            return vec2<T>(std::max(first.x(), second.x()), std::max(first.y(), second.y()));
        }
    };

    template<typename T>
    vec2<T> operator+(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x() + b.x(), a.y() + b.y());
    }

    template<typename T>
    vec2<T> operator-(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x() - b.x(), a.y() - b.y());
    }

    template<typename T>
    vec2<T> operator*(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x() * b.x(), a.y() * b.y());
    }

    template<typename T>
    vec2<T> operator/(const vec2<T>& a, const vec2<T>& b)
    {
        return vec2<T>(a.x() / b.x(), a.y() / b.y());
    }

    template<typename T>
    bool operator==(const vec2<T>& a, const vec2<T>& b)
    {
        return a.x() == b.x() && a.y() == b.y();
    }

    template<typename T>
    bool operator!=(const vec2<T>& a, const vec2<T>& b)
    {
        return a.x() != b.x() || a.y() != b.y();
    }

    using ivec2 = vec2<int16_t>;
    using fvec2 = vec2<float>;
    
    // quad or rect class, for rotations and min/max calculations
    template<typename T>
    struct rect
    {
        T values[4] = {};
        
        rect() = default;
        rect(vec2<T> first, vec2<T> second)
        {
            values[0] = std::min(first.x(), second.x());
            values[1] = std::min(first.y(), second.y());
            values[2] = std::max(first.x(), second.x());
            values[3] = std::max(first.y(), second.y());
        }
        
        rect(auto x, auto y, auto w, auto h)
        {
            values[0] = static_cast<T>(x);
            values[1] = static_cast<T>(y);
            values[2] = static_cast<T>(x + w);
            values[3] = static_cast<T>(y + h);
        }
        
        const T& min_x() const
        {
            return values[0];
        }
        
        const T& min_y() const
        {
            return values[1];
        }
        
        const T& max_x() const
        {
            return values[2];
        }
        
        const T& max_y() const
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
        
        void rotate(float angle)
        {
            
        }
        
        void rotate_centered(float angle, float center)
        {
            
        }
    };

    using irect = rect<int16_t>;
    using frect = rect<float>;

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
            float acos = std::acosf(rot.y());
            float angle = (rot.x() >= 0) ? acos : -acos;
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
            rot[0] = std::sinf(angle);
            rot[1] = std::cosf(angle);
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
