#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(location = 0) in vec4 vs_col[];

// We gonna reuse the circle shader
layout(location = 0) out vec2 fs_pos;
layout(location = 1) out vec4 fs_col;
layout(location = 2) out float fs_radius;

layout(push_constant, row_major) uniform PushConstants{
    mat3 transform;
    vec2 scale;
};

void main()
{
    vec2 rad = gl_in[0].gl_Position.zw;
    float max_rad = max(rad.x, rad.y);
    vec2 a = gl_in[0].gl_Position.xy - rad;
    vec2 b = gl_in[0].gl_Position.xy + rad;
    vec2 d = vec2(max_rad * 2.);
#ifdef KUAS_ANTIALIAS
    // preserve pixel for AA parts
    a -= vec2(1.0);
    b += vec2(1.0);
#endif

    rad -= 0.5;

    // transform edges
    vec3 e0 = transform * vec3(a, 1.0);
    vec3 e1 = transform * vec3(b.x, a.y, 1.0);
    vec3 e2 = transform * vec3(a.x, b.y, 1.0);
    vec3 e3 = transform * vec3(b, 1.0);

    fs_pos = vec2(-1.0);
    fs_col = vs_col[0];
    fs_radius = max_rad;
    gl_Position = vec4(e0.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    fs_pos = vec2(d.x + 1.0, -1.0);
    fs_col = vs_col[0];
    fs_radius = max_rad;
    gl_Position = vec4(e1.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    fs_pos = vec2(-1.0, d.y + 1.0);
    fs_col = vs_col[0];
    fs_radius = max_rad;
    gl_Position = vec4(e2.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    fs_pos = d + 1.0;
    fs_col = vs_col[0];
    fs_radius = max_rad;
    gl_Position = vec4(e3.xy * scale - 1.0, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
