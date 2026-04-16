#pragma once
#include <shader.h>
#include <materials.h>
#include <mesh.h>
#include <pipeline.h>


class RenderPass
{

    Material* material;
    GPUMesh* mesh;
    PipeLine* pipeline;

    vector<pair<int, uniq_com_ptr<ID3D11ShaderResourceView>>> vertexTextures;
    vector<pair<int, uniq_com_ptr<ID3D11ShaderResourceView>>> hullTextures;
    vector<pair<int, uniq_com_ptr<ID3D11ShaderResourceView>>> domainTextures;
    vector<pair<int, uniq_com_ptr<ID3D11ShaderResourceView>>> geometryTextures;
    vector<pair<int, uniq_com_ptr<ID3D11ShaderResourceView>>> pixelTextures;

    // <const buffer to use, array of pointer to float in global variables>, slot is always 1
    pair<ID3D11Buffer*, vector<const float*>> vertexHostVar;
    pair<ID3D11Buffer*, vector<const float*>> hullHostVar;
    pair<ID3D11Buffer*, vector<const float*>> domainHostVar;
    pair<ID3D11Buffer*, vector<const float*>> geometryHostVar;
    pair<ID3D11Buffer*, vector<const float*>> pixelHostVar;

    // slot is always 0 for all stages
    ID3D11Buffer* transformationBuffer;

    // slot is always 7 for pixel shader
    ID3D11Buffer* materialBuffer = NULL;

    // the pointers needed for input layout
    uniq_com_ptr<ID3D11InputLayout> pInputLayout;
    vector<ID3D11Buffer*> vertex_buffers;
    vector<unsigned int> stride;

    // store a reference to world rotation from unordered_map
    const float &rotationX, &rotationY;

public:
    RenderPass(GPUMesh* mesh, PipeLine* pipeline, Material* material, ResourceManager* manager, Shader* shaderManager, const unordered_map<wstring, float>& global_variables, ID3D11Device* pDevice);
    
    void execute(const DirectX::XMMATRIX& ViewProjMatrix, ID3D11DeviceContext* pContext);

};