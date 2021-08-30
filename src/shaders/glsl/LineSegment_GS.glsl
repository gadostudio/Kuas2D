#version 450 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

layout(location = 0) in vec4 vs_col[];

layout(location = 0) out vec4 fs_col;
#ifdef KUAS_ANTIALIAS
layout(location = 1) out vec2 fs_pos;
layout(location = 2) out vec2 fs_size;
#endif

layout(push_constant, row_major) uniform PushConstants {
    mat3 transform;
    vec2 scale;
};

void main()
{
    vec2 a = gl_in[0].gl_Position.xy;
    vec2 b = gl_in[1].gl_Position.xy;
    float w_a = gl_in[0].gl_Position.z;
    float w = w_a * 2.0 + 1.0;

#ifdef KUAS_ANTIALIAS
    w_a += 1.5;
#else
    w_a += 0.5;
#endif

    // calculate offset
    vec2 d = b - a;
    vec2 t = normalize(d);
    vec2 o = w_a * vec2(-t.y, t.x);

#ifdef KUAS_CAPPED_LINE
    t *= w_a;
    float ld = length(d + 2.0 * t);
#else
    float ld = length(d);
#endif

#ifdef KUAS_ANTIALIAS
    vec2 halfsize = (vec2(ld, w) * 0.5) - 0.5;
    // transform edges
    vec3 e0 = transform * vec3(a - o - t, 1.0);
    vec3 e1 = transform * vec3(b - o + t, 1.0);
    vec3 e2 = transform * vec3(a + o - t, 1.0);
    vec3 e3 = transform * vec3(b + o + t, 1.0);
#else
    // transform edges
    vec3 e0 = transform * vec3(a - o, 1.0);
    vec3 e1 = transform * vec3(b - o, 1.0);
    vec3 e2 = transform * vec3(a + o, 1.0);
    vec3 e3 = transform * vec3(b + o, 1.0);
#endif

    fs_col = vs_col[0];
#ifdef KUAS_ANTIALIAS
    fs_pos = vec2(-1);
    fs_size = halfsize;
#endif
    gl_Position = vec4(e0.xy * scale - 1.0, 0.0, 1.0); // top-left
    EmitVertex();

    fs_col = vs_col[0];
#ifdef KUAS_ANTIALIAS
    fs_pos = vec2(ld + 1, -1.0);
    fs_size = halfsize;
#endif
    gl_Position = vec4(e1.xy * scale - 1.0, 0.0, 1.0); // top-right
    EmitVertex();

    fs_col = vs_col[0];
#ifdef KUAS_ANTIALIAS
    fs_pos = vec2(-1, w + 1);
    fs_size = halfsize;
#endif
    gl_Position = vec4(e2.xy * scale - 1.0, 0.0, 1.0); // bottom-left
    EmitVertex();

    fs_col = vs_col[0];
#ifdef KUAS_ANTIALIAS
    fs_pos = vec2(ld, w) + 1.0;
    fs_size = halfsize;
#endif
    gl_Position = vec4(e3.xy * scale - 1.0, 0.0, 1.0); // bottom-right
    EmitVertex();

    EndPrimitive();
}
