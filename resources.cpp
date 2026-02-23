#include <resources.h>

ShaderResources::ShaderResources(ID3D11Device* device) : pDevice(device){}

void ShaderResources::createConstantBuffer(string name, size_t size)
{
    // don't create that buffer if another buffer of the same already exists
    if (constantBuffers.find(name) != constantBuffers.end()) return;

    ID3D11Buffer* buffer;
    D3D11_BUFFER_DESC cBuffer = {};
    cBuffer.Usage = D3D11_USAGE_DEFAULT;
    cBuffer.ByteWidth = size;
    cBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pDevice->CreateBuffer(&cBuffer, NULL, &buffer);

    constantBuffers[name] = buffer;
}

void ShaderResources::createConstantBuffer(size_t size)
{
    ID3D11Buffer* buffer;
    D3D11_BUFFER_DESC cBuffer = {};
    cBuffer.Usage = D3D11_USAGE_DEFAULT;
    cBuffer.ByteWidth = size;
    cBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pDevice->CreateBuffer(&cBuffer, NULL, &buffer);

    constantSizedBuffers[size/16] = buffer;
}

ID3D11Buffer** ShaderResources::getConstBuffer(string name)
{

    // if that buffer doesn't exist, return null
    if(constantBuffers.find(name) == constantBuffers.end()) return NULL;

    return &constantBuffers[name];
}

ID3D11Buffer** ShaderResources::getConstBuffer(size_t size)
{

    // if that buffer doesn't exist, return null
    if(constantSizedBuffers.find(size) == constantSizedBuffers.end())
    {
        createConstantBuffer(size*16);
    }

    return &constantSizedBuffers[size];
}

ShaderResources::~ShaderResources()
{
    for (auto buffers : constantBuffers){
        buffers.second->Release();
    }
}