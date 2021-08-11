#pragma once

#include <string>
#include <unordered_map>

namespace kuas
{
    enum class ShaderType
    {
        VertexShader,
        PixelShader
    };

    enum class ShaderBasicType
    {
        Int,
        Int2,
        Int3,
        Int4,
        Uint,
        Uint2,
        Uint3,
        Uint4,
        Float,
        Float2,
        Float3,
        Float4,
    };

    struct ShaderVtxAttribute
    {
        const char* name;
        ShaderBasicType returnType;
    };

    class ShaderBuilder
    {
    public:
        ShaderBuilder(ShaderType type);

        ShaderBuilder& beginShader();
        ShaderBuilder& addDefinition(const char* name, const char* def);
        ShaderBuilder& addDefinition(const char* name, const char* args, const char* def);
        ShaderBuilder& addFunction(const char* name, ShaderBasicType returnType, const char* args, const char* body);
        ShaderBuilder& entryPoint(const char* body);
        ShaderBuilder& endShader();

        void buildGLSL(const char* ver);
        void buildHLSL();

    private:
        std::string m_shaderStr;
        std::string m_entryPointStr;
        std::vector<std::string> m_vtxAttr;
        ShaderType m_type;
    };
}
