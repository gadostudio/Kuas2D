#pragma once

#include <cstdint>

namespace kuas
{
    struct ColorRGBA
    {
        float r, g, b, a;

        inline ColorRGBA() :
            r(0), g(0), b(0), a(0)
        {
        }

        inline explicit ColorRGBA(float gray, float alpha = 1.0f) :
            r(gray), g(gray), b(gray), a(alpha)
        {
        }

        inline explicit ColorRGBA(float red, float green, float blue, float alpha = 1.0f) :
            r(red), g(green), b(blue), a(alpha)
        {
        }

        inline explicit ColorRGBA(uint32_t c32) :
            r(static_cast<float>(c32 & 255) / 255.f),
            g(static_cast<float>((c32 >> 8) & 255) / 255.f),
            b(static_cast<float>((c32 >> 16) & 255) / 255.f),
            a(static_cast<float>((c32 >> 24) & 255) / 255.f)
        {
        }

        inline ColorRGBA(const ColorRGBA& other) :
            r(other.r), g(other.g), b(other.b), a(other.a)
        {
        }

        inline ColorRGBA& operator=(const ColorRGBA& other)
        {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }

        inline ColorRGBA& operator+=(const ColorRGBA& other)
        {
            r += other.r;
            g += other.g;
            b += other.b;
            a += other.a;
            return *this;
        }

        inline ColorRGBA& operator-=(const ColorRGBA& other)
        {
            r -= other.r;
            g -= other.g;
            b -= other.b;
            a -= other.a;
            return *this;
        }

        inline ColorRGBA& operator*=(const ColorRGBA& other)
        {
            r *= other.r;
            g *= other.g;
            b *= other.b;
            a *= other.a;
            return *this;
        }

        inline ColorRGBA& operator/=(const ColorRGBA& other)
        {
            r /= other.r;
            g /= other.g;
            b /= other.b;
            a /= other.a;
            return *this;
        }

        inline static ColorRGBA adjustContrast(const ColorRGBA& c, float contrast)
        {
            return ColorRGBA(
                ((c.r - 0.5f) * contrast) + 0.5f,
                ((c.g - 0.5f) * contrast) + 0.5f,
                ((c.b - 0.5f) * contrast) + 0.5f);
        }

        inline static ColorRGBA brighten(const ColorRGBA& c, float darkAmount)
        {
            darkAmount = 1.0f / (1.0f + darkAmount);

            return ColorRGBA(
                1.0f - (1.0f - c.r) * darkAmount,
                1.0f - (1.0f - c.g) * darkAmount,
                1.0f - (1.0f - c.b) * darkAmount);
        }

        inline static ColorRGBA darken(const ColorRGBA& c, float darkAmount)
        {
            darkAmount = 1.0f / (1.0f + darkAmount);

            return ColorRGBA(
                c.r * darkAmount,
                c.g * darkAmount,
                c.b * darkAmount);
        }

        inline static ColorRGBA invert(const ColorRGBA& c, bool withAlpha = false)
        {
            return ColorRGBA(
                1.0f - c.r, 1.0f - c.g, 1.0f - c.b,
                withAlpha ? 1.0f - c.a : c.a);
        }
    };

    inline static ColorRGBA operator+(const ColorRGBA& a, const ColorRGBA& b)
    {
        return ColorRGBA(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
    }

    inline static ColorRGBA operator-(const ColorRGBA& a, const ColorRGBA& b)
    {
        return ColorRGBA(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
    }

    inline static ColorRGBA operator*(const ColorRGBA& a, const ColorRGBA& b)
    {
        return ColorRGBA(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
    }

    inline static ColorRGBA operator/(const ColorRGBA& a, const ColorRGBA& b)
    {
        return ColorRGBA(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a);
    }
}
