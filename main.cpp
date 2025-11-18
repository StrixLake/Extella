#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
                                            1, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                                 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        shader->device->pDevice->CreateInputLayout(layout, 3, shader->shaderBlob[cowVertex]->GetBufferPointer(), 
                                                   shader->shaderBlob[cowVertex]->GetBufferSize(), &pLayout);

        D3D11_BUFFER_DESC cBuffer = {};
        cBuffer.Usage = D3D11_USAGE_DEFAULT;
        cBuffer.ByteWidth = sizeof(XMMATRIX)*3;
        cBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        shader->device->pDevice->CreateBuffer(&cBuffer, NULL, &transformBuffer);
        
        int x, y, z;
        uint8_t* image2 = stbi_load("mesh/spot_.png", &x, &y, &z, 3);
        uint8_t* image = (uint8_t*) malloc(x*y*4);
        uint8_t* image1 = (uint8_t*) malloc(x*y*4);
        uint8_t* imaget = image;
        for (int i = 0; i < x*y*3; i += 3){
            memcpy(imaget, image2, 3);
            imaget[3] = 255;
            imaget += 4;
            image2 += 3;
        }
        image1 += (x*y*4 -1);
        for(int i = 0; i < y; ++i){
            image1 -= x*4;
            memcpy(image1, image, x*4);
            image += x*4;
        }
        

        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        device->pDevice->CreateSamplerState( &sampDesc, &pSampler );

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = x;
        desc.Height = y;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.SampleDesc = {1,0};
        D3D11_SUBRESOURCE_DATA init;
        init.pSysMem = image1;
        init.SysMemPitch = x*4;
        device->pDevice->CreateTexture2D(&desc, &init, &pTexture);
        device->pDevice->CreateShaderResourceView(pTexture, NULL, &pTextureView);
        
        
        return;
    }

    ID3D11SamplerState *pSampler;
    ID3D11Texture2D *pTexture;
    ID3D11ShaderResourceView *pTextureView;
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
        stride = sizeof(DirectX::XMFLOAT2);
        device->pContext->IASetVertexBuffers(1, 1, &pTexCoords, &stride, &offset);
        stride = sizeof(DirectX::XMFLOAT3);
        device->pContext->IASetVertexBuffers(2, 1, &pNormals, &stride, &offset);

        device->pContext->IASetIndexBuffer(pIndices, DXGI_FORMAT_R32_UINT, 0);

        device->pContext->VSSetShader(shaders->vertexShaders[cowVertex], NULL, 0);
        device->pContext->PSSetShader(shaders->pixelShaders[cowPixel], NULL, 0);

        device->pContext->VSSetConstantBuffers(0, 1, &transformBuffer);

        device->pContext->PSSetSamplers(0, 1, &pSampler);
        device->pContext->PSSetShaderResources(0, 1, &pTextureView);

        device->pContext->DrawIndexed(triangleCount*3, 0, 0);
    }

    ~Cow(){
        pLayout->Release();
        transformBuffer->Release();
        shaders->vertexShaders[cowVertex]->Release();
        shaders->pixelShaders[cowPixel]->Release();
        pSampler->Release();
        pTexture->Release();
        pTextureView->Release();
    }

};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PWSTR pCmdLine, int nCmdShow){
    
    Window window(hInstance);

    InputState input;
    input = 0;

    window.camera = &input;

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
    thread renderThread(render, &input, &device, (Mesh*)&spot, &kill_sig);

    
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    kill_sig = 1;
    renderThread.join();

    return 0;
}