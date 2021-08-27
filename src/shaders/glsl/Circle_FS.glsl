#version 450 core

layout(location = 0) in vec2 gs_pos;
layout(location = 1) in vec4 gs_col;
layout(location = 2) in float gs_radius;
#ifndef KUAS_FILL_SHAPE
layout(location = 3) in float gs_thickness;
#endif

layout(location = 0) out vec4 o_col;

float sdfCircle(vec2 p, float radius)
{
    return length(p) - radius;
}

void main()
{
    float d = sdfCircle(gs_pos - gs_radius, gs_radius);
    float ddf = length(vec2(dFdx(d), dFdy(d)));
#ifndef KUAS_FILL_SHAPE
    d = abs(d) - gs_thickness;
#endif
    d -= 0.5;
    d = clamp(d / ddf + 0.5, 0.0, 1.0);
    o_col = gs_col;
#ifdef KUAS_ANTIALIAS
    o_col.a *= 1.0 - d;
#else
    o_col.a *= 1.0 - step(0.5, d);
#endif
}
