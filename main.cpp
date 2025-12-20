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
        //float rotationSpeed = 1;// per second;
        float angleRotated = Allinfo.variables[L"Angle"]/100;
        camera[0] *= DirectX::XMMatrixRotationY(angleRotated);
    
        float xoffset = Allinfo.variables[L"Position X"] /100;
        float yoffset = Allinfo.variables[L"Position Y"] /100;
        float zoffset = Allinfo.variables[L"Position Z"] /100;
        camera[0] *= DirectX::XMMatrixTranslation(xoffset, yoffset, zoffset);

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
    int elements = 4;
    BSTR *x = (BSTR*)CoTaskMemAlloc(sizeof(BSTR)*elements);
    x[0] = SysAllocString(L"Angle");
    x[1] = SysAllocString(L"Position X");
    x[2] = SysAllocString(L"Position Y");
    x[3] = SysAllocString(L"Position Z");

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