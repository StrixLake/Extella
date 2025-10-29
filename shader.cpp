#include <shader.h>
#include <d3dcompiler.h>

Shader::Shader(DXDevice* device){
    this->device = device;
    return;
}

void Shader::createShader(const wchar_t* filename, ShaderType type, ShaderIndex index){

    ID3DBlob *ShaderByte;
    D3DReadFileToBlob(filename, &ShaderByte);

    shaderBlob[index] = ShaderByte;

    switch(type){
        case vertex:
        {
            ID3D11VertexShader* vshader;
            device->pDevice->CreateVertexShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &vshader);
            vertexShaders[index] = vshader;
            break;
        }
        case pixel:
        {
            ID3D11PixelShader* pshader;
            device->pDevice->CreatePixelShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &pshader);
            pixelShaders[index] = pshader;
            break;
        }
    }
}
