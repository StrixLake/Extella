#pragma once
#include <d3d11.h>
#include <string>
#include <boost/unordered_map.hpp>
using std::string;
using boost::unordered_map;

// this class is for managing the lifetimes of constant
// buffers and other shader resoures that exist for the
// lifetime of the application
class ShaderResources{

    unordered_map<string, ID3D11Buffer*> constantBuffers;
    ID3D11Device* pDevice = NULL;
public:
    ShaderResources() = delete;
    ShaderResources(ID3D11Device* device);

    void createConstantBuffer(string name, size_t size);

    ID3D11Buffer** getConstBuffer(string name);

    ~ShaderResources();
};