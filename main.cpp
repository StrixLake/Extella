#define EXPORT extern "C" __declspec(dllexport)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Windows.h>
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <watch.h>


class Extension : public BaseWorker{
public:
    Extension() = delete;
    Extension(Mesh* meshPointer) : mesh(meshPointer) {}

    void extensionWork(array<XMMATRIX, 3> &camera, float time){
        float rotationSpeed = 1; // per second;
        camera[0] *= DirectX::XMMatrixRotationY(rotationSpeed*time);
    
        camera[0] = DirectX::XMMatrixTranspose(camera[0]);
        camera[1] = DirectX::XMMatrixTranspose(camera[1]);
        camera[2] = DirectX::XMMatrixTranspose(camera[2]);
    }

    Mesh* mesh;
};


struct {
    DXDevice* device;
    Mesh* spot;
    Watch* stopwatch;
} Allinfo;


EXPORT void InitializeRenderer(IDirect3DSurface9** pSurface){
    

    DXDevice *device = new DXDevice();
    device->CreateViews();
    device->SetTargets();

    Shader *shaders = new Shader(device);

    Mesh *spot = new Mesh("mesh/spot_.obj", shaders, obj, L"mesh/default_.png");
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
