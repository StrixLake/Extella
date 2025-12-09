#include <device.h>
#include <mesh.h>
#include <watch.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#define MOVE 0.01

using DirectX::XMVectorSet;

void renderer(DXDevice* device, Mesh* mesh, Watch* stopwatch){

    auto color = DirectX::Colors::Black;

    
    array<XMMATRIX, 3> transform = {DirectX::XMMatrixIdentity(), // world transform
                                    DirectX::XMMatrixLookAtLH(XMVectorSet(0., 0., -20., 0.),
                                                                  XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ),
                                                                  XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f )), // camera matrix
                                      DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 
                                                                          (float)WIDTH/HEIGHT, 0.01, 100.)}; // perspective matrix
                                                                          
    stopwatch->lap();
                                                                          
    float time = stopwatch->getTotalTime(); // in seconds

    device->pContext->ClearDepthStencilView(device->pDepthView, D3D11_CLEAR_DEPTH, 1.f, 0);
    device->pContext->ClearRenderTargetView(device->pRenderView, color);

        
    Mesh* nmesh = mesh;
    while(nmesh != NULL){
        nmesh->Draw(transform, time);
        nmesh = nmesh->next;
    }

    device->pContext->Flush();
    
    return;
}