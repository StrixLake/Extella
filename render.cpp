#include <device.h>
#include <mesh.h>
#include <watch.h>
#include <state.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#define MOVE 0.01

using DirectX::XMVectorSet;

void renderer(State* state){

    auto color = DirectX::Colors::Black;

    
    array<XMMATRIX, 3> transform = {DirectX::XMMatrixIdentity(), // world transform
                                    DirectX::XMMatrixLookAtLH(XMVectorSet(0., 20., -20., 0.),
                                                                  XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ),
                                                                  XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f )), // camera matrix
                                      DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 
                                                                          (float)WIDTH/HEIGHT, 0.01, 1000.)}; // perspective matrix
                                                                          
    state->stopwatch->lap();
                                                                          
    state->variables[L"time"] = state->stopwatch->getTotalTime(); // in seconds

    state->pDevice->pContext->ClearDepthStencilView(state->pDevice->pDepthView, D3D11_CLEAR_DEPTH, 1.f, 0);
    state->pDevice->pContext->ClearRenderTargetView(state->pDevice->pRenderView, color);

        
    Mesh* nmesh = state->mesh;
    while(nmesh != NULL){
        nmesh->Draw(transform, state->variables);
        nmesh = nmesh->next;
    }

    //state->pDevice->pContext->Flush();
    
    return;
}