#include <config.h>
#include <extella.h>
#include <windows.h>
#include <d3d11.h>
#define UNICODE

IDXGISwapChain* CreateSwap(HWND hwnd, ID3D11Device* pDevice){
    
    DXGI_MODE_DESC desc;
    desc.Height = HEIGHT;
    desc.Width = WIDTH;
    desc.RefreshRate = {60, 1};
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc = desc;
    sd.BufferCount = 2;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.Flags = 0;
    sd.SampleDesc = {1, 0};
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.OutputWindow = hwnd;

    IDXGIDevice* dxgiDevice = 0;
    pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

    IDXGIAdapter* dxgiAdapter = 0;
    dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

    IDXGIFactory* dxgiFactory = 0;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

    DXGI_ADAPTER_DESC ddc;
    dxgiAdapter->GetDesc(&ddc);
    OutputDebugString("Device in use: \n");
    OutputDebugStringW(ddc.Description);
    OutputDebugString("\n");
    
    IDXGISwapChain* pSwapChain = NULL;

    dxgiFactory->CreateSwapChain(pDevice, &sd, &pSwapChain);
    
    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();
    
    return pSwapChain;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
    
    IDXGIDevice2* dxgiDevice = NULL;
    IDXGISurface2* dxgiSurface = NULL;
    InitializeRenderer(&dxgiSurface, &dxgiDevice);
    
    ID3D11DeviceContext* pContext = NULL;
    ID3D11Device* pDevice = NULL;
    ID3D11Texture2D* renderTexture = NULL;
    dxgiDevice->QueryInterface(__uuidof(ID3D11Device), (void**)&pDevice);
    dxgiSurface->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&renderTexture);
    dxgiDevice->Release();
    dxgiSurface->Release();
    pDevice->GetImmediateContext(&pContext);
    
    // Register the window class.
    const char CLASS_NAME[]  = "Extella";
    
    WNDCLASS wc = { };

    
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        "Viewport",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        0, 0, WIDTH, HEIGHT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    IDXGISwapChain* swapchain = CreateSwap(hwnd, pDevice);
    ID3D11Texture2D* backTexture = NULL;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTexture);

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (true)
    {
        if(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        render();
        pContext->CopyResource(backTexture, renderTexture);
        swapchain->Present(2,0);
    }

    pDevice->Release();
    pContext->Release();
    backTexture->Release();
    renderTexture->Release();
    swapchain->Release();
    release();

    return 0;
}
