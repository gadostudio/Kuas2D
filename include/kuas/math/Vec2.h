#pragma once

#include "CommonMath.h"

namespace kuas
{
    template<typename T>
    struct Vec2
    {
        T x, y;

        inline Vec2(T scalar = 0) :
            x(scalar), y(scalar)
        {
        }

        inline Vec2(T x, T y) :
            x(x), y(y)
        {
        }

        inline Vec2(const Vec2<T>& other) :
            x(other.x), y(other.y)
        {
        }

        template<typename OtherT>
        inline explicit Vec2(const Vec2<OtherT>& other) :
            x(static_cast<T>(other.x)),
            y(static_cast<T>(other.y))
        {
        }

        inline Vec2<T>& operator=(const Vec2<T>& other)
        {
            x = other.x;
            y = other.y;
            return *this;
        }
    };

    template<typename T> Vec2<T> operator+(const Vec2<T>& a, const Vec2<T>& b) { return Vec2<T>(a.x + b.x, a.y + b.y); }
    template<typename T> Vec2<T> operator-(const Vec2<T>& a, const Vec2<T>& b) { return Vec2<T>(a.x - b.x, a.y - b.y); }
    template<typename T> Vec2<T> operator*(const Vec2<T>& a, const Vec2<T>& b) { return Vec2<T>(a.x * b.x, a.y * b.y); }
    template<typename T> Vec2<T> operator/(const Vec2<T>& a, const Vec2<T>& b) { return Vec2<T>(a.x / b.x, a.y / b.y); }

    template<typename T> Vec2<T> operator+(const Vec2<T>& a, T s) { return Vec2<T>(a.x + s, a.y + s); }
    template<typename T> Vec2<T> operator-(const Vec2<T>& a, T s) { return Vec2<T>(a.x - s, a.y - s); }
    template<typename T> Vec2<T> operator*(const Vec2<T>& a, T s) { return Vec2<T>(a.x * s, a.y * s); }
    template<typename T> Vec2<T> operator/(const Vec2<T>& a, T s) { return Vec2<T>(a.x / s, a.y / s); }

    template<typename T>
    inline static T dot(const Vec2<T>& a, const Vec2<T>& b)
    {
        return (a.x * b.x) + (a.y * b.y);
    }

    template<typename T>
    inline static T lengthSq(const Vec2<T>& v)
    {
        return dot(v, v);
    }

    template<typename T>
    inline static T length(const Vec2<T>& v)
    {
        return static_cast<T>(std::sqrt(dot(v, v)));
    }

    template<typename T>
    inline static Vec2<T> normalize(const Vec2<T>& v)
    {
        return v / length(v);
    }

    template<typename T>
    inline static T distanceSq(const Vec2<T>& a, const Vec2<T>& b)
    {
        return lengthSq(b - a);
    }

    template<typename T>
    inline static T distance(const Vec2<T>& a, const Vec2<T>& b)
    {
        return length(b - a);
    }

    using Vec2I = Vec2<int32_t>;
    using Vec2U = Vec2<uint32_t>;
    using Vec2F = Vec2<float>;
}
