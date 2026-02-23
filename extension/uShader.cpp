#include <extension.h>
#include <delegate.h>
#include <mesh.h>

class UnifiedShader : public BaseWorker
{
    friend BaseWorker* createUnifiedShader(Mesh* mesh, wstring ushader, string stages, vector<wstring> constVariables, vector<string> constBuffers);

    Mesh* mesh;
    vector<wstring> constVariables;
    vector<string> constBuffers;
    ShaderResources* cbuffers;
    wstring unifiedShader;
    ID3D11InputLayout* pLayout = NULL;
    string stages = "vp";

public:

    void postConstruction()
    {
        D3D11_INPUT_ELEMENT_DESC layout[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                            0, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
                                            1, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                                 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        mesh->pDevice->CreateInputLayout(layout, 3, mesh->shaders->getVertexShaderBlob(unifiedShader)->GetBufferPointer(), 
                                                   mesh->shaders->getVertexShaderBlob(unifiedShader)->GetBufferSize(), &pLayout);
    }

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {

        mesh->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        mesh->pContext->IASetInputLayout(pLayout);
        UINT offset = 0;
        UINT stride = sizeof(DirectX::XMFLOAT3);
        mesh->pContext->IASetVertexBuffers(0, 1, &mesh->pVertices, &stride, &offset);
        stride = sizeof(DirectX::XMFLOAT2);
        mesh->pContext->IASetVertexBuffers(1, 1, &mesh->pTexCoords, &stride, &offset);
        stride = sizeof(DirectX::XMFLOAT3);
        mesh->pContext->IASetVertexBuffers(2, 1, &mesh->pNormals, &stride, &offset);

        mesh->pContext->IASetIndexBuffer(mesh->pIndices, DXGI_FORMAT_R32_UINT, 0);


        // set the shader for all the stages from the "stages" string
        for(char stage : stages)
        {
            switch(stage)
            {
                case 'v':
                    mesh->pContext->VSSetShader(mesh->shaders->getVertexShader(unifiedShader), NULL, 0);
                    break;
                case 'p':
                    mesh->pContext->PSSetSamplers(0, 1, &mesh->pSampler);
                    mesh->pContext->PSSetShaderResources(0, 1, &mesh->pTextureView);
                    mesh->pContext->PSSetShader(mesh->shaders->getPixelShader(unifiedShader), NULL, 0);
                    break;
                case 't':
                    mesh->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
                    mesh->pContext->HSSetShader(mesh->shaders->getHullShader(unifiedShader), NULL, 0);
                    mesh->pContext->DSSetShader(mesh->shaders->getDomainShader(unifiedShader), NULL, 0);
                    break;
                case 'g':
                    mesh->pContext->GSSetShader(mesh->shaders->getGeometryShader(unifiedShader), NULL, 0);
                    break;
            }   
        }

        // set the const buffers specified in the vector inside mesh object
        // for all the stages

        vector<ID3D11Buffer*> buffers;

        for(string cbuffer : constBuffers)
        {
            buffers.push_back(*cbuffers->getConstBuffer(cbuffer));
        }

        mesh->pContext->VSSetConstantBuffers(0, buffers.size(), buffers.data());
        mesh->pContext->HSSetConstantBuffers(0, buffers.size(), buffers.data());
        mesh->pContext->DSSetConstantBuffers(0, buffers.size(), buffers.data());
        mesh->pContext->GSSetConstantBuffers(0, buffers.size(), buffers.data());
        mesh->pContext->PSSetConstantBuffers(0, buffers.size(), buffers.data());
        
        if (constVariables.size() == 0) return;
        // if constVariables is non empty
        // in the next const buffer slot, set the const buffer for variables
        int nextSlot = buffers.size();
        // check the size in bytes of the variables vector
        size_t size = constVariables.size()*sizeof(float); // since all variables are floats
        // get the nearest multiple of 16
        size = ((size + 15) / 16)*16;
        // get the const buffer of that size
        ID3D11Buffer* vBuffer = *cbuffers->getConstBuffer(size);
        // load the variables in a vector
        vector<float> constVars;
        constVars.reserve(size/sizeof(float));
        for(wstring var : constVariables)
        {
            constVars.push_back(variables[var]);
        }
        // update the vBuffer for all those variables
        mesh->pContext->UpdateSubresource(vBuffer, 0, NULL, constVars.data(), 0, 0);
        // bind to the next slot
        mesh->pContext->VSSetConstantBuffers(nextSlot, 1, &vBuffer);
        mesh->pContext->HSSetConstantBuffers(nextSlot, 1, &vBuffer);
        mesh->pContext->DSSetConstantBuffers(nextSlot, 1, &vBuffer);
        mesh->pContext->GSSetConstantBuffers(nextSlot, 1, &vBuffer);
        mesh->pContext->PSSetConstantBuffers(nextSlot, 1, &vBuffer);

    }

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){}

    ~UnifiedShader()
    {
        if(pLayout != NULL) pLayout->Release();
    }
};


BaseWorker* createUnifiedShader(Mesh* mesh, wstring ushader, string stages, vector<wstring> constVariables, vector<string> constBuffers)
{
    UnifiedShader* out = new UnifiedShader();
    out->unifiedShader = ushader;
    out->mesh = mesh;
    out->cbuffers = mesh->cbuffers;
    out->stages = stages;
    out->constBuffers = constBuffers;
    out->constVariables = constVariables;

    return out;
}