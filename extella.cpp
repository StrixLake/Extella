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
#include <query.h>
#include <extella.h>

State* state;

BaseWorker* createTransformation(Mesh* mesh, ShaderResources* resource);
BaseWorker* createTessellation(Mesh* mesh, ShaderResources* resource);


EXPORT void InitializeRenderer(IDXGISurface2** pSurface, IDXGIDevice2** pDevice){

    DXDevice *device = new DXDevice();
    //device->CreateSwap(0);
    device->CreateViews();
    device->SetTargets();

    Query* query = new Query(device->pDevice, device->pContext);

    Shader *shaders = new Shader(device->pDevice);

    ShaderResources* resource = new ShaderResources(device->pDevice);
    
    Mesh *grid = new Mesh("mesh/triangle.obj", shaders, device->pContext, resource, L"mesh/default_.png");
    //Mesh *spot = new Mesh("mesh/spot_.obj", shaders, device->pContext, resource, L"mesh/spot_.png");
    Mesh *grass = new Mesh("mesh/grass.obj", shaders, device->pContext, resource, L"mesh/normals.png");
    grass->instanceCount = 100000;
    
    grid->insertNextNode(grass);
    //grid->insertNextNode(spot);
    
    resource->createConstantBuffer("transformMatrix", sizeof(XMMATRIX)*3);
    resource->createConstantBuffer("grassVariables", sizeof(float)*4);
    
    grid->extension += createTransformation(grid, resource);
    grid->extension += createVertex(grid, resource, L"GridShader");
    grid->extension += createPixel(grid, resource, L"GridShader");
    grid->extension += disableBackCulling(grid);
    
    //spot->extension += createTransformation(spot, resource);
    //spot->extension += createVertex(spot, resource, L"spot");
    //spot->extension += createPixel(spot, resource, L"spot");
    
    grass->extension += createTransformation(grass, resource);
    grass->extension += createUnifiedShader(grass, L"grass", "vp");
    grass->extension += disableBackCulling(grass);
    grass->extension += createBlendState(grass);
    grass->constBuffers = {"transformMatrix", "grassVariables"};

    Watch* stopwatch = new Watch();
    
    state = new State();
    state->pDevice = device;
    state->mesh = grid;
    state->stopwatch = stopwatch;
    state->resources = resource;
    state->shaders = shaders;
    state->query = query;

    device->pRender->QueryInterface(__uuidof(IDXGISurface2), (void**)pSurface);
    device->pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void**)pDevice);

    state->variables[L"Distance"] = 1000;
    state->variables[L"Instances"] = 100;
    
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
    
    const wchar_t* x[] = {L"Distance", L"Position X",
                          L"Position Y", L"Position Z",
                          L"in-tes", L"Instances"};

    SAFEARRAY* ar = SafeArrayCreateVector(VT_BSTR, 0, elements);
    
    for(long i = 0; i < elements; ++i)
    {
        BSTR data = SysAllocString(x[i]);
        SafeArrayPutElement(ar, &i, data);
        SysFreeString(data);
    }

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