#include <device.h>

DXDevice::DXDevice(){
    
    IDXGIFactory1* pDXGIFactory;
    CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pDXGIFactory);

    IDXGIAdapter1* pAdapter;
    pDXGIFactory->EnumAdapters1(DEVICE, &pAdapter);

    D3D_FEATURE_LEVEL pFeatures[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
    D3D_FEATURE_LEVEL pFeatureLevel;
    D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, 0, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                        pFeatures, 2, D3D11_SDK_VERSION, &this->pDevice, &pFeatureLevel, &this->pContext);
    
    #ifdef WIREFRAME
    D3D11_RASTERIZER_DESC desc;
    desc.FillMode = D3D11_FILL_WIREFRAME;
    desc.CullMode = D3D11_CULL_NONE;
    desc.FrontCounterClockwise = false;
    
    ID3D11RasterizerState *pRasterState;
    this->pDevice->CreateRasterizerState(&desc, &pRasterState);
    this->pContext->RSSetState(pRasterState);
    pRasterState->Release();
    #endif
    
    Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D9ExObj);
    D3DPRESENT_PARAMETERS presentParams = {};
    presentParams.Windowed = TRUE;
    presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParams.BackBufferCount = 1;
    presentParams.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParams.hDeviceWindow = GetDesktopWindow(); // Dummy window
    presentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    pD3D9ExObj->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, 
                     D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE, 
                     &presentParams, NULL, &pD3D9ExDevice);

    pAdapter->Release();
    pDXGIFactory->Release();

    return;
}


void DXDevice::CreateSwap(HWND hwnd){
    
    DXGI_MODE_DESC desc;
    desc.Height = HEIGHT;
    desc.Width = WIDTH;
    desc.RefreshRate = {60, 1};
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc = desc;
    sd.BufferCount = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.Flags = 0;
    sd.SampleDesc = {1, 0};
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.OutputWindow = hwnd;

    IDXGIDevice* dxgiDevice = 0;
    pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

    IDXGIAdapter* dxgiAdapter = 0;
    dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

    IDXGIFactory* dxgiFactory = 0;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

    DXGI_ADAPTER_DESC ddc;
    dxgiAdapter->GetDesc(&ddc);

    dxgiFactory->CreateSwapChain(this->pDevice, &sd, &this->pSwapChain);

    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();

    return;
}

void DXDevice::CreateViews(){

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = WIDTH;
    textureDesc.Height = HEIGHT;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    textureDesc.SampleDesc = {1,0};
    pDevice->CreateTexture2D(&textureDesc, NULL, &pRender);

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    pDevice->CreateRenderTargetView(pRender, &rtvDesc, &pRenderView);
    

    // create depth buffer
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = WIDTH;
    descDepth.Height = HEIGHT;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc = {1,0};
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    pDevice->CreateTexture2D(&descDepth, NULL, &this->pDepthBuffer);

    // create depth view
    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.Texture2D.MipSlice = 0;

    pDevice->CreateDepthStencilView(pDepthBuffer, &desc, &this->pDepthView);

    return;
}

void DXDevice::SetTargets(){

    // get a shared handle for the dx9 surface
    IDXGIResource* pDXGIResource;
    pRender->QueryInterface(__uuidof(IDXGIResource), (void**)&pDXGIResource);
    HANDLE pSharedHandle;
    pDXGIResource->GetSharedHandle(&pSharedHandle);

    // create a dx9 texture
    pD3D9ExDevice->CreateTexture(WIDTH, HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pD3D9Texture, &pSharedHandle);
    pD3D9Texture->GetSurfaceLevel(0, &pD3D9Surface);

    // set the render target for dx11 renderer
    pContext->OMSetRenderTargets(1, &pRenderView, pDepthView);
    
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
    if (pD3D9ExObj != NULL) pD3D9ExObj->Release();
    if (pD3D9ExDevice != NULL) pD3D9ExDevice->Release();
    if (pD3D9Surface != NULL) pD3D9Surface->Release();
    if (pD3D9Texture != NULL) pD3D9Texture->Release();

    if (pDevice != NULL) pDevice->Release();
    if (pContext != NULL) pContext->Release();
    if (pSwapChain != NULL) pSwapChain->Release();
    if (pRender != NULL) pRender->Release();
    if (pRenderView != NULL) pRenderView->Release();
    if (pDepthBuffer != NULL) pDepthBuffer->Release();
    if (pDepthView != NULL) pDepthView->Release();

    return;
}