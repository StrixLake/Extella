#pragma once
#include <config.h>
#include <Windows.h>
#include <atomic>
#include <mesh.h>
using std::atomic;

struct InputState{

    atomic<float> verticalAngle;
    atomic<float> horizontalAngle;
    atomic<float> x_sp, y_sp, z_sp;

    void operator=(float value){
        this->horizontalAngle = value;
        this->verticalAngle = value;
        this->x_sp = value;
        this->y_sp = value;
        this->z_sp = value;
    }
};

void render(InputState* input, DXDevice* device, Mesh* mesh, atomic<int>* kill_sig);

class Window{
public:
    Window(HINSTANCE hInstance);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE handle;
    HWND hwnd;

    InputState* camera;

    bool isClicked = false;
    int xprev;
    int yprev;
};