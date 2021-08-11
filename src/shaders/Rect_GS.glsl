#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// from vertex shader
layout(location = 0) in vec4 vs_col[];
#ifndef KUAS_FILL_SHAPE
layout(location = 1) in float vs_thickness[];
#endif
#ifdef KUAS_ROUNDED_SHAPE
layout(location = 2) in float vs_roundness[];
#endif


// to fragment shader
layout(location = 0) out vec4 fs_col;
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
layout(location = 1) out vec2 fs_size;
layout(location = 2) out vec2 fs_pos;
#endif
#ifndef KUAS_FILL_SHAPE
layout(location = 3) out float fs_thickness;
#endif
#ifdef KUAS_ROUNDED_SHAPE
layout(location = 4) out float fs_roundness;
#endif


layout(push_constant, row_major) uniform PushConstants {
    mat3 transform;
    vec2 scale;
};

void main()
{
    vec2 a = gl_in[0].gl_Position.xy;
    vec2 b = gl_in[0].gl_Position.zw;
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
    // preserve pixel for AA parts
    vec2 size = b - a;
    vec2 halfSize = size * 0.5;
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

    fs_col = vs_col[0];
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
    fs_size = halfSize;
    fs_pos = vec2(-1.0);
#endif
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos - vs_thickness[0];
    fs_thickness = vs_thickness[0];
#endif
#ifdef KUAS_ROUNDED_SHAPE
    fs_roundness = vs_roundness[0];
#endif
    gl_Position = vec4(e0.xy * scale - 1.0, 0.0, 1.0); // top-left
    EmitVertex();

    fs_col = vs_col[0];
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
    fs_size = halfSize;
    fs_pos = vec2(size.x + 1.0, -1.0);
#endif
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos + vec2(vs_thickness[0], -vs_thickness[0]);
    fs_thickness = vs_thickness[0];
#endif
#ifdef KUAS_ROUNDED_SHAPE
    fs_roundness = vs_roundness[0];
#endif
    gl_Position = vec4(e1.xy * scale - 1.0, 0.0, 1.0); // top-right
    EmitVertex();

    fs_col = vs_col[0];
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
    fs_size = halfSize;
    fs_pos = vec2(-1.0, size.y + 1.0);
#endif
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos + vec2(-vs_thickness[0], vs_thickness[0]);
    fs_thickness = vs_thickness[0];
#endif
#ifdef KUAS_ROUNDED_SHAPE
    fs_roundness = vs_roundness[0];
#endif
    gl_Position = vec4(e2.xy * scale - 1.0, 0.0, 1.0); // bottom-left
    EmitVertex();

    fs_col = vs_col[0];
#if defined(KUAS_ANTIALIAS) || !defined(KUAS_FILL_SHAPE)
    fs_size = halfSize;
    fs_pos = size + 1.0;
#endif
#ifndef KUAS_FILL_SHAPE
    fs_pos = fs_pos + vs_thickness[0];
    fs_thickness = vs_thickness[0];
#endif
#ifdef KUAS_ROUNDED_SHAPE
    fs_roundness = vs_roundness[0];
#endif
    gl_Position = vec4(e3.xy * scale - 1.0, 0.0, 1.0); // bottom-right
    EmitVertex();

    EndPrimitive();
}
