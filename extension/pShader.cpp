#include <extension.h>
#include <delegate.h>
#include <mesh.h>

class PixelShader : public BaseWorker
{
public:
    Mesh* mesh;
    ShaderResources* cbuffers;
    wstring pixelShader;

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables) override
    {
        mesh->pContext->PSSetShader(mesh->shaders->getPixelShader(pixelShader), NULL, 0);

        mesh->pContext->PSSetSamplers(0, 1, &mesh->pSampler);
        mesh->pContext->PSSetShaderResources(0, 1, &mesh->pTextureView);
        mesh->pContext->PSSetConstantBuffers(1, 1, cbuffers->getConstBuffer("grassVariables"));
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
