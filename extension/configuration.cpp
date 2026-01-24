#include <extension.h>
// contains helper extensions
// to configure the draws
// like blending or raster state


class DisableBackFaceCulling : public BaseWorker
{

    ID3D11RasterizerState* pRasterState = NULL;

public:

    Mesh* mesh;

    void postConstruction()
    {
        D3D11_RASTERIZER_DESC desc = {};

        desc.FillMode =	D3D11_FILL_SOLID;
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
    }

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){}

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        mesh->pContext->RSSetState(pRasterState);
    }

    ~DisableBackFaceCulling()
    {
        pRasterState->Release();
    }
};


BaseWorker* disableBackCulling(Mesh* mesh)
{
    DisableBackFaceCulling* out = new DisableBackFaceCulling();
    out->mesh = mesh;
    return out;
}