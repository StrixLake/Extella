#include <mesh.h>
#include <vectors.h>
#include <tiny_obj_loader.h>


// unlike last time where i parsed all 4 cases of
// normals and texcoord being present or not
// this time i assume the obj file will always have normals
// and texcoord

std::pair<vector<Mesh>, vector<Material>> load_obj(string filename)
{
    tinyobj::ObjReaderConfig config;
    config.vertex_color = false;

    tinyobj::ObjReader reader;
    reader.ParseFromFile(filename, config);

    vector<tinyobj::shape_t> shapes = reader.GetShapes();
    assert(shapes.size() != 0);
    
    vector<tinyobj::material_t> materials = reader.GetMaterials();
    
    // sort shapes by material
    std::sort(shapes.begin(), shapes.end(), [](tinyobj::shape_t& a, tinyobj::shape_t& b)
    {
        return a.mesh.material_ids[0] < b.mesh.material_ids[0];
    });

    tinyobj::attrib_t attributes = reader.GetAttrib();

    vector<Material> out_material;
    for(auto material : materials)
    {
        out_material.push_back(material_factory(material));
    }

    // merge shapes with the same material (append their index buffers)
    vector<tinyobj::shape_t> merged_shapes = {shapes[0]};
    for(auto shape = shapes.begin() + 1 /*start with the second element*/ ; shape != shapes.end(); ++shape)
    {
        if (shape->mesh.material_ids[0] == merged_shapes.rbegin()->mesh.material_ids[0]) // if the material id is same as the last shape
        {
            merged_shapes.rbegin()->mesh.indices.insert(merged_shapes.rbegin()->mesh.indices.end(), shape->mesh.indices.begin(), shape->mesh.indices.end());
        }
        else {
            merged_shapes.push_back(*shape);
        }
    }

    vector<Mesh> out_mesh;

    // for each mesh in the final merged meshes, convert them to
    // an instance of Mesh class
    for(auto &mesh : merged_shapes)
    {
        Mesh new_mesh;
        new_mesh.prefered_material = materials[mesh.mesh.material_ids[0]].name;
        new_mesh.name = mesh.name;

        vector<unsigned int> index_buffer;
        vector<float> vertex_buffer;
        vector<float> normal_buffer;
        vector<float> texcoord_buffer;

        unordered_map<Float8, int> index_map;

        for(auto index : mesh.mesh.indices)
        {
            // 1. load the pair of vertex/normal/texcoord for each triangle vertex
            Float3 vertex = {attributes.vertices[3*index.vertex_index], 
                             attributes.vertices[3*index.vertex_index+1],
                             attributes.vertices[3*index.vertex_index+2]};

            Float3 normal = {attributes.normals[3*index.normal_index], 
                             attributes.normals[3*index.normal_index+1],
                             attributes.normals[3*index.normal_index+2]};

            Float2 texcoord =  {attributes.texcoords[2*index.texcoord_index], 
                                attributes.texcoords[2*index.texcoord_index+1]};

            Float8 vertex_pair = {vertex, normal, texcoord};

            // 2. check if that pair is already in the buffer
            if(index_map.find(vertex_pair) != index_map.end())
            // 2.1 if the pair is in the buffer, get its index
            {
                unsigned int old_index = index_map[vertex_pair];
                index_buffer.push_back(old_index);
            }
            // 2.2 if the pair is not in the buffer, put it in the buffer and get its index
            else {
                index_map[vertex_pair] = vertex_buffer.size()/3;

                index_buffer.push_back(index_map[vertex_pair]);

                vertex_buffer.push_back(vertex.x);
                vertex_buffer.push_back(vertex.y);
                vertex_buffer.push_back(vertex.z);

                normal_buffer.push_back(normal.x);
                normal_buffer.push_back(normal.y);
                normal_buffer.push_back(normal.z);

                texcoord_buffer.push_back(texcoord.x);
                texcoord_buffer.push_back(texcoord.y);
            }

        }

        new_mesh.index_buffer = std::move(index_buffer);
        new_mesh.buffers.push_back({"POSITION", sizeof(Float3), DXGI_FORMAT_R32G32B32_FLOAT, std::move(vertex_buffer)});
        new_mesh.buffers.push_back({"NORMAL", sizeof(Float3), DXGI_FORMAT_R32G32B32_FLOAT, std::move(normal_buffer)});
        new_mesh.buffers.push_back({"TEXCOORD", sizeof(Float2), DXGI_FORMAT_R32G32_FLOAT, std::move(texcoord_buffer)});

        out_mesh.push_back(std::move(new_mesh));
    }

    return {std::move(out_mesh), std::move(out_material)};

}


GPUMesh convert_mesh(Mesh& mesh, ID3D11Device* pDevice)
{
    GPUMesh out_mesh;
    out_mesh.name = mesh.name;
    out_mesh.triangle_count = mesh.index_buffer.size()/3;
    out_mesh.prefered_material = mesh.prefered_material;

    auto create_buffer = [pDevice](vector<float> &cpuBuffer) -> ID3D11Buffer*
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.ByteWidth = cpuBuffer.size()*sizeof(float);

        D3D11_SUBRESOURCE_DATA init = {};
        init.pSysMem = cpuBuffer.data();

        ID3D11Buffer* out;
        pDevice->CreateBuffer(&desc, &init, &out);
        return out;
    };

    // init index buffer
    D3D11_BUFFER_DESC indexDesc = {};
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexDesc.ByteWidth = mesh.index_buffer.size()*sizeof(unsigned int);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = mesh.index_buffer.data();

    ID3D11Buffer* index_buffer;
    pDevice->CreateBuffer(&indexDesc, &indexData, &index_buffer);

    out_mesh.index_buffer = uniq_com_ptr<ID3D11Buffer>(index_buffer);

    for(auto &buffer : mesh.buffers)
    {
        ID3D11Buffer* vertBuffer = create_buffer(buffer.buffer);
        out_mesh.vertex_buffers.push_back({buffer.semantic, uniq_com_ptr<ID3D11Buffer>(vertBuffer),
                                                buffer.stride, buffer.format});
    }

    return out_mesh;
}