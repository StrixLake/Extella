#include <Windows.h>
#include <wnd.h>
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <thread>
using std::thread;

class Cow : Mesh{
public:
    Cow(const char* filename, Shader* shader) : Mesh(filename, shader, obj){
        this->shaders->createShader(L"shaders/vshader.cso", vertex, cowVertex);
        this->shaders->createShader(L"shaders/pshader.cso", pixel, cowPixel);

        D3D11_INPUT_ELEMENT_DESC layout[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                            0, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0}};

        shader->device->pDevice->CreateInputLayout(layout, 1, shader->shaderBlob[cowVertex]->GetBufferPointer(), 
                                                   shader->shaderBlob[cowVertex]->GetBufferSize(), &pLayout);

        D3D11_BUFFER_DESC cBuffer = {};
        cBuffer.Usage = D3D11_USAGE_DEFAULT;
        cBuffer.ByteWidth = sizeof(XMMATRIX)*3;
        cBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        shader->device->pDevice->CreateBuffer(&cBuffer, NULL, &transformBuffer);
        
        return;
    }

    ID3D11Buffer *transformBuffer;
    ID3D11InputLayout* pLayout;

    void Draw(array<XMMATRIX,3> camera, float time) override{
        float rotationSpeed = 1; // per second;
        camera[0] *= DirectX::XMMatrixRotationY(rotationSpeed*time);
    
        camera[0] = DirectX::XMMatrixTranspose(camera[0]);
        camera[1] = DirectX::XMMatrixTranspose(camera[1]);
        camera[2] = DirectX::XMMatrixTranspose(camera[2]);

        shaders->device->pContext->UpdateSubresource(transformBuffer, 0, NULL, camera.data(), 0, 0);
        
        shaders->device->pContext->IASetInputLayout(pLayout);

        UINT offset = 0;
        UINT stride = sizeof(DirectX::XMFLOAT3);
        device->pContext->IASetVertexBuffers(0, 1, &pVertices, &stride, &offset);
        //device->pContext->IASetVertexBuffers(1, 1, &pNormals, &stride, &offset);

        device->pContext->IASetIndexBuffer(pIndices, DXGI_FORMAT_R32_UINT, 0);

        device->pContext->VSSetShader(shaders->vertexShaders[cowVertex], NULL, 0);
        device->pContext->PSSetShader(shaders->pixelShaders[cowPixel], NULL, 0);

        device->pContext->VSSetConstantBuffers(0, 1, &transformBuffer);

        device->pContext->DrawIndexed(triangleCount*3, 0, 0);
    }

    ~Cow(){
        pLayout->Release();
        transformBuffer->Release();
        shaders->vertexShaders[cowVertex]->Release();
        shaders->pixelShaders[cowPixel]->Release();
    }

};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PWSTR pCmdLine, int nCmdShow){
    
    Window window(hInstance);

    Viewport viewport;

    window.camera = &viewport;

    DXDevice device;
    device.CreateSwap(window.hwnd);
    device.CreateViews();
    device.SetTargets();

    Shader shaders(&device);

    Cow spot("mesh/spot_.obj", &shaders);

    ShowWindow(window.hwnd, nCmdShow);

    MSG msg = {};

    atomic<int> kill_sig;
    kill_sig = 0;
    thread renderThread(render, &viewport, &device, (Mesh*)&spot, &kill_sig);

    
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    kill_sig = 1;
    renderThread.join();

    return 0;
}