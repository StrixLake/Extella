#pragma once
#include <device.h>
#include <DirectXMath.h>
#include <array>
#include <shader.h>
using DirectX::XMMATRIX;
using std::array;

enum FileType{
    obj, stl
};

class Mesh{
public:
    Mesh() = delete;
    Mesh(const char* filename, Shader* shader, FileType type);

    virtual void Draw(array<XMMATRIX,3> camera, float time) = 0;

    uint32_t triangleCount;
    ID3D11Buffer* pVertices = NULL;
    ID3D11Buffer* pNormals = NULL;
    ID3D11Buffer* pIndices = NULL;
    Shader* shaders;
    DXDevice* device;

    Mesh* next = NULL;

    ~Mesh();
};
