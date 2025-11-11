#pragma once
#include <config.h>
#include <Windows.h>
#include <atomic>
#include <mesh.h>
using std::atomic;

// carries the input state of the keyboard and mouse
// and records the keys pressed
struct InputState{

    atomic<float> verticalAngle;
    atomic<float> horizontalAngle; 
    atomic<int8_t> key_w, key_a, key_s, key_d;
    atomic<int8_t> key_q, key_e;

    void operator=(float value){
        this->horizontalAngle = value;
        this->verticalAngle = value;
        this->key_w = (int8_t)value;
        this->key_a = (int8_t)value;
        this->key_s = (int8_t)value;
        this->key_d = (int8_t)value;
        this->key_q = (int8_t)value;
        this->key_e = (int8_t)value;
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