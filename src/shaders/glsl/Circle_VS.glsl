#version 450 core

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec4 i_col;
layout(location = 2) in float i_radius;
#ifndef KUAS_FILL_SHAPE
layout(location = 3) in float i_thickness;
#endif

layout(location = 0) out vec4 gs_col;

void main()
{
    gl_Position.xyz = vec3(i_pos + 0.5, i_radius);
#ifndef KUAS_FILL_SHAPE
    gl_Position.w = i_thickness;
#endif
    gs_col = i_col;
}
