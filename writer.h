#pragma once
#include <device.h>
#include <dwrite.h>
#include <d2d1.h>


class Writer{
public:
    Writer(DXDevice* device);

    void fps();
    void frameTime();
    LARGE_INTEGER performanceCount;
    LARGE_INTEGER frequency;

    ID2D1RenderTarget *pRenderTarget;
    ID2D1SolidColorBrush *pBrush;
    IDWriteTextFormat *pFormat;

    ~Writer();
};