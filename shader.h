#pragma once
#include <device.h>
#include <unordered_map>
#include <vector>
using std::vector;
using std::unordered_map;

enum ShaderIndex{
    cowVertex, cowPixel
};

enum ShaderType{
    vertex, pixel
};

class Shader{
public:
    Shader(DXDevice* device);

    void createShader(const wchar_t* filename, ShaderType type, ShaderIndex index);

    DXDevice* device;
    unordered_map<ShaderIndex, ID3D11VertexShader*> vertexShaders;
    unordered_map<ShaderIndex, ID3D11PixelShader*> pixelShaders;
    unordered_map<ShaderIndex, ID3DBlob*> shaderBlob;

};