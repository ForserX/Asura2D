#pragma once

#undef M_PI
#define M_PI 3.14159265358979323846

namespace Asura::Math
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
    public:
        static constexpr bool string_serialize = true;

    public:
        union
        {
            struct {
                T data[2];
            };

            struct {
                T x;
                T y;
            };
        };

    public:
        vec2() : x(0), y(0) {};
        vec2(auto dx, auto dy)
        {
            data[0] = static_cast<T>(dx);
            data[1] = static_cast<T>(dy);
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
            if constexpr (std::is_floating_point_v<T>) {
                return (x == FLT_MAX && y == FLT_MAX) || (x == static_cast<T>(0) && y == static_cast<T>(0));
            }
            
            return (x == static_cast<T>(0) && y == static_cast<T>(0));
        }

        stl::string to_string()
        {
            return "[" + stl::to_string(x) + "," + stl::to_string(y) + "]";
        }

        void from_string(const stl::string_view& sval)
        {
            size_t offset = 0;
            auto get_string = [&sval, &offset](char start, char end) {
                const size_t begin_offset = sval.find_first_not_of(start, offset);
                const size_t end_offset = sval.find_first_of(end, begin_offset);
                offset = end_offset;

                return stl::string(sval.begin() + begin_offset, sval.begin() + end_offset);
            };

            offset = sval.find_first_not_of('[');
            if (offset != static_cast<size_t>(-1)) {
                data[0] = stl::stod(get_string('[', ','));
                data[1] = stl::stod(get_string(',', ']'));
            }
        }

        static vec2<T> unstrigify(const stl::string& sval)
        {
            vec2<T> vec;
            vec.from_string(sval);
            return vec;
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

    using IVec2 = vec2<int16_t>;
    using FVec2 = vec2<float>;
    
    // quad or Rect class, for rotations and min/max calculations
    template<typename T>
    struct Rect
    {
        T values[4] = {};
        
        Rect() = default;
        Rect(vec2<T> first, vec2<T> second)
        {
            values[0] = std::min(first.x, second.x);
            values[1] = std::min(first.y, second.y);
            values[2] = std::max(first.x, second.x);
            values[3] = std::max(first.y, second.y);
        }
        
        Rect(auto x, auto y, auto w, auto h)
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
        
        vec2<T> center() const
        {
            return {
                min_x() + 0.5 * (max_x() - min_x()),
                min_y() + 0.5 * (max_y() - min_y())
            };
        }

        vec2<T> min() const
        {
            return { min_x(), min_y()};
        }
        
        vec2<T> max() const
        {
            return { max_x(), max_y()};
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

        void operator+=(const Rect& Left)
        {
            values[0] += Left.values[0];
            values[1] += Left.values[1];
            values[2] += Left.values[2];
            values[3] += Left.values[3];
        }

        void operator-=(const Rect& Left)
        {
            values[0] -= Left.values[0];
            values[1] -= Left.values[1];
            values[2] -= Left.values[2];
            values[3] -= Left.values[3];
        }
    };

    using IRect = Rect<int16_t>;
    using FRect = Rect<float>;

    class Transform
    {
    public:
        static constexpr bool string_serialize = true;

    private:
        FVec2 pos = {};
        FVec2 rot = {};        // sin - x, cos - y
        
    public:
        Transform() = default;
        Transform(FVec2 in_pos, FVec2 in_rot)
        {
            pos = in_pos;
            rot = in_rot;
        }

        float angle() const
        {
            float acos = ::acosf(rot.y);
            float angle = (rot.x >= 0) ? acos : -acos;
            return angle;
        }
        
        const FVec2& rotation() const
        {
            return rot;
        }
        
        const FVec2& position() const
        {
            return pos;
        }
        
        void set_angle(float angle)
        {
            rot[0] = ::sinf(angle);
            rot[1] = ::cosf(angle);
        }
        
        void set_rotation(const FVec2& new_rot)
        {
            rot = new_rot;
        }
        
        void set_position(const FVec2& new_pos)
        {
            pos = new_pos;
        }

        stl::string to_string()
        {
            return "[ " + stl::to_string(pos.x) + " " + stl::to_string(pos.y) + " " + stl::to_string(rot.x) + " " + stl::to_string(rot.y) + " ]";
        }

        void from_string(const stl::string& sval)
        {
            size_t offset = 0;
            auto get_string = [&sval, &offset]() {
                const size_t begin_offset = sval.find_first_not_of(' ', offset);
                const size_t end_offset = sval.find_first_of(' ', begin_offset);
                offset = end_offset;

                return stl::string(sval.begin() + begin_offset, sval.begin() + end_offset);
            };

            offset = sval.find_first_not_of('[');
            if (offset != -1) {
                pos[0] = stl::stof(get_string());
                pos[1] = stl::stof(get_string());
                rot[0] = stl::stof(get_string());
                rot[1] = stl::stof(get_string());
            }
        }

        static Transform unstrigify(const stl::string& sval)
        {
            Transform trans;
            trans.from_string(sval);
            return trans;
        }
    };

    inline float Degrees(float rad)
    {
        return (rad / M_PI) * 360.f;
    }

    inline float Radians(float degrees)
    {
        return (degrees / 360.f) * M_PI;
    }

    float Distance(float x, float x2);

    inline void Multiply4x4(const float* mat1, const float* mat2, float* res)
    {
        int i, j, k;
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                res[i * 4 + j] = 0;
                for (k = 0; k < 4; k++)
                    res[i * 4 + j] += mat1[i * 4 + k] * mat2[k * 4 + j];
            }
        }
    }
}