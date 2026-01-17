#include <extension.h>
#include <delegate.h>
#include <mesh.h>

class GeometryShader : public BaseWorker
{
public:
    Mesh* mesh;
    ShaderResources* cbuffers;
    wstring geometryShader;

    void postConstruction(){}

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
    {
        mesh->pContext->GSSetConstantBuffers(0, 1, cbuffers->getConstBuffer("transformMatrix"));
        mesh->pContext->GSSetShader(mesh->shaders->getGeometryShader(geometryShader), NULL, 0);
    }

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){}
};

BaseWorker* createGeometry(Mesh* mesh, ShaderResources* resources, wstring geometryShader)
{
    GeometryShader* out = new GeometryShader();
    out->geometryShader = geometryShader;
    out->mesh = mesh;
    out->cbuffers = resources;
    return out;
}