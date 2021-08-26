#version 450 core

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec4 i_col;
layout(location = 2) in float i_width;

layout(location = 0) out vec4 gs_col;

void main()
{
    gl_Position = vec4(i_pos + 0.5, i_width, 0.0);
    gs_col = i_col;
}
