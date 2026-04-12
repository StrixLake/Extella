#pragma once
#include <d3d11.h>
#include <config.h>
#include <deque>
#include <materials.h>
#include <boost/unordered_map.hpp>
using boost::unordered_map;
#ifdef BOOST
#include <boost/container/string.hpp>
using wstring = boost::container::basic_string<wchar_t>;
using string = boost::container::basic_string<char>;
#else
#include <string>
using wstring = std::wstring;
using string = std::string;
#endif

class ResourceManager
{
    ID3D11Device* pDevice;

    unordered_map<string, uniq_com_ptr<ID3D11Texture2D>> textures;

    // each const buffer will always be 128 bytes
    // contains 100 buffers
    std::deque<uniq_com_ptr<ID3D11Buffer>> constantBufferRing;

    // set up 1 default samplers and set them to the
    // first slots of all shader stages
    uniq_com_ptr<ID3D11SamplerState> samplers;

public: 
    ResourceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

    void createTexture2D(D3D11_TEXTURE2D_DESC desc, string name);
    void createTexture2DfromImage(string filename);
    void createTexturesFromMaterial(Material& material);

    ID3D11Texture2D* getTexture2D(string texture);

    ID3D11Buffer* getConstBuffer();

};