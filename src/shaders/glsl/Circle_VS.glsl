#version 450 core

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec4 i_col;
layout(location = 2) in float i_radius;
#ifndef KUAS_FILL_SHAPE
layout(location = 3) in float i_thickness;
#endif

layout(location = 0) out vec4 gs_col;
layout(location = 1) out float gs_radius;
#ifndef KUAS_FILL_SHAPE
layout(location = 2) out float gs_thickness;
#endif

void main()
{
    gl_Position.xy = i_pos;
    gs_col = i_col;
    gs_radius = i_radius;
#ifndef KUAS_FILL_SHAPE
    gs_thickness = i_thickness;
#endif
}
