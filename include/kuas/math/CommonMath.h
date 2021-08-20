#pragma once

#include <cmath>
#include <cstdint>

namespace kuas
{
    template<typename T>
    struct Constants
    {
        static constexpr T PI = T(3.141592654);
        static constexpr T TWOPI = PI * 2;
    };

    template<typename T>
    inline static constexpr T degreeToRadian(T degree)
    {
        static constexpr T c = Constants<T>::PI / 180;
        return degree * c;
    }

    template<typename T>
    inline static constexpr T radianToDegree(T radian)
    {
        static constexpr T c = 180 / Constants<T>::PI;
        return radian * c;
    }
}
