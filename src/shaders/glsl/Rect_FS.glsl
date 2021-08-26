#version 450 core

#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE) || defined(KUAS_ROUNDED_SHAPE)
#define KUAS_USE_SDF
#endif

// from geometry shader
layout(location = 0) in vec4 gs_col;
#ifdef KUAS_USE_SDF
layout(location = 1) in vec2 gs_size;
layout(location = 2) in vec2 gs_pos;
#endif
#ifndef KUAS_FILL_SHAPE
layout(location = 3) in float gs_thickness;
#endif
#ifdef KUAS_ROUNDED_SHAPE
layout(location = 4) in float gs_roundness;
#endif

// to framebuffer
layout(location = 0) out vec4 o_col;

float sdfRect(in vec2 p, in vec2 size)
{
    vec2 d = abs(p) - size;
    return max(d.x, d.y);
}

float sdfRoundedRect(in vec2 p, in vec2 size, in float roundness)
{
    float min_rad = min(roundness, min(size.x, size.y));
    vec2 d = abs(p) - (size - min_rad);
    return (length(max(d, 0.0)) + min(max(d.x, d.y), 0.0)) - min_rad;
}

void main()
{
    const float sq2_2 = sqrt(2.) / 2.;
#ifdef KUAS_USE_SDF
#ifdef KUAS_ROUNDED_SHAPE
    float round_factor = 0;
#ifndef KUAS_FILL_SHAPE
    round_factor = max(0, gs_thickness - gs_roundness); // adjust roundness based on thickness
#endif
    float d = sdfRoundedRect(gs_pos - gs_size, gs_size, gs_roundness - round_factor);
#else
    float d = sdfRect(gs_pos - gs_size, gs_size);
#endif
    float ddf = sq2_2 * length(vec2(dFdx(d), dFdy(d)));
#endif

#ifndef KUAS_FILL_SHAPE
    d = abs(d) - gs_thickness;
#endif

#ifdef KUAS_USE_SDF
    //d = clamp(d / ddf + 0.5, 0.0, 1.0);
    d = clamp(smoothstep(0.5 - ddf, 0.5 + ddf, d), 0.0, 1.0);
#endif

    o_col = gs_col;
#ifdef KUAS_ANTIALIAS
    o_col.a *= 1.0 - d;
#else
#if !defined(KUAS_FILL_SHAPE) || defined(KUAS_ROUNDED_SHAPE)
    o_col.a *= step(0.5, 1.0 - d);
#endif
#endif
}
