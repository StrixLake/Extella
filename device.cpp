#include <device.h>

DXDevice::DXDevice(){
    
    IDXGIFactory2* pDXGIFactory;
    CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&pDXGIFactory);

    IDXGIAdapter1* pAdapter;
    pDXGIFactory->EnumAdapters1(DEVICE, &pAdapter);

    D3D_FEATURE_LEVEL pFeatures[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
    D3D_FEATURE_LEVEL pFeatureLevel;
    D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, 0, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                        pFeatures, 2, D3D11_SDK_VERSION, &pDevice, &pFeatureLevel, &pContext);
    

    pAdapter->Release();
    pDXGIFactory->Release();

    return;
}

// dead code
void DXDevice::CreateSwap(HWND hwnd){
    
    DXGI_MODE_DESC desc;
    desc.Height = HEIGHT;
    desc.Width = WIDTH;
    desc.RefreshRate = {60, 1};
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = WIDTH;
    sd.Height = HEIGHT;
    sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.Stereo = false;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.SampleDesc = {1,0};
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    IDXGIDevice1* dxgiDevice = 0;
    pDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);

    IDXGIAdapter1* dxgiAdapter = 0;
    dxgiDevice->GetParent(__uuidof(IDXGIAdapter1), (void**)&dxgiAdapter);

    IDXGIFactory2* dxgiFactory = 0;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);

    
    dxgiAdapter->GetDesc(&ddc);

    dxgiFactory->CreateSwapChainForComposition(this->pDevice, &sd, NULL,&this->pSwapChain);

    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();

    return;
}

void DXDevice::CreateViews(){

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    pDevice->CreateRenderTargetView(pRender, &rtvDesc, &pRenderView);
    

    // create depth view
    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.Texture2D.MipSlice = 0;

    pDevice->CreateDepthStencilView(pDepthBuffer, &desc, &this->pDepthView);

    return;
}

void DXDevice::SetTargets(){
    
    // set viewport for the resterizer
    D3D11_VIEWPORT vp = {};
    vp.Width = (float)WIDTH;
    vp.Height = (float)HEIGHT;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.MaxDepth = 1.f;
    vp.MinDepth = 0.f;
    pContext->RSSetViewports(1, &vp);

    return;
}

DXDevice::~DXDevice(){

    if (pDevice != NULL) pDevice->Release();
    if (pContext != NULL) pContext->Release();
    if (pSwapChain != NULL) pSwapChain->Release();
    if (pRenderView != NULL) pRenderView->Release();
    if (pDepthView != NULL) pDepthView->Release();

    return;
}