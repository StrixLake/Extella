#include <resources.h>
#include <stb_image.h>
#include <DirectXColors.h>

ResourceManager::ResourceManager(ID3D11Device* device, ID3D11DeviceContext* pContext)
{
    stbi_set_flip_vertically_on_load(true);
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

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MinLOD = -FLT_MAX;
    samplerDesc.MaxLOD = FLT_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    
    ID3D11SamplerState* pSampler;
    pDevice->CreateSamplerState(&samplerDesc, &pSampler);
    samplers = uniq_com_ptr<ID3D11SamplerState>(pSampler);

    pContext->VSSetSamplers(0, 1, &pSampler);
    pContext->HSSetSamplers(0, 1, &pSampler);
    pContext->DSSetSamplers(0, 1, &pSampler);
    pContext->GSSetSamplers(0, 1, &pSampler);
    pContext->PSSetSamplers(0, 1, &pSampler);

}

void ResourceManager::clearRTV(ID3D11DeviceContext* pContext) const
{
    for(const uniq_com_ptr<ID3D11RenderTargetView>& rtv : rtvs)
    {
        ID3D11RenderTargetView* view_to_clear = rtv.get();
        pContext->ClearRenderTargetView(view_to_clear, DirectX::Colors::Black);
    }
    pContext->ClearDepthStencilView(depthView.get(),  D3D11_CLEAR_DEPTH, 1.f, 0);
}


void ResourceManager::createTexture2D(D3D11_TEXTURE2D_DESC desc, string name)
{
    ID3D11Texture2D* texture;
    pDevice->CreateTexture2D(&desc, NULL, &texture);
    textures[name] = uniq_com_ptr<ID3D11Texture2D>(texture);
    // if it's a render target, create an rtv view so it can be set
    // to black on clear before rendering
    if(desc.BindFlags & D3D11_BIND_RENDER_TARGET)
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        ID3D11RenderTargetView* pRenderView;
        pDevice->CreateRenderTargetView(texture, &rtvDesc, &pRenderView);
        rtvs.push_back(uniq_com_ptr<ID3D11RenderTargetView>(pRenderView));
    }
    // or if it's the default depth buffer, then create a depth view
    // to clear that later as well
    if(name == "depth buffer")
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
        depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.Texture2D.MipSlice = 0;
        ID3D11DepthStencilView* pDepthView;
        pDevice->CreateDepthStencilView(texture, &depthDesc, &pDepthView);
        depthView = uniq_com_ptr<ID3D11DepthStencilView>(pDepthView);
    }
}

void ResourceManager::createTexture2DfromImage(string filename)
{
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
    stbi_image_free(image);
}


ID3D11Texture2D* ResourceManager::getTexture2D(string texture)
{
    return textures.at(texture).get();
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
    constantBufferRing.push_back(std::move(unique_out));
    return out;
}

// the material must only contain texture map names
void ResourceManager::createTexturesFromMaterial(Material& material)
{
    for(auto texture : material.material_textures)
    {
        string texture_path = texture.second;
        if(texture_path != "") createTexture2DfromImage(texture_path);
    }
}