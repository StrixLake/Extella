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
#include <d3d11shader.h>
#include <d3dcompiler.h>

State* state;



EXPORT void InitializeRenderer(IDXGISurface2** pSurface, IDXGIDevice2** pDevice){

    DXDevice *device = new DXDevice();
    //device->CreateSwap(0);
    device->CreateViews();
    device->SetTargets();

    Query* query = new Query(device->pDevice, device->pContext);

    Shader *shaders = new Shader(device->pDevice);
    
    state = new State();
    state->pDevice = device;
    state->mesh = grid;
    state->stopwatch = stopwatch;
    state->resources = resource;
    state->shaders = shaders;
    state->query = query;

    device->pRender->QueryInterface(__uuidof(IDXGISurface2), (void**)pSurface);
    device->pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void**)pDevice);

    state->variables[L"Distance"] = 100;
    state->variables[L"aspect ratio"] = (float)WIDTH/HEIGHT;

    // testing something unrelated
    ID3DBlob* vShader = shaders->getVertexShaderBlob(L"spot");
    volatile Shader_Reflection_Desc desc = shaders->reflect(vShader);

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