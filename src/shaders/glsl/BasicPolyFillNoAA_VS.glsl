#version 450 core

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec4 i_col;

layout(location = 0) out vec4 fs_col;

layout(push_constant, row_major) uniform PushConstants {
    mat3 transform;
    vec2 scale;
};

void main()
{
    vec3 v = (transform * vec3(i_pos, 1.0)) * vec3(scale, 1.0);
    v.xy = vec2(v.x, -v.y) - vec2(1.0);
    gl_Position = vec4(v, 1.0);
    fs_col = i_col;
}
