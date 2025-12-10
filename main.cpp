#define EXPORT extern "C" __declspec(dllexport)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Windows.h>
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <watch.h>

struct {
    DXDevice* device;
    Mesh* spot;
    Watch* stopwatch;
    unordered_map<wstring, float> variables;
} Allinfo;

class Extension : public BaseWorker{
public:
    Extension() = delete;
    Extension(Mesh* meshPointer) : mesh(meshPointer) {}

    void extensionWork(array<XMMATRIX, 3> &camera, float time){
        float rotationSpeed = Allinfo.variables[L"Rotation speed"];
        //float rotationSpeed = 1;// per second;
        camera[0] *= DirectX::XMMatrixRotationY(rotationSpeed*time);
    
        camera[0] = DirectX::XMMatrixTranspose(camera[0]);
        camera[1] = DirectX::XMMatrixTranspose(camera[1]);
        camera[2] = DirectX::XMMatrixTranspose(camera[2]);
    }

    Mesh* mesh;
};


EXPORT void InitializeRenderer(IDirect3DSurface9** pSurface){
    

    DXDevice *device = new DXDevice();
    device->CreateViews();
    device->SetTargets();

    Shader *shaders = new Shader(device);

    Mesh *spot = new Mesh("mesh/spot_.obj", shaders, obj, L"mesh/spot_.png");
    spot->extension = new Extension(spot);
    Watch* stopwatch = new Watch();

    Allinfo.device = device;
    Allinfo.spot = spot;
    Allinfo.stopwatch = stopwatch;

    *pSurface = device->pD3D9Surface;

    return;
}


void renderer(DXDevice* device, Mesh* mesh, Watch* watch);

EXPORT void render(){
    renderer(Allinfo.device, (Mesh*)Allinfo.spot, Allinfo.stopwatch);
}

EXPORT void release() {
    delete Allinfo.device;
    delete Allinfo.spot;
    delete Allinfo.stopwatch;
}

EXPORT SAFEARRAY* getGlobalVariables(){
    BSTR *x = (BSTR*)CoTaskMemAlloc(sizeof(BSTR));
    x[0] = SysAllocString(L"Rotation speed");

    SAFEARRAYBOUND bound = {};
    bound.cElements = 1;
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
    Allinfo.variables[variable] = value;
}