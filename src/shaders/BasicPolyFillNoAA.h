#pragma once

namespace kuas
{
    static const char* shaderBasicPolyFillNoAA_VS =
        "layout(location = 0) in vec2 i_pos;"
        "layout(location = 1) in vec2 i_col;"

        "out vec4 fs_col"

        "layout(push_constant) uniform {"
        "   mat3 transform;"
        "} pushConstants;"

        "void main()"
        "{"
        "   gl_Position = vec4(pushConstants.transform * vec3(i_pos, 1.0), 1.0);"
        "}";

    static const char* shaderBasicPolyFillNoAA_FS =
        "in vec4 fs_col;"
        "layout(location = 0) out vec4 o_col;"

        "layout(push_constant) uniform {"
        "   mat3 transform;"
        "} pushConstants;"

        "void main()"
        "{"
        "   o_col = fs_col;"
        "}"
}
