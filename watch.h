#pragma once
#include <device.h>
#include <dwrite.h>
#include <d2d1.h>


class Watch{
    
    LARGE_INTEGER frequency;
    LARGE_INTEGER initialTime;
    LARGE_INTEGER lastLap;
    double lap_time = 0;

public:
    Watch();
    
    void lap();
    double getLapTime();
    double getTotalTime();
};