#pragma once
#include <d3d11.h>
#include <vector>
#include <config.h>
#include <materials.h>
#include <DirectXMath.h>
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
using std::vector;

struct Mesh
{
    string name;
    string prefered_material;
    vector<unsigned int> index_buffer;
    struct Semantic_Buffer
    {
        string semantic;
        unsigned int stride;
        DXGI_FORMAT format;
        vector<float> buffer;
    };
    vector<Semantic_Buffer> buffers;

};

class GPUMesh
{
public:

    string name;
    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
    uniq_com_ptr<ID3D11Buffer> index_buffer;
    int triangle_count;
    int instance_count = 1;
    struct VertexBuffers
    {
        string semantic;
        uniq_com_ptr<ID3D11Buffer> vertex_buffer;
        unsigned int stride;
        DXGI_FORMAT format;
    };

    string prefered_material;

    unordered_map<string, VertexBuffers> vertex_buffers;
};

std::pair<vector<Mesh>, vector<Material>> load_obj(string filename);

GPUMesh convert_mesh(Mesh& mesh, ID3D11Device* pDevice);