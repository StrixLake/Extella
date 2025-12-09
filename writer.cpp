#include <cwchar>
#include <writer.h>

Writer::Writer(DXDevice* device){

    // get the target surface
    IDXGISurface *pSurface;
    //device->pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&pSurface);

    // get hwnd from swapChain desc
    DXGI_SWAP_CHAIN_DESC desc;
    //device->pSwapChain->GetDesc(&desc);

    //HWND hwnd = desc.OutputWindow;
    //float dpi = GetDpiForWindow(hwnd);

    //D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
    //                                            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
    //                                            dpi, dpi);

    ID2D1Factory *pFactory;
    //D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pFactory);

    //pFactory->CreateDxgiSurfaceRenderTarget(pSurface, properties, &this->pRenderTarget);

    //pFactory->Release();
    //pSurface->Release();

    // set up dwrite now
    //IDWriteFactory *pDFactory;
    //DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&pDFactory);

    //pDFactory->CreateTextFormat(L"Ariel", NULL, 
    //                            DWRITE_FONT_WEIGHT_REGULAR,
    //                            DWRITE_FONT_STYLE_NORMAL,
    //                            DWRITE_FONT_STRETCH_NORMAL,
    //                            18.f, 
    //                            L"en-us",
    //                            &this->pFormat);

    //pDFactory->Release();

    //pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite), &this->pBrush);

    QueryPerformanceCounter(&performanceCount);
    QueryPerformanceCounter(&delay);
    QueryPerformanceFrequency(&frequency);

    
    return;
}


void Writer::frameTime(){

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double time_elapsed = 1000* (double)(now.QuadPart - performanceCount.QuadPart) / frequency.QuadPart;
    double time_delay = 1000* (double)(now.QuadPart - delay.QuadPart) / frequency.QuadPart;
    
    if (time_delay > 100){ 
        last_frame_time = time_elapsed;
        delay = now;
    }

    wchar_t time[9] = L"";
    time[7] = L'm';
    time[8] = L's';

    swprintf(time, 7, L"%f", last_frame_time);
    
    //pRenderTarget->BeginDraw();
//
    //D2D1_RECT_F rect = {0, 0, 150, 50};
//
    //pRenderTarget->DrawTextA(time, 9, pFormat, rect, pBrush);
//
    //pRenderTarget->EndDraw();

    performanceCount = now;

    return;
}



Writer::~Writer(){
    //pRenderTarget->Release();
    //pBrush->Release();
    //pFormat->Release();
    
}