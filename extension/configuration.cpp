#include <extension.h>
// contains helper extensions
// to configure the draws
// like blending or raster state


class RasterMode : public BaseWorker
{
    friend BaseWorker* rasterMode(Mesh* mesh, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode);
    ID3D11RasterizerState* pRasterState = NULL;

public:

    Mesh* mesh;

    void postConstruction(){}

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){}

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        mesh->pContext->RSSetState(pRasterState);
    }

    ~RasterMode()
    {
        pRasterState->Release();
    }
};


BaseWorker* rasterMode(Mesh* mesh, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode)
{
    RasterMode* out = new RasterMode();

    D3D11_RASTERIZER_DESC desc = {};

    desc.FillMode =	fill_mode;
    desc.CullMode =	cull_mode;
    desc.FrontCounterClockwise = FALSE;
    desc.DepthBias = 0;
    desc.SlopeScaledDepthBias = 0.0f;
    desc.DepthBiasClamp = 0.0f;
    desc.DepthClipEnable = TRUE;
    desc.ScissorEnable = FALSE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;
    
    mesh->pDevice->CreateRasterizerState(&desc, &out->pRasterState);

    out->mesh = mesh;
    return out;
}

class BlendStateT0 : public BaseWorker
{
public:
    Mesh* mesh;

    D3D11_BLEND_DESC blendDesc = {};

    ID3D11BlendState* pBlendState = NULL;

    void postConstruction()
    {
        mesh->pDevice->CreateBlendState(&blendDesc, &pBlendState);
    }

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        mesh->pContext->OMSetBlendState(pBlendState, NULL, 0xFFFFFFFF);
    }

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){}

    ~BlendStateT0()
    {
        pBlendState->Release();
    }
};

BaseWorker* createBlendState(Mesh* mesh)
{
    BlendStateT0 *out = new BlendStateT0();
    out->mesh = mesh;

    D3D11_BLEND_DESC blend = {};
    blend.IndependentBlendEnable = false;
    blend.RenderTarget[0].BlendEnable = true;
    blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    
    out->blendDesc = blend;
    return out;
}