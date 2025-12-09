#pragma once
#include <device.h>
#include <unordered_map>
#include <vector>
#include <string>
using std::vector;
using std::unordered_map;
using std::wstring;

class Shader{

    unordered_map<wstring, ID3D11VertexShader*> vertexShaders;
    unordered_map<wstring, ID3D11PixelShader*> pixelShaders;
    unordered_map<wstring, ID3DBlob*> shaderBlob;

public:
    Shader(DXDevice* device);

    ID3D11VertexShader* getVertexShader(wstring ShaderFileName);
    ID3D11PixelShader* getPixelShader(wstring ShaderFileName);
    ID3DBlob* getShaderBlob(wstring ShaderFileName);

    DXDevice* device;

    ~Shader();
};