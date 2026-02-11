#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <windows.h>
#include <config.h>
#include <array>
#include <DirectXMath.h>
#include <boost/unordered_map.hpp>
#include <boost/container/string.hpp>
using boost::unordered_map;
using wstring = boost::container::basic_string<wchar_t>;
using DirectX::XMMATRIX;
using std::array;

class DXDevice{
public:
    DXDevice();

    void CreateSwap(HWND hwnd);

    void CreateViews();

    void SetTargets();

    // dx11 interfaces
    ID3D11Device* pDevice = NULL;
    ID3D11DeviceContext* pContext = NULL;
    IDXGISwapChain1* pSwapChain = NULL;
    ID3D11Texture2D* pRender = NULL;
    ID3D11RenderTargetView* pRenderView = NULL;
    ID3D11Texture2D* pDepthBuffer = NULL;
    ID3D11DepthStencilView* pDepthView = NULL;

    DXGI_ADAPTER_DESC ddc;
    
    ~DXDevice();
};
