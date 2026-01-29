#include <extension.h>

class TransformationExtenstion : public BaseWorker{

public:
    void postConstruction(){};

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        float xoffset = variables[L"Position X"] /20;
        float yoffset = variables[L"Position Y"] /20;
        float zoffset = variables[L"Position Z"] /10;
        camera[0] *= DirectX::XMMatrixRotationY(variables[L"CameraX"]/100);
        camera[0] *= DirectX::XMMatrixRotationX(variables[L"CameraY"]/100);
        camera[0] *= DirectX::XMMatrixTranslation(xoffset, yoffset, zoffset);

    };

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){};
};

BaseWorker* createTransformation(Mesh* mesh, ShaderResources* resources)
{
    TransformationExtenstion *out = new TransformationExtenstion();
    return out;
}

class VertexShader : public BaseWorker
{
public:
    Mesh* mesh;
    ShaderResources* cbuffers;
    wstring vertexShader;
    ID3D11InputLayout* pLayout = NULL;

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

        mesh->pDevice->CreateInputLayout(layout, 3, mesh->shaders->getVertexShaderBlob(vertexShader)->GetBufferPointer(), 
                                                   mesh->shaders->getVertexShaderBlob(vertexShader)->GetBufferSize(), &pLayout);
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
        mesh->pContext->VSSetShader(mesh->shaders->getVertexShader(vertexShader), NULL, 0);
        mesh->pContext->VSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("transformMatrix"));

    }

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){};

    ~VertexShader()
    {
        if(pLayout != NULL) pLayout->Release();
    }
};

BaseWorker* createVertex(Mesh* mesh, ShaderResources* resources, wstring vertexShader)
{
    VertexShader* out = new VertexShader();
    out->vertexShader = vertexShader;
    out->mesh = mesh;
    out->cbuffers = resources;
    return out;
}


class VertexInstanced : public BaseWorker
{
public:
    
    Mesh* mesh;
    ShaderResources* cbuffers;
    wstring vertexShader;
    ID3D11InputLayout* pLayout = NULL;
    int hr;

    void postConstruction()
    {
        D3D11_INPUT_ELEMENT_DESC layout[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                            0, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
                                            1, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                                 2, 0, 
                                                 D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,
                                                3, 0, 
                                                D3D11_INPUT_PER_INSTANCE_DATA, 1}};

        hr = mesh->pDevice->CreateInputLayout(layout, 4, mesh->shaders->getVertexShaderBlob(vertexShader)->GetBufferPointer(), 
                                                   mesh->shaders->getVertexShaderBlob(vertexShader)->GetBufferSize(), &pLayout);
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
        stride = sizeof(DirectX::XMFLOAT2);
        mesh->pContext->IASetVertexBuffers(3, 1, &mesh->pInstanceData, &stride, &offset);

        mesh->pContext->IASetIndexBuffer(mesh->pIndices, DXGI_FORMAT_R32_UINT, 0);
        mesh->pContext->VSSetShader(mesh->shaders->getVertexShader(vertexShader), NULL, 0);
        mesh->pContext->VSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("transformMatrix"));
    }

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {}

    ~VertexInstanced()
    {
        if(pLayout != NULL) pLayout->Release();
    }
};

BaseWorker* createVertexInstanced(Mesh* mesh, ShaderResources* resources, wstring vertexShader)
{
    VertexInstanced* out = new VertexInstanced();
    out->mesh = mesh;
    out->vertexShader = vertexShader;
    out->cbuffers = resources;
    return out;
}