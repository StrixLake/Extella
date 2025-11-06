#pragma once
#include <config.h>
#include <Windows.h>
#include <atomic>
#include <mesh.h>
using std::atomic;

struct Viewport{

    atomic<float> verticalAngle;
    atomic<float> horizontalAngle;
    atomic<float> x_sp, y_sp, z_sp;

    void operator=(float value){
        this->horizontalAngle = 0;
        this->verticalAngle = 0;
        this->x_sp = 0;
        this->y_sp = 0;
        this->z_sp = 0;
    }
};

void render(Viewport* viewport, DXDevice* device, Mesh* mesh, atomic<int>* kill_sig);

class Window{
public:
    Window(HINSTANCE hInstance);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE handle;
    HWND hwnd;

    Viewport* camera;

    bool isClicked = false;
    int xprev;
    int yprev;
};