#pragma once
#include <d3d11.h>
#include <shader.h>
#include <DirectXMath.h>
#include <array>
#include <delegate.h>
#include <resources.h>
using std::array;
using DirectX::XMMATRIX;

enum FileType{
    obj, stl
};

class Mesh{
public:
    Mesh() = delete;
    Mesh(const char* filename, Shader* shader, ID3D11DeviceContext* context, FileType type, const wchar_t* textureFile);

    void Draw(array<XMMATRIX,3> camera, unordered_map<wstring, float> &variables);

    void createInstanceBuffer(vector<float> &instanceData);

    uint32_t triangleCount;
    uint32_t instanceCount = 0;
    ID3D11Buffer* pVertices = NULL;
    ID3D11Buffer* pNormals = NULL;
    ID3D11Buffer* pIndices = NULL;
    ID3D11Buffer* pTexCoords = NULL;
    ID3D11Buffer* pInstanceData = NULL;

    ID3D11SamplerState *pSampler = NULL;
    ID3D11Texture2D *pTexture = NULL;
    ID3D11ShaderResourceView *pTextureView = NULL;
    
    ShaderResources* cbuffers;
    Shader* shaders;
    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;

    Delegate extension;

    Mesh* next = NULL;

    ~Mesh();
};
