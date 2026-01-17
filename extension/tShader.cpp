#include <extension.h>
#include <delegate.h>
#include <mesh.h>

class TessellationExtension : public BaseWorker{
public:
    Mesh* mesh;
    ShaderResources* cbuffers;
    ID3D11RasterizerState* pRasterState = NULL;
    wstring hullShader;
    wstring domainShader;

    void postConstruction(){
        
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode =	D3D11_FILL_WIREFRAME;
        desc.CullMode =	D3D11_CULL_NONE;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthBias = 0;
        desc.SlopeScaledDepthBias = 0.0f;
        desc.DepthBiasClamp = 0.0f;
        desc.DepthClipEnable = TRUE;
        desc.ScissorEnable = FALSE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        
        mesh->pDevice->CreateRasterizerState(&desc, &pRasterState);

        cbuffers->createConstantBuffer("TesBuffer", sizeof(int)*4);
    };
    
    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {

        mesh->pContext->RSSetState(pRasterState);
        // set the tessellation factor in the constant buffers
        int Tes[4] = {static_cast<int>(variables[L"out-tes"] / 10),
                    static_cast<int>(variables[L"in-tes"] / 10)};

        mesh->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );

        mesh->pContext->UpdateSubresource(*cbuffers->getConstBuffer("TesBuffer"), 0, NULL, Tes, 0, 0);
        mesh->pContext->HSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("TesBuffer"));
        mesh->pContext->DSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("transformMatrix"));
        
        mesh->pContext->HSSetShader(mesh->shaders->getHullShader(hullShader), NULL, 0);
        mesh->pContext->DSSetShader(mesh->shaders->getDomainShader(domainShader), NULL, 0);

    };

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){};

    ~TessellationExtension()
    {
        if(pRasterState != NULL) pRasterState->Release();
    }

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