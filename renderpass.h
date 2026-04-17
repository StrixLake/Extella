#pragma once
#include <shader.h>
#include <materials.h>
#include <mesh.h>
#include <pipeline.h>
using std::tuple;
using std::get;
typedef std::function<void(ID3D11Buffer*, ID3D11DeviceContext*)> cbufLambda;

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

    // <slot, const buffer to use, array of pointer to float in global variables>
    tuple<int, ID3D11Buffer*, vector<const float*>> vertexHostVar;
    tuple<int, ID3D11Buffer*, vector<const float*>> hullHostVar;
    tuple<int, ID3D11Buffer*, vector<const float*>> domainHostVar;
    tuple<int, ID3D11Buffer*, vector<const float*>> geometryHostVar;
    tuple<int, ID3D11Buffer*, vector<const float*>> pixelHostVar;

    // general purpose constant buffers other than host variables
    // <slot, buffer pointer, lambda to setup the buffer>
    vector<tuple<int, ID3D11Buffer*, cbufLambda>> vertexConstBuffers;
    vector<tuple<int, ID3D11Buffer*, cbufLambda>> hullConstBuffers;
    vector<tuple<int, ID3D11Buffer*, cbufLambda>> domainConstBuffers;
    vector<tuple<int, ID3D11Buffer*, cbufLambda>> geometryConstBuffers;
    vector<tuple<int, ID3D11Buffer*, cbufLambda>> pixelConstBuffers;

    // the pointers needed for input layout
    uniq_com_ptr<ID3D11InputLayout> pInputLayout;
    vector<ID3D11Buffer*> vertex_buffers;
    vector<unsigned int> stride;

    // store a reference to world rotation from unordered_map
    const float &rotationX, &rotationY;

public:
    RenderPass(GPUMesh* mesh, PipeLine* pipeline, Material* material, ResourceManager* manager, Shader* shaderManager, const unordered_map<wstring, float>& global_variables, ID3D11Device* pDevice);
    
    void execute(ID3D11DeviceContext* pContext);
    
    cbufLambda getCBufferStruct(string bufferName, const unordered_map<wstring, float>& global_variables);
};