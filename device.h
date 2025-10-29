#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>
#include <config.h>

class DXDevice{
public:
    DXDevice();

    void CreateSwap(HWND hwnd);

    void CreateViews();

    void SetTargets();

    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;
    IDXGISwapChain* pSwapChain;
    ID3D11RenderTargetView* pRenderView;
    ID3D11Texture2D* pDepthBuffer;
    ID3D11DepthStencilView* pDepthView;
    
    ~DXDevice();
};