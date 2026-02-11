#pragma once
#include <d3d11.h>
#include <boost/unordered_map.hpp>
#include <vector>
#include <memory>
#include <boost/container/string.hpp>
using std::vector;
using boost::unordered_map;
using wstring = boost::container::basic_string<wchar_t>;
using std::unique_ptr;

template<typename T>
struct Deleter
{
    void operator()(T& comPointer) noexcept
    {
        comPointer->Release();
    }
};

class Shader{

    unordered_map<wstring, unique_ptr<ID3D11VertexShader, Deleter<ID3D11VertexShader*>>> vertexShaders;
    unordered_map<wstring, unique_ptr<ID3D11PixelShader, Deleter<ID3D11PixelShader*>>> pixelShaders;
    unordered_map<wstring, unique_ptr<ID3D11HullShader, Deleter<ID3D11HullShader*>>> hullShaders;
    unordered_map<wstring, unique_ptr<ID3D11DomainShader, Deleter<ID3D11DomainShader*>>> domainShaders;
    unordered_map<wstring, unique_ptr<ID3D11GeometryShader, Deleter<ID3D11GeometryShader*>>> geometryShaders;
    unordered_map<wstring, unique_ptr<ID3DBlob, Deleter<ID3DBlob*>>> vertexShaderBlob;

public:
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