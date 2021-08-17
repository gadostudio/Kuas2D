#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(location = 0) in vec4 vs_col[];
layout(location = 1) in float vs_radius[];
#ifndef KUAS_FILL_SHAPE
layout(location = 2) in float vs_thickness[];
#endif

layout(location = 0) out vec2 fs_pos;
layout(location = 1) out vec4 fs_col;
layout(location = 2) out float fs_radius;
#ifndef KUAS_FILL_SHAPE
layout(location = 3) out float fs_thickness;
#endif

layout(push_constant, row_major) uniform PushConstants {
    mat3 transform;
    vec2 scale;
};

void main()
{
    float rad = vs_radius[0];
    vec2 a = gl_in[0].gl_Position.xy - rad;
    vec2 b = gl_in[0].gl_Position.xy + rad;
    vec2 d = vec2(rad * 2.);
#ifdef KUAS_ANTIALIAS
    // preserve pixel for AA parts
    a -= vec2(1.0);
    b += vec2(1.0);
#endif
#ifndef KUAS_FILL_SHAPE
    // preserve pixel for thickness part
    a -= vs_thickness[0];
    b += vs_thickness[0];
#endif

    // transform edges
    vec3 e0 = transform * vec3(a, 1.0);
    vec3 e1 = transform * vec3(b.x, a.y, 1.0);
    vec3 e2 = transform * vec3(a.x, b.y, 1.0);
    vec3 e3 = transform * vec3(b, 1.0);

    fs_pos = vec2(-1.0);
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos - vs_thickness[0];
    fs_thickness = vs_thickness[0];
#endif
    fs_col = vs_col[0];
    fs_radius = rad;
    gl_Position = vec4(e0.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    fs_pos = vec2(d.x + 1.0, -1.0);
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos + vec2(vs_thickness[0], -vs_thickness[0]);
    fs_thickness = vs_thickness[0];
#endif
    fs_col = vs_col[0];
    fs_radius = rad;
    gl_Position = vec4(e1.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    fs_pos = vec2(-1.0, d.y + 1.0);
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos + vec2(-vs_thickness[0], vs_thickness[0]);
    fs_thickness = vs_thickness[0];
#endif
    fs_col = vs_col[0];
    fs_radius = rad;
    gl_Position = vec4(e2.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    fs_pos = d + 1.0;
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos + vs_thickness[0];
    fs_thickness = vs_thickness[0];
#endif
    fs_col = vs_col[0];
    fs_radius = rad;
    gl_Position = vec4(e3.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
