#pragma once

#include "Vec2.h"

namespace kuas
{
    template<typename T>
    struct Mat3
    {
        T m[3][3];

        Mat3() :
            m { 1, 0, 0,
                0, 1, 0,
                0, 0, 1 }
        {
        }

        explicit Mat3(float s) :
            m { s, s, s,
                s, s, s,
                s, s, s }
        {
        }

        explicit Mat3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22) :
            m { m00, m01, m02,
                m10, m11, m12,
                m20, m21, m22 }
        {
        }

        Mat3(const Mat3<T>& other) :
            m { other.m[0][0], other.m[0][1], other.m[0][2],
                other.m[1][0], other.m[1][1], other.m[1][2],
                other.m[2][0], other.m[2][1], other.m[2][2]  }
        {
        }

        template<typename OtherT>
        explicit Mat3(const Mat3<OtherT>& other) :
            m { static_cast<T>(other.m[0][0]), static_cast<T>(other.m[0][1]), static_cast<T>(other.m[0][2]),
                static_cast<T>(other.m[1][0]), static_cast<T>(other.m[1][1]), static_cast<T>(other.m[1][2]),
                static_cast<T>(other.m[2][0]), static_cast<T>(other.m[2][1]), static_cast<T>(other.m[2][2]) }
        {
        }

        Mat3<T>& operator=(const Mat3<T>& other)
        {
            m[0][0] = other.m[0][0]; m[0][1] = other.m[0][1]; m[0][2] = other.m[0][2];
            m[1][0] = other.m[1][0]; m[1][1] = other.m[1][1]; m[1][2] = other.m[1][2];
            m[2][0] = other.m[2][0]; m[2][1] = other.m[2][1]; m[2][2] = other.m[2][2];
            return *this;
        }

        Mat3<T>& operator+=(const Mat3<T>& other)
        {
            m[0][0] += other.m[0][0]; m[0][1] += other.m[0][1]; m[0][2] += other.m[0][2];
            m[1][0] += other.m[1][0]; m[1][1] += other.m[1][1]; m[1][2] += other.m[1][2];
            m[2][0] += other.m[2][0]; m[2][1] += other.m[2][1]; m[2][2] += other.m[2][2];
            return *this;
        }

        Mat3<T>& operator-=(const Mat3<T>& other)
        {
            m[0][0] -= other.m[0][0]; m[0][1] -= other.m[0][1]; m[0][2] -= other.m[0][2];
            m[1][0] -= other.m[1][0]; m[1][1] -= other.m[1][1]; m[1][2] -= other.m[1][2];
            m[2][0] -= other.m[2][0]; m[2][1] -= other.m[2][1]; m[2][2] -= other.m[2][2];
            return *this;
        }

        Mat3<T>& operator*=(const Mat3<T>& other)
        {
            *this = *this * other;
            return *this;
        }

        T* operator[](size_t i)
        {
            return m[i];
        }

        template<class FriendT>
        friend static Mat3<FriendT> operator*(const Mat3<FriendT>& a, const Mat3<FriendT>& b);

        static inline Mat3<T> translate(T x, T y) noexcept
        {
            return Mat3<T>(
                1.0f, 0.0f, x,
                0.0f, 1.0f, y,
                0.0f, 0.0f, 1.0f);
        }

        static inline Mat3<T> scale(T x, T y) noexcept
        {
            return Mat3<T>(
                x,    0.0f, 0.0f,
                0.0f, y,    0.0f,
                0.0f, 0.0f, 1.0f);
        }

        static inline Mat3<T> scaleAt(T x, T y, const Vec2<T>& center) noexcept
        {
            return Mat3<T>(
                x,    0.0f, center.x - x * center.x,
                0.0f, y,    center.y - y * center.y,
                0.0f, 0.0f, 1.0f);
        }

        static inline Mat3<T> rotate(T angle) noexcept
        {
            T s = std::sin(angle);
            T c = std::cos(angle);
            return Mat3<T>(
                 c,    s,    0.0f,
                -s,    c,    0.0f,
                 0.0f, 0.0f, 1.0f);
        }

    private:
        Mat3(bool)
        {
        }
    };

    template<typename T>
    static inline Mat3<T> operator+(const Mat3<T>& a, const Mat3<T>& b)
    {
        return Mat3<T>(
            a.m[0][0] + b.m[0][0], a.m[0][1] + b.m[0][1], a.m[0][2] + b.m[0][2],
            a.m[1][0] + b.m[1][0], a.m[1][1] + b.m[1][1], a.m[1][2] + b.m[1][2],
            a.m[2][0] + b.m[2][0], a.m[2][1] + b.m[2][1], a.m[2][2] + b.m[2][2]);
    }

    template<typename T>
    static inline Mat3<T> operator-(const Mat3<T>& a, const Mat3<T>& b)
    {
        return Mat3<T>(
            a.m[0][0] - b.m[0][0], a.m[0][1] - b.m[0][1], a.m[0][2] - b.m[0][2],
            a.m[1][0] - b.m[1][0], a.m[1][1] - b.m[1][1], a.m[1][2] - b.m[1][2],
            a.m[2][0] - b.m[2][0], a.m[2][1] - b.m[2][1], a.m[2][2] - b.m[2][2]);
    }

    template<typename T>
    static inline Mat3<T> operator*(const Mat3<T>& a, const Mat3<T>& b)
    {
        Mat3<T> ret(true);

        float x = a.m[0][0];
        float y = a.m[0][1];
        float z = a.m[0][2];
        ret.m[0][0] = x * b.m[0][0] + y * b.m[1][0] + z * b.m[2][0];
        ret.m[0][1] = x * b.m[0][1] + y * b.m[1][1] + z * b.m[2][1];
        ret.m[0][2] = x * b.m[0][2] + y * b.m[1][2] + z * b.m[2][2];

        x = a.m[1][0];
        y = a.m[1][1];
        z = a.m[1][2];
        ret.m[1][0] = x * b.m[0][0] + y * b.m[1][0] + z * b.m[2][0];
        ret.m[1][1] = x * b.m[0][1] + y * b.m[1][1] + z * b.m[2][1];
        ret.m[1][2] = x * b.m[0][2] + y * b.m[1][2] + z * b.m[2][2];

        x = a.m[2][0];
        y = a.m[2][1];
        z = a.m[2][2];
        ret.m[2][0] = x * b.m[0][0] + y * b.m[1][0] + z * b.m[2][0];
        ret.m[2][1] = x * b.m[0][1] + y * b.m[1][1] + z * b.m[2][1];
        ret.m[2][2] = x * b.m[0][2] + y * b.m[1][2] + z * b.m[2][2];

        return ret;
    }

    using Mat3F = Mat3<float>;
}
