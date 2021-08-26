#version 450 core

layout(location = 0) in vec4 gs_col;
#ifdef KUAS_ANTIALIAS
layout(location = 1) in vec2 gs_pos;
layout(location = 2) in float gs_width;
#endif

layout(location = 0) out vec4 o_col;

void main()
{
    o_col = gs_col;
#ifdef KUAS_ANTIALIAS
    const float sq2_2 = sqrt(2.) / 2.;
    float d = gs_pos.y;
    float ddf = sq2_2 * length(vec2(dFdx(d), dFdy(d)));
    d = smoothstep(0.5 - ddf, 0.5 + ddf, abs(d) - gs_width);
    o_col = gs_col;
    o_col.a *= 1 - clamp(d, 0, 1);
#endif
}
