#version 450 core

layout(location = 0) in vec4 vs_col;
layout(location = 0) out vec4 o_col;

void main()
{
    o_col = vs_col;
}
