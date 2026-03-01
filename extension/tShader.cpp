#include <extension.h>
#include <delegate.h>
#include <mesh.h>

struct Tesfactor
{
    int outtesFactor;
    int intesFactor;
    int g;
    float time;
};

class TessellationExtension : public BaseWorker{
public:
    Mesh* mesh;
    ShaderResources* cbuffers;
    wstring hullShader;
    wstring domainShader;

    void postConstruction() override
    {
        cbuffers->createConstantBuffer("TesBuffer", sizeof(Tesfactor));
    };
    
    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables) override
    {
        // set the tessellation factor in the constant buffers
        Tesfactor Tes = {static_cast<int>(variables[L"out-tes"] / 10),
                    static_cast<int>(variables[L"in-tes"] / 10),
                0, variables[L"time"]};

        mesh->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );

        mesh->pContext->UpdateSubresource(*cbuffers->getConstBuffer("TesBuffer"), 0, NULL, &Tes, 0, 0);
        mesh->pContext->HSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("TesBuffer"));
        mesh->pContext->DSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("transformMatrix"));
        mesh->pContext->DSSetConstantBuffers(1, 1, cbuffers->getConstBuffer("TesBuffer"));
        
        mesh->pContext->HSSetShader(mesh->shaders->getHullShader(hullShader), NULL, 0);
        mesh->pContext->DSSetShader(mesh->shaders->getDomainShader(domainShader), NULL, 0);

    };

};



BaseWorker* createTessellation(Mesh* mesh, ShaderResources* resources, wstring hullShader, wstring domainShader)
{
    TessellationExtension *out = new TessellationExtension();
    out->domainShader = domainShader;
    out->hullShader = hullShader;
    out->mesh = mesh;
    out->cbuffers = resources;
    return out;
}