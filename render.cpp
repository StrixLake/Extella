#include <wnd.h>
#include <device.h>
#include <mesh.h>
#include <writer.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#define MOVE 0.01

using DirectX::XMVectorSet;

void render(InputState* input, DXDevice* device, Mesh* mesh, atomic<int>* kill_sig){

    auto color = DirectX::Colors::Black;

    float time = 0; // in seconds
    ULONG timeMSBase = GetTickCount64();

    array<XMMATRIX, 3> transform = {DirectX::XMMatrixIdentity(), // world transform
                                    DirectX::XMMatrixLookAtLH(XMVectorSet(0., 0., -5., 0.),
                                                                  XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ),
                                                                  XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f )), // camera matrix
                                    DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 
                                                                          (float)WIDTH/HEIGHT, 0.01, 100.)}; // perspective matrix


    Writer writer(device);

    while(!*kill_sig){
        
        float distance = MOVE*writer.last_frame_time;
        transform[1] *= DirectX::XMMatrixRotationY(input->horizontalAngle);
        transform[1] *= DirectX::XMMatrixRotationX(input->verticalAngle);

        transform[1] *= DirectX::XMMatrixTranslation((input->key_a-input->key_d)*distance, (input->key_q-input->key_e)*distance, (input->key_s-input->key_w)*distance);
        

        device->pContext->ClearDepthStencilView(device->pDepthView, D3D11_CLEAR_DEPTH, 1.f, 0);
        device->pContext->ClearRenderTargetView(device->pRenderView, color);

        
        Mesh* nmesh = mesh;
        while(nmesh != NULL){
            nmesh->Draw(transform, time);
            nmesh = nmesh->next;
        }

        writer.frameTime();

        time = (float)(GetTickCount64() - timeMSBase)/1000;
        device->pSwapChain->Present(1,0);
    }
    
    return;
}