#include <extension.h>
#include <delegate.h>
#include <mesh.h>

class PixelShader : public BaseWorker
{
public:
    Mesh* mesh;
    ShaderResources* cbuffers;
    wstring pixelShader;

    void postConstruction(){}

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        mesh->pContext->PSSetShader(mesh->shaders->getPixelShader(pixelShader), NULL, 0);

        mesh->pContext->PSSetSamplers(0, 1, &mesh->pSampler);
        mesh->pContext->PSSetShaderResources(0, 1, &mesh->pTextureView);
        mesh->pContext->PSSetConstantBuffers(1, 1, cbuffers->getConstBuffer("grassVariables"));
    }

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){}
};

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

BaseWorker* createPixel(Mesh* mesh, ShaderResources* resources, wstring pixelShader)
{
    PixelShader* out = new PixelShader();
    out->pixelShader = pixelShader;
    out->mesh = mesh;
    out->cbuffers = resources;
    return out;
}

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