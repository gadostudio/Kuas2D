#version 450 core

layout(location = 0) in vec4 i_bbox;
layout(location = 1) in vec4 i_col;
#ifndef KUAS_FILL_SHAPE
layout(location = 2) in float i_thickness;
#endif
#ifdef KUAS_ROUNDED_SHAPE
layout(location = 3) in float i_roundness;
#endif

layout(location = 0) out vec4 gs_col;
#ifndef KUAS_FILL_SHAPE
layout(location = 1) out float gs_thickness;
#endif
#ifdef KUAS_ROUNDED_SHAPE
layout(location = 2) out float gs_roundness;
#endif

layout(push_constant, row_major) uniform PushConstants {
    mat3 transform;
    vec2 scale;
};

void main()
{
    gl_Position = i_bbox + 0.5;
    gs_col = i_col;
#ifndef KUAS_FILL_SHAPE
    gs_thickness = i_thickness;
#endif
#ifdef KUAS_ROUNDED_SHAPE
    gs_roundness = i_roundness;
#endif
}
