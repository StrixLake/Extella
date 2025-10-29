#include <wnd.h>
#include <device.h>
#include <mesh.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
using DirectX::XMVectorSet;

void render(Viewport* viewport, DXDevice* device, Mesh* mesh, atomic<int>* kill_sig){

    auto color = DirectX::Colors::Black;

    float time = 0; // in seconds
    ULONG timeMSBase = GetTickCount64();

    array<XMMATRIX, 3> transform = {DirectX::XMMatrixIdentity(), // world transform
                                    DirectX::XMMatrixLookAtLH(XMVectorSet(0., 0., -5., 0.),
                                                                  XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ),
                                                                  XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f )),
                                    DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)WIDTH/HEIGHT, 0.01, 100.)};

    viewport->horizontalAngle = 0;
    viewport->verticalAngle = 0;
    viewport->x_pos = 0;
    viewport->y_pos = 0;
    viewport->z_pos = 0;

    while(!*kill_sig){
        device->pContext->ClearDepthStencilView(device->pDepthView, D3D11_CLEAR_DEPTH, 1.f, 0);
        device->pContext->ClearRenderTargetView(device->pRenderView, color);

        
        Mesh* nmesh = mesh;
        while(nmesh != NULL){
            nmesh->Draw(transform, time);
            nmesh = nmesh->next;
        }

        time = (float)(GetTickCount64() - timeMSBase)/1000;
        device->pSwapChain->Present(0,0);

        Sleep(5);
    }
    
    return;
}