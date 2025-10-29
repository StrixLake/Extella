#pragma once
#include <config.h>
#include <Windows.h>
#include <atomic>
#include <mesh.h>
using std::atomic;

struct Viewport{
    atomic<float> verticalAngle;
    atomic<float> horizontalAngle;
    atomic<float> x_pos, y_pos, z_pos;
};

void render(Viewport* viewport, DXDevice* device, Mesh* mesh, atomic<int>* kill_sig);

class Window{
public:
    Window(HINSTANCE hInstance);

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnClick();
    void OnMove();

    HINSTANCE handle;
    HWND hwnd;

    Viewport* camera;

    struct mouse{
        bool isClicked = false;
        int xprev;
        int yprev;
    };
};