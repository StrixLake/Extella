#include <watch.h>

Watch::Watch(){
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&initialTime);
    QueryPerformanceCounter(&lastLap);
}

void Watch::lap(){
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    lap_time = ((double)now.QuadPart - (double)lastLap.QuadPart)/(double)frequency.QuadPart;

    lastLap = now;
    return;
}

double Watch::getLapTime(){
    return lap_time;
}

double Watch::getTotalTime(){
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    double time_elapsed = ((double)now.QuadPart - (double)initialTime.QuadPart)/(double)frequency.QuadPart;
    return time_elapsed;
}