#version 450 core

layout(location = 0) in vec4 i_col;
layout(location = 0) out vec4 o_col;

float sdfCircle(vec2 p, float radius)
{
    return length(p) - radius;
}

void main()
{
    float d = sdfCircle(p);
    o_col = vec4(i_col, a);
}
