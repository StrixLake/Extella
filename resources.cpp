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

    constantSizedBuffers[size] = buffer;
}

ID3D11Buffer** ShaderResources::getConstBuffer(string name)
{

    // if that buffer doesn't exist, return null
    if(constantBuffers.find(name) == constantBuffers.end()) return NULL;

    return &constantBuffers[name];
}

ID3D11Buffer** ShaderResources::getConstBuffer(size_t size)
{
    // if size if not multiple of 16, return NULL
    if (size % 16 != 0) return NULL;

    // if that buffer doesn't exist, create the buffer
    if(constantSizedBuffers.find(size) == constantSizedBuffers.end())
    {
        createConstantBuffer(size);
    }

    return &constantSizedBuffers[size];
}

ShaderResources::~ShaderResources()
{
    for (auto buffers : constantBuffers){
        buffers.second->Release();
    }
    for (auto buffers : constantSizedBuffers){
        buffers.second->Release();
    }
    for (auto texture : textures){
        texture.second->Release();
    }
}

void ShaderResources::createTexture2D(string name, D3D11_TEXTURE2D_DESC description)
{
    ID3D11Texture2D* pTexture = NULL;
    pDevice->CreateTexture2D(&description, NULL, &pTexture);
    textures[name] = pTexture;
}

ID3D11Texture2D* ShaderResources::getTexture2D(string name)
{
    if(textures.find(name) == textures.end()) return NULL;
    
    return textures[name];
}