#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <vector>
#include <string>
using std::vector;
using std::unordered_map;
using std::wstring;

class Shader{

    unordered_map<wstring, ID3D11VertexShader*> vertexShaders;
    unordered_map<wstring, ID3D11PixelShader*> pixelShaders;
    unordered_map<wstring, ID3D11HullShader*> hullShaders;
    unordered_map<wstring, ID3D11DomainShader*> domainShaders;
    unordered_map<wstring, ID3D11GeometryShader*> geometryShaders;
    unordered_map<wstring, ID3DBlob*> shaderBlob;

public:
    Shader(ID3D11Device* device);

    ID3D11VertexShader* getVertexShader(wstring ShaderFileName);
    ID3D11PixelShader* getPixelShader(wstring ShaderFileName);
    ID3D11HullShader* getHullShader(wstring ShaderFileName);
    ID3D11DomainShader* getDomainShader(wstring ShaderFileName);
    ID3D11GeometryShader* getGeometryShader(wstring ShaderFileName);
    ID3DBlob* getShaderBlob(wstring ShaderFileName);

    ID3D11Device* pDevice;

    ~Shader();
};