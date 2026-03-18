#pragma once
#include <d3d11.h>
#include <boost/unordered_map.hpp>
#include <vector>
#include <memory>
using std::vector;
using boost::unordered_map;
#ifdef BOOST
#include <boost/container/string.hpp>
using wstring = boost::container::basic_string<wchar_t>;
using string = boost::container::basic_string<char>;
#else
#include <string>
using wstring = std::wstring;
using string = std::string;
#endif
using std::unique_ptr;
using std::pair;

template<typename T>
struct Deleter
{
    void operator()(T& comPointer) noexcept
    {
        comPointer->Release();
    }
};

enum class HLSLTypes{
    FLOAT, FLOAT2, FLOAT3, FLOAT4, MATRIX
};

struct Shader_Reflection_Desc
{
    // we need to know the name of the shader, and in the shader, the number of const buffers
    // the names of those const buffers, the slots of those const buffers
    // the names of the variables in the const buffer
    // the type of variable (float3, float2, matrix, etc)
    // we also need the texture information which includes
    // the name of the texture, the slot of that texture
    // we should also include the type of texture (2D or 3D) but right now
    // i'm only concerned about 2D textures and 3D textures can be extended later
    // Because i'm sometimes typing all stages of the pipeline in the same shader
    // we will also store the type of shader this desc struct is for like vertex or pixel

    struct ConstBuffer
    {
        struct Variable
        {
            string name;
            HLSLTypes type;
        };
        string name;
        int slot;
        vector<Variable> variables;
    };
    
    vector<ConstBuffer> constBuffers;
    // <texture name, slot>
    vector<pair<string, int>> textures;

    string shader_stage;

    // input semantics
    // <semantic, index>
    vector<pair<string, int>> input_semantics;
};

class Shader{

    unordered_map<wstring, unique_ptr<ID3D11VertexShader, Deleter<ID3D11VertexShader*>>> vertexShaders;
    unordered_map<wstring, unique_ptr<ID3D11PixelShader, Deleter<ID3D11PixelShader*>>> pixelShaders;
    unordered_map<wstring, unique_ptr<ID3D11HullShader, Deleter<ID3D11HullShader*>>> hullShaders;
    unordered_map<wstring, unique_ptr<ID3D11DomainShader, Deleter<ID3D11DomainShader*>>> domainShaders;
    unordered_map<wstring, unique_ptr<ID3D11GeometryShader, Deleter<ID3D11GeometryShader*>>> geometryShaders;
    unordered_map<wstring, unique_ptr<ID3DBlob, Deleter<ID3DBlob*>>> vertexShaderBlob;
    unordered_map<ID3DBlob*, Shader_Reflection_Desc> reflections;

    
    public:
    Shader_Reflection_Desc reflect(ID3DBlob* shaderBlob);
    Shader(ID3D11Device* device);

    ID3D11VertexShader* getVertexShader(wstring ShaderFileName);
    ID3D11PixelShader* getPixelShader(wstring ShaderFileName);
    ID3D11HullShader* getHullShader(wstring ShaderFileName);
    ID3D11DomainShader* getDomainShader(wstring ShaderFileName);
    ID3D11GeometryShader* getGeometryShader(wstring ShaderFileName);
    ID3DBlob* getVertexShaderBlob(wstring ShaderFileName);

    void HotReload();

    ID3D11Device* pDevice;
};