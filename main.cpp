#define EXPORT extern "C" __declspec(dllexport)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Windows.h>
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <watch.h>
#include <atomic>
using std::atomic;

struct {
    DXDevice* device;
    Mesh* spot;
    Watch* stopwatch;
    Shader* shaders;
    unordered_map<wstring, atomic<float>> variables;
} Allinfo;

class Extension : public BaseWorker{
public:
    Extension() = delete;
    Extension(Mesh* meshPointer) : mesh(meshPointer) {}

    void extensionWork(array<XMMATRIX, 3> &camera, float time){
        //float rotationSpeed = 1;// per second;
        float angleRotated = Allinfo.variables[L"Angle"]/100;
        camera[0] *= DirectX::XMMatrixRotationY(angleRotated);
    
        float xoffset = Allinfo.variables[L"Position X"] /100;
        float yoffset = Allinfo.variables[L"Position Y"] /100;
        float zoffset = Allinfo.variables[L"Position Z"] /100;
        camera[0] *= DirectX::XMMatrixTranslation(xoffset, yoffset, zoffset);
        camera[0] = DirectX::XMMatrixRotationX(Allinfo.variables[L"CameraY"]/100) * camera[0];
        camera[0] = DirectX::XMMatrixRotationY(Allinfo.variables[L"CameraX"]/100) * camera[0];

        camera[0] = DirectX::XMMatrixTranspose(camera[0]);
        camera[1] = DirectX::XMMatrixTranspose(camera[1]);
        camera[2] = DirectX::XMMatrixTranspose(camera[2]);

        // set the tessellation factor in the constant buffers
        int Tes[4] = {static_cast<int>(Allinfo.variables[L"out-tes"] / 10),
                    static_cast<int>(Allinfo.variables[L"in-tes"] / 10)};

        mesh->device->pContext->UpdateSubresource(mesh->tesBuffer, 0, NULL, Tes, 0, 0);
    }

    Mesh* mesh;
};


EXPORT void InitializeRenderer(IDXGISurface2** pSurface, IDXGIDevice2** pDevice){
    

    DXDevice *device = new DXDevice();
    //device->CreateSwap(0);
    device->CreateViews();
    device->SetTargets();

    Shader *shaders = new Shader(device);
    Allinfo.shaders = shaders;

    Mesh *plane = new Mesh("mesh/plane.obj", shaders, obj, L"mesh/default_.png");
    plane->extension = new Extension(plane);
    Mesh *spot = new Mesh("mesh/spot_.obj", shaders, obj, L"mesh/spot_.png");
    spot->extension = new Extension(spot);
    Watch* stopwatch = new Watch();

    plane->next = spot;

    Allinfo.device = device;
    Allinfo.spot = plane;
    Allinfo.stopwatch = stopwatch;

    device->pRender->QueryInterface(__uuidof(IDXGISurface2), (void**)pSurface);
    device->pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void**)pDevice);

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
    delete Allinfo.shaders;
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
    Allinfo.variables[variable] = value;
}

EXPORT float getVariable(BSTR variable)
{
    return Allinfo.variables[variable];
}