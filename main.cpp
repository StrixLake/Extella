#define EXPORT extern "C" __declspec(dllexport)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Windows.h>
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <watch.h>
#include <state.h>
#include <resources.h>

State* state;

class Extension : public BaseWorker{
public:
    Extension() = delete;
    Extension(Mesh* meshPointer, ShaderResources* cbuf) : mesh(meshPointer), cbuffers(cbuf) {}

    void extensionWork(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables){
    
        float xoffset = variables[L"Position X"] /20;
        float yoffset = variables[L"Position Y"] /20;
        float zoffset = variables[L"Position Z"] /10;
        camera[0] *= DirectX::XMMatrixRotationX(variables[L"CameraY"]/100);
        camera[0] *= DirectX::XMMatrixRotationY(variables[L"CameraX"]/100);
        camera[0] *= DirectX::XMMatrixTranslation(xoffset, yoffset, zoffset);

        camera[0] = DirectX::XMMatrixTranspose(camera[0]);
        camera[1] = DirectX::XMMatrixTranspose(camera[1]);
        camera[2] = DirectX::XMMatrixTranspose(camera[2]);

        mesh->pContext->UpdateSubresource(*cbuffers->getConstBuffer("transformMatrix"), 0, NULL, camera.data(), 0,0);

        // set the tessellation factor in the constant buffers
        int Tes[4] = {static_cast<int>(variables[L"out-tes"] / 10),
                    static_cast<int>(variables[L"in-tes"] / 10)};

        mesh->pContext->UpdateSubresource(mesh->tesBuffer, 0, NULL, Tes, 0, 0);
    }

    Mesh* mesh;
    ShaderResources* cbuffers;
};


EXPORT void InitializeRenderer(IDXGISurface2** pSurface, IDXGIDevice2** pDevice){
    

    DXDevice *device = new DXDevice();
    device->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
    //device->CreateSwap(0);
    device->CreateViews();
    device->SetTargets();

    Shader *shaders = new Shader(device->pDevice);

    state = new State();

    state->shaders = shaders;

    Mesh *plane = new Mesh("mesh/plane.obj", shaders, device->pContext, obj, L"mesh/default_.png");
    ShaderResources* resource = new ShaderResources(device->pDevice);
    resource->createConstantBuffer("transformMatrix", sizeof(XMMATRIX)*3);
    
    device->pContext->DSSetConstantBuffers(0, 1, resource->getConstBuffer("transformMatrix"));
    plane->extension = new Extension(plane, resource);
    Watch* stopwatch = new Watch();


    state->pDevice = device;
    state->mesh = plane;
    state->stopwatch = stopwatch;
    state->resources = resource;

    device->pRender->QueryInterface(__uuidof(IDXGISurface2), (void**)pSurface);
    device->pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void**)pDevice);

    return;
}


void renderer(State* state);

EXPORT void render(){
    renderer(state);
}

EXPORT void release() {
    delete state;
}

EXPORT SAFEARRAY* getGlobalVariables(){
    int elements = 6;
    BSTR *x = (BSTR*)CoTaskMemAlloc(sizeof(BSTR)*elements);
    x[0] = SysAllocString(L"Angle");
    x[1] = SysAllocString(L"Position X");
    x[2] = SysAllocString(L"Position Y");
    x[3] = SysAllocString(L"Position Z");
    x[4] = SysAllocString(L"in-tes");
    x[5] = SysAllocString(L"out-tes");

    SAFEARRAYBOUND bound = {};
    bound.cElements = elements;
    bound.lLbound = 0;

    SAFEARRAY* ar = (SAFEARRAY*)CoTaskMemAlloc(sizeof(SAFEARRAY));
    ar->cbElements = sizeof(BSTR);
    ar->cDims = 1;
    ar->rgsabound[0] = bound;
    ar->fFeatures = FADF_BSTR;
    ar->pvData = x;

    return ar;
}

EXPORT void setVariable(BSTR variable, float value){
    // since BSTR is wchar_t*
    // we can use it directly as the key
    if(state != NULL) state->variables[variable] = value;
}

EXPORT float getVariable(BSTR variable)
{
    return state->variables[variable];
}