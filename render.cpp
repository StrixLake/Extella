#include <device.h>
#include <mesh.h>
#include <writer.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#define MOVE 0.01

using DirectX::XMVectorSet;

void renderer(void* input, DXDevice* device, Mesh* mesh){

    auto color = DirectX::Colors::Black;

    float time = 0; // in seconds
    ULONG timeMSBase = GetTickCount64();

    array<XMMATRIX, 3> transform = {DirectX::XMMatrixIdentity(), // world transform
                                    DirectX::XMMatrixLookAtLH(XMVectorSet(0., 0., -20., 0.),
                                                                  XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ),
                                                                  XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f )), // camera matrix
                                    DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 
                                                                          (float)WIDTH/HEIGHT, 0.01, 100.)}; // perspective matrix


    Writer writer(device);

        
    float distance = MOVE*writer.last_frame_time;
        

    device->pContext->ClearDepthStencilView(device->pDepthView, D3D11_CLEAR_DEPTH, 1.f, 0);
    device->pContext->ClearRenderTargetView(device->pRenderView, color);

        
    Mesh* nmesh = mesh;
    while(nmesh != NULL){
        nmesh->Draw(transform, time);
        nmesh = nmesh->next;
    }

    writer.frameTime();

    time = (float)(GetTickCount64() - timeMSBase)/1000;

    device->pContext->Flush();
    
    return;
}