#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>
#include <config.h>
#include <d3d9.h>

class DXDevice{
public:
    DXDevice();

    void CreateSwap(HWND hwnd);

    void CreateViews();

    void SetTargets();

    // dx9 interfaces
    IDirect3D9Ex* pD3D9ExObj = NULL;
    LPDIRECT3DDEVICE9EX pD3D9ExDevice = NULL;
    IDirect3DSurface9* pD3D9Surface = NULL;
    IDirect3DTexture9* pD3D9Texture = NULL;

    // dx11 interfaces
    ID3D11Device* pDevice = NULL;
    ID3D11DeviceContext* pContext = NULL;
    IDXGISwapChain* pSwapChain = NULL;
    ID3D11Texture2D* pRender = NULL;
    ID3D11RenderTargetView* pRenderView = NULL;
    ID3D11Texture2D* pDepthBuffer = NULL;
    ID3D11DepthStencilView* pDepthView = NULL;
    
    ~DXDevice();
};