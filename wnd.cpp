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

    //Window *window = (Window*)GetWindowLongPtr(hwnd, -21);

    switch(uMsg){
        case WM_LBUTTONDOWN:
            window->isClicked = true;
            window->xprev = *(short*)&lParam;
            window->yprev = *((short*)&lParam +1);
            break;
            case WM_LBUTTONUP:
            window->isClicked = false;
            break;
            case WM_MOUSEMOVE:{
                short xprev = *(short*)&lParam;
                short yprev = *((short*)&lParam +1);
                if(window->isClicked){
                    window->camera->horizontalAngle = -tan(float(xprev - window->xprev)/300);
                    window->camera->verticalAngle = -tan(float(yprev - window->yprev)/300);
                    window->xprev = xprev;
                    window->yprev = yprev;
                }
            break;
            }
        case WM_KEYDOWN:
            if( wParam == 0x57) /*W*/  window->camera->key_w = 1;
            else if( wParam == 0x41) /*A*/  window->camera->key_a = 1;
            else if( wParam == 0x53) /*S*/  window->camera->key_s = 1;
            else if( wParam == 0x44) /*D*/  window->camera->key_d = 1;
            else if( wParam == 0x45) /*E*/  window->camera->key_e = 1;
            else if( wParam == 0x51) /*Q*/  window->camera->key_q = 1;
            break;
        case WM_KEYUP:
            if( wParam == 0x57) /*W*/  window->camera->key_w = 0;
            else if( wParam == 0x41) /*A*/  window->camera->key_a = 0;
            else if( wParam == 0x53) /*S*/  window->camera->key_s = 0;
            else if( wParam == 0x44) /*D*/  window->camera->key_d = 0;
            else if( wParam == 0x45) /*D*/  window->camera->key_e = 0;
            else if( wParam == 0x51) /*D*/  window->camera->key_q = 0;
            break;
        case WM_DESTROY:
        case WM_QUIT:
            PostQuitMessage(0);
        default:
           return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    }

    return 0;
}