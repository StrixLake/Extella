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
#include <extension.h>

State* state;

BaseWorker* createTransformation(Mesh* mesh, ShaderResources* resource);
BaseWorker* createTessellation(Mesh* mesh, ShaderResources* resource);


EXPORT void InitializeRenderer(IDXGISurface2** pSurface, IDXGIDevice2** pDevice){
    

    DXDevice *device = new DXDevice();
    //device->CreateSwap(0);
    device->CreateViews();
    device->SetTargets();

    Shader *shaders = new Shader(device->pDevice);

    state = new State();

    state->shaders = shaders;

    Mesh *plane2 = new Mesh("mesh/plane.obj", shaders, device->pContext, obj, L"mesh/default_.png");
    ShaderResources* resource = new ShaderResources(device->pDevice);
    resource->createConstantBuffer("transformMatrix", sizeof(XMMATRIX)*3);
    plane2->cbuffers = resource;
    
    plane2->extension += createTransformation(plane2, resource);
    plane2->extension += createVertex(plane2, resource, L"shaders/vshader.cso");
    plane2->extension += createTessellation(plane2, resource, L"shaders/hshader.cso", L"shaders/dshader2.cso");
    plane2->extension += createGeometry(plane2, resource, L"");
    plane2->extension += createPixel(plane2, resource, L"shaders/pshader2.cso");

    Watch* stopwatch = new Watch();


    state->pDevice = device;
    state->mesh = plane2;
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