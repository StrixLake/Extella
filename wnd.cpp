#include <wnd.h>

void SetState(HWND hwnd, LPARAM lParam){
    CREATESTRUCT* data = (CREATESTRUCT*)lParam;
    void* pthis = data->lpCreateParams;
    // -21 is for GWLP_USERDATA
    SetWindowLongPtr(hwnd, -21, (LONG_PTR)pthis);
    return;
}

Window::Window(HINSTANCE hInstance) : handle(hInstance){

    WNDCLASS wnd = {};
    wnd.hInstance = hInstance;
    wnd.lpfnWndProc = Window::WndProc;
    wnd.lpszClassName = "Extella";

    RegisterClass(&wnd);

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
    
    this->hwnd = CreateWindowEx(0, "Extella", "Viewport", WS_OVERLAPPEDWINDOW,
        100, 100, WIDTH, HEIGHT, NULL, NULL, hInstance, this);

    return;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

    if (uMsg == WM_NCCREATE) SetState(hwnd, lParam);

    Window *window = (Window*)GetWindowLongPtr(hwnd, -21);

    switch(uMsg){
        case WM_LBUTTONDOWN:
            window->isClicked = true;
            break;
        case WM_LBUTTONUP:
            window->isClicked = false;
            break;
        case WM_KEYDOWN:
            #define MOVE 0.01
            if( wParam == 0x57) /*W*/  window->camera->z_sp = -1*MOVE;
            if( wParam == 0x41) /*A*/  window->camera->x_sp = MOVE;
            if( wParam == 0x53) /*S*/  window->camera->z_sp = MOVE;
            if( wParam == 0x44) /*D*/  window->camera->x_sp = -1*MOVE;
            break;
        case WM_KEYUP:
            if( wParam == 0x57) /*W*/  window->camera->z_sp = 0;
            if( wParam == 0x41) /*A*/  window->camera->x_sp = 0;
            if( wParam == 0x53) /*S*/  window->camera->z_sp = 0;
            if( wParam == 0x44) /*D*/  window->camera->x_sp = 0;
            break;
        case WM_DESTROY:
        case WM_QUIT:
            PostQuitMessage(0);
        default:
           return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    }

    return 0;
}