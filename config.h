#pragma once
#define DEVICE 0
#define WIDTH 1920
#define HEIGHT 1080
//#define BOOST

template<typename T>
struct Deleter
{
    void operator()(T& comPointer) noexcept
    {
        comPointer->Release();
    }
};