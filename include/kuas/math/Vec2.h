#pragma once

namespace kuas
{
    template<typename T>
    struct Vec2
    {
        T x, y;

        Vec2(T scalar = 0) :
            x(scalar), y(scalar)
        {
        }

        Vec2(T x, T y) :
            x(x), y(y)
        {
        }

        Vec2(const Vec2<T>& other) :
            x(other.x), y(other.y)
        {
        }

        template<typename OtherT>
        explicit Vec2(const Vec2<OtherT>& other) :
            x(static_cast<T>(other.x)),
            y(static_cast<T>(other.y))
        {
        }

        Vec2<T>& operator=(const Vec2<T>& other)
        {
            x = other.x;
            y = other.y;
            return *this;
        }
    };

    using Vec2F = Vec2<float>;
}
