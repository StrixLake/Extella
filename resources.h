#pragma once
#include <d3d11.h>
#include <boost/unordered_map.hpp>
#include <boost/container/string.hpp>
using string = boost::container::basic_string<char>;
using boost::unordered_map;

// this class is for managing the lifetimes of constant
// buffers and other shader resoures that exist for the
// lifetime of the application
class ShaderResources{

    unordered_map<string, ID3D11Buffer*> constantBuffers;
    unordered_map<size_t, ID3D11Buffer*> constantSizedBuffers;
    ID3D11Device* pDevice = NULL;
public:
    ShaderResources() = delete;
    ShaderResources(ID3D11Device* device);

    void createConstantBuffer(string name, size_t size);
    void createConstantBuffer(size_t size);

    ID3D11Buffer** getConstBuffer(string name);

    ID3D11Buffer** getConstBuffer(size_t size);

    ~ShaderResources();
};