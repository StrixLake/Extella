#include <resources.h>
#include <stb_image.h>

ResourceManager::ResourceManager(ID3D11Device* device)
{
    pDevice = device;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = 128;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    for(int i = 0; i < 100; ++i)
    {
        ID3D11Buffer* ptr;
        pDevice->CreateBuffer(&desc, NULL, &ptr);
        constantBufferRing.push_back(uniq_com_ptr<ID3D11Buffer>(ptr));
    }

}


void ResourceManager::createTexture2D(D3D11_TEXTURE2D_DESC desc, string name)
{
    ID3D11Texture2D* texture;
    pDevice->CreateTexture2D(&desc, NULL, &texture);
    textures[name] = uniq_com_ptr<ID3D11Texture2D>(texture);
}

void ResourceManager::createTexture2DfromImage(string filename)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channel;
    uint8_t* image = stbi_load(filename.data(), &width, &height, &channel, 4);

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.SampleDesc = {1,0};

    D3D11_SUBRESOURCE_DATA initial_data = {};
    initial_data.SysMemPitch = width*4;
    initial_data.pSysMem = image;

    ID3D11Texture2D* texture;
    pDevice->CreateTexture2D(&desc, &initial_data, &texture);
    textures[filename] = uniq_com_ptr<ID3D11Texture2D>(texture);
}


ID3D11Texture2D* ResourceManager::getTexture2D(string texture)
{
    return textures[texture].get();
}

ID3D11Buffer* ResourceManager::getConstBuffer()
{
    // move the first buffer in a unique pointer
    uniq_com_ptr<ID3D11Buffer> unique_out = std::move(constantBufferRing.front());
    // remove the null unique pointer from the ring
    constantBufferRing.pop_front();
    // get the raw pointer to return
    ID3D11Buffer* out = unique_out.get();
    // move that unique pointer back in the ring
    constantBufferRing.push_back(unique_out);
    return out;
}