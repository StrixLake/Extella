#include <mesh.h>
#include <resources.h>

class TransformationExtenstion : public BaseWorker{

public:
    Mesh* mesh;
    ShaderResources* cbuffers;

    void postConstruction(){};

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        float xoffset = variables[L"Position X"] /20;
        float yoffset = variables[L"Position Y"] /20;
        float zoffset = variables[L"Position Z"] /10;
        camera[0] *= DirectX::XMMatrixRotationX(variables[L"CameraY"]/100);
        camera[0] *= DirectX::XMMatrixRotationY(variables[L"CameraX"]/100);
        camera[0] *= DirectX::XMMatrixTranslation(xoffset, yoffset, zoffset);

    };

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){};
};

class TessellationExtension : public BaseWorker{

public:
    Mesh* mesh;
    ShaderResources* cbuffers;

    void postConstruction(){};

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        // set the tessellation factor in the constant buffers
        int Tes[4] = {static_cast<int>(variables[L"out-tes"] / 10),
                    static_cast<int>(variables[L"in-tes"] / 10)};

        mesh->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
        mesh->pContext->UpdateSubresource(*cbuffers->getConstBuffer("TesBuffer"), 0, NULL, Tes, 0, 0);
        mesh->pContext->HSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("TesBuffer"));

        mesh->pContext->DSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("transformMatrix"));
    };

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){};

};

BaseWorker* createTransformation(Mesh* mesh, ShaderResources* resources)
{
    TransformationExtenstion *out = new TransformationExtenstion();
    out->mesh = mesh;
    out->cbuffers = resources;
    return out;
}

BaseWorker* createTessellation(Mesh* mesh, ShaderResources* resources){
    TessellationExtension *out = new TessellationExtension();
    out->mesh = mesh;
    out->cbuffers = resources;
    return out;
}