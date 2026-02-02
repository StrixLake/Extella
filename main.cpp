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

    ShaderResources* resource = new ShaderResources(device->pDevice);

    Mesh *grid = new Mesh("mesh/triangle.obj", shaders, device->pContext, resource, L"mesh/default_.png");
    Mesh *spot = new Mesh("mesh/spot_.obj", shaders, device->pContext, resource, L"mesh/spot_.png");
    Mesh *grass = new Mesh("mesh/grass.obj", shaders, device->pContext, resource, L"mesh/normals.png");
    grass->instanceCount = 1;

    grid->insertNextNode(grass);
    grid->insertNextNode(spot);

    resource->createConstantBuffer("transformMatrix", sizeof(XMMATRIX)*3);
    resource->createConstantBuffer("grassVariables", sizeof(float)*4);
    
    grid->extension += createTransformation(grid, resource);
    grid->extension += createVertex(grid, resource, L"GridShader");
    grid->extension += createPixel(grid, resource, L"GridShader");
    grid->extension += disableBackCulling(grid);

    spot->extension += createTransformation(spot, resource);
    spot->extension += createVertex(spot, resource, L"spot");
    spot->extension += createPixel(spot, resource, L"spot");

    grass->extension += createTransformation(grass, resource);
    grass->extension += createUnifiedShader(grass, L"grass", "vp");
    grass->extension += disableBackCulling(grass);
    grass->constBuffers = {"transformMatrix", "grassVariables"};

    Watch* stopwatch = new Watch();

    state->pDevice = device;
    state->mesh = grid;
    state->stopwatch = stopwatch;
    state->resources = resource;

    device->pRender->QueryInterface(__uuidof(IDXGISurface2), (void**)pSurface);
    device->pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void**)pDevice);

    state->variables[L"Distance"] = 100;

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
    x[0] = SysAllocString(L"Distance");
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

EXPORT void hotReload()
{
    state->shaders->HotReload();
}