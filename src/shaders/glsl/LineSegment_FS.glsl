#version 450 core

layout(location = 0) in vec4 gs_col;
#ifdef KUAS_ANTIALIAS
layout(location = 1) in vec2 gs_pos;
layout(location = 2) in vec2 gs_size;
#endif

layout(location = 0) out vec4 o_col;

void main()
{
    o_col = gs_col;
#ifdef KUAS_ANTIALIAS
    float dy = length(vec2(dFdx(gs_pos.y), dFdy(gs_pos.y)));
    //float dy = fwidth(gs_pos.y);
    vec2 p = abs(gs_pos - gs_size);
    float d = 0;
#ifdef KUAS_ROUND_CAPPED_LINE
    float r = gs_size.y;
#else
    float r = 0.5;
#endif
    
    // we split into two cases to avoid very thin line caused by partial derivative.
    if (p.x < (gs_size.x - r)) {
        d = p.y - gs_size.y - 0.5;
        d = d / dy + 0.5;
    }
    else {
#ifdef KUAS_ROUND_CAPPED_LINE
        p.x -= gs_size.x - r;
        d = length(p) - r;
#else
        p -= gs_size;
        d = max(p.x, p.y);
#endif
        float ddf = length(vec2(dFdx(d), dFdy(d)));
        d -= 0.5;
        d = d / ddf + 0.5;
    }

    o_col.a *= 1 - clamp(d, 0, 1);
#endif
}
