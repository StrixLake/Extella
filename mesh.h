#pragma once
#include <device.h>
#include <shader.h>

enum FileType{
    obj, stl
};

class Mesh{
public:
    Mesh() = delete;
    Mesh(const char* filename, Shader* shader, FileType type, const wchar_t* textureFile);

    void Draw(array<XMMATRIX,3> camera, float time);

    uint32_t triangleCount;
    ID3D11Buffer* pVertices = NULL;
    ID3D11Buffer* pNormals = NULL;
    ID3D11Buffer* pIndices = NULL;
    ID3D11Buffer* pTexCoords = NULL;

    ID3D11SamplerState *pSampler = NULL;
    ID3D11Texture2D *pTexture = NULL;
    ID3D11ShaderResourceView *pTextureView = NULL;
    ID3D11InputLayout* pLayout = NULL;
    
    ID3D11Buffer *transformBuffer = NULL;
    ID3D11Buffer *tesBuffer = NULL;
    

    Shader* shaders;
    DXDevice* device;

    BaseWorker* extension = NULL;

    Mesh* next = NULL;

    ~Mesh();
};
