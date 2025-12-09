#define EXPORT extern "C" __declspec(dllexport)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Windows.h>
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <watch.h>


class Cow : Mesh{
public:
    Cow(const char* filename, Shader* shader, const wchar_t* textureFile) : Mesh(filename, shader, obj, textureFile) {}

    void Draw(array<XMMATRIX,3> camera, float time){
        float rotationSpeed = 1; // per second;
        camera[0] *= DirectX::XMMatrixRotationY(rotationSpeed*time);
    
        camera[0] = DirectX::XMMatrixTranspose(camera[0]);
        camera[1] = DirectX::XMMatrixTranspose(camera[1]);
        camera[2] = DirectX::XMMatrixTranspose(camera[2]);

        device->pContext->UpdateSubresource(transformBuffer, 0, NULL, 
                                            camera.data(), 0, 0);
        
        device->pContext->IASetInputLayout(pLayout);

        UINT offset = 0;
        UINT stride = sizeof(DirectX::XMFLOAT3);
        device->pContext->IASetVertexBuffers(0, 1, &pVertices, &stride, &offset);
        stride = sizeof(DirectX::XMFLOAT2);
        device->pContext->IASetVertexBuffers(1, 1, &pTexCoords, &stride, &offset);
        stride = sizeof(DirectX::XMFLOAT3);
        device->pContext->IASetVertexBuffers(2, 1, &pNormals, &stride, &offset);

        device->pContext->IASetIndexBuffer(pIndices, DXGI_FORMAT_R32_UINT, 0);

        device->pContext->VSSetShader(shaders->getVertexShader(L"shaders/vshader.cso"), NULL, 0);
        device->pContext->PSSetShader(shaders->getPixelShader(L"shaders/pshader.cso"), NULL, 0);

        device->pContext->VSSetConstantBuffers(0, 1, &transformBuffer);

        device->pContext->PSSetSamplers(0, 1, &pSampler);
        device->pContext->PSSetShaderResources(0, 1, &pTextureView);

        device->pContext->DrawIndexed(triangleCount*3, 0, 0);
    }


};


struct {
    DXDevice* device;
    Cow* spot;
    Watch* stopwatch;
} Allinfo;


EXPORT void InitializeRenderer(IDirect3DSurface9** pSurface){
    

    DXDevice *device = new DXDevice();
    device->CreateViews();
    device->SetTargets();

    Shader *shaders = new Shader(device);

    Cow *spot = new Cow("mesh/spot_.obj", shaders, L"mesh/spot_.png");
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
