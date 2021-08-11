#include "ShaderBuilder.h"
#include <spdlog/fmt/fmt.h>

namespace kuas
{
    ShaderBuilder::ShaderBuilder(ShaderType type) :
        m_type(type)
    {
    }
    
    ShaderBuilder& ShaderBuilder::beginShader()
    {
        m_shaderStr.reserve(4096);
        return *this;
    }
    
    ShaderBuilder& ShaderBuilder::addDefinition(const char* name, const char* def)
    {
        m_shaderStr += fmt::format("#define {:s} {:s}\n", name, def);
        return *this;
    }

    ShaderBuilder& ShaderBuilder::addDefinition(const char* name, const char* args, const char* def)
    {
        m_shaderStr += fmt::format("#define {:s}({:s})\\\n{:s}", name, args, def);
        return *this;
    }
    
    ShaderBuilder& ShaderBuilder::addFunction(const char* name, ShaderBasicType returnType, const char* args, const char* body)
    {
        const char* retType = nullptr;

        switch (returnType) {
            case ShaderBasicType::Int:
                retType = "INT";
                break;
            case ShaderBasicType::Int2:
                retType = "INT2";
                break;
            case ShaderBasicType::Int3:
                retType = "INT3";
                break;
            case ShaderBasicType::Int4:
                retType = "INT4";
                break;
            case ShaderBasicType::Uint:
                retType = "UINT";
                break;
            case ShaderBasicType::Uint2:
                retType = "UINT2";
                break;
            case ShaderBasicType::Uint3:
                retType = "UINT3";
                break;
            case ShaderBasicType::Uint4:
                retType = "UINT4";
                break;
            case ShaderBasicType::Float:
                retType = "FLOAT";
                break;
            case ShaderBasicType::Float2:
                retType = "FLOAT2";
                break;
            case ShaderBasicType::Float3:
                retType = "FLOAT3";
                break;
            case ShaderBasicType::Float4:
                retType = "FLOAT4";
                break;
        }

        m_shaderStr += fmt::format("{:s} {:s}({:s}) {{ {:s} }}", retType, name, args, body);
        return *this;
    }
    
    ShaderBuilder& ShaderBuilder::entryPoint(const char* body)
    {
        return *this;
    }
    
    ShaderBuilder& ShaderBuilder::endShader()
    {
        return *this;
    }
    
    void ShaderBuilder::buildGLSL(const char* ver)
    {
        static const char* headerCode =
            "#define INT int\n"
            "#define INT2 int2\n"
            "#define INT3 int3\n"
            "#define INT4 int4\n"
            "#define UINT uint\n"
            "#define UINT2 uint2\n"
            "#define UINT3 uint3\n"
            "#define UINT4 uint4\n"
            "#define FLOAT float\n"
            "#define FLOAT2 vec2\n"
            "#define FLOAT3 vec3\n"
            "#define FLOAT4 vec4\n"
            "#define TEX2D sampler2D\n"
            "#define TEX_SMP(x, uv) \\\ntexture(x, (uv))\n";
    }

    void ShaderBuilder::buildHLSL()
    {
    }
}
