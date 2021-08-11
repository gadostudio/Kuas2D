#version 450 core

// from geometry shader
layout(location = 0) in vec4 gs_col;
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
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

float sdfBox(in vec2 p, in vec2 size)
{
    vec2 d = abs(p) - size;
    return max(d.x, d.y);
}

void main()
{
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
    float d = sdfBox(gs_pos - gs_size, gs_size);
#endif
#ifndef KUAS_FILL_SHAPE
    d = abs(d) - gs_thickness;
#endif

#ifdef KUAS_ANTIALIAS
    o_col = vec4(gs_col.rgb, gs_col.a * clamp(1.0 - d, 0.0, 1.0));
#else
    o_col = gs_col;
#ifndef KUAS_FILL_SHAPE
    o_col.a *= step(0.5, clamp(1.0 - d, 0.0, 1.0));
#endif
#endif
}
