#define TINYOBJLOADER_IMPLEMENTATION
#include <mesh.h>
#include <fstream>
#include <string>
#include <tiny_obj_loader.h>
using std::vector;
using std::ifstream;
using std::string;

// have to use a temp struct
// because XMFLOAT3 cannot be used 
// inside unordered_map
struct Float3{
    float x;
    float y;
    float z;

    bool operator==(const Float3& other) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }

    Float3 operator/(int other){
        this->x /= other;
        this->y /= other;
        this->z /= other;
        return *this;
    }

    Float3 operator+(const Float3& other){
        Float3 self = {this->x + other.x, this->y + other.y,this->z + other.z,};
        return self;
    }

    Float3 operator*(int other){
        Float3 self = {this->x*other, this->y*other, this->z*other};
        return self;
    }
};

struct Float2{
    float x;
    float y;

    bool operator==(const Float2& other) const noexcept {
        return x == other.x && y == other.y;
    }
};

struct VertexTex{
    Float3 vertex;
    Float2 TexCord;

    bool operator==(const VertexTex& other) const noexcept {
        return vertex == other.vertex && TexCord == other.TexCord;
    }
};

namespace std {
    template<>
    struct hash<Float2> {
        std::size_t operator()(const Float2& f) const noexcept {
            std::size_t hx = std::hash<float>{}(f.x);
            std::size_t hy = std::hash<float>{}(f.y);

            std::size_t seed = hx;
            seed ^= 3*hy << 3;
            return seed;
        }
        
    };
    template<>
    struct hash<Float3> {
        std::size_t operator()(const Float3& f) const noexcept {
            std::size_t hx = std::hash<float>{}(f.x);
            std::size_t hy = std::hash<float>{}(f.y);
            std::size_t hz = std::hash<float>{}(f.z);

            std::size_t seed = hx;
            seed ^= hy << 1;
            seed ^= hz << 3;
            return seed;
        }
        
    };
    template<>
    struct hash<VertexTex> {
        std::size_t operator()(const VertexTex& f) const noexcept {
            std::size_t hx = std::hash<Float3>{}(f.vertex);
            std::size_t hy = std::hash<Float2>{}(f.TexCord);
            

            std::size_t seed = hx;
            seed ^= 2*hy << 2;
            return seed;
        }
        
    };
}

void load_stl(vector<Float3> &vertex, vector<uint32_t> &index, vector<Float3> &normal, const char* filename){
    // read the stl file
    std::ifstream vertexData(filename, std::ios::binary);
    vertexData.seekg(80);
    char num[4];
    vertexData.read(num, 4);
    int triangleCount = *(int*)num;

    unordered_map<Float3, uint32_t> vertex_; // hold the index of the vertex in the vector
    unordered_map<Float3, uint32_t> normals_; // hold the number of normals of that vertex
    
    for (int i = 0; i < triangleCount; ++i){
        Float3 triangle[4];
        vertexData.read((char*)&triangle, sizeof(triangle));
        vertexData.ignore(2);

        // all 3 vertex of the triangle assume the name
        // normal as its plane
        // if a vertex is not already in the vertex vector
        // then push_back it into the vector and 
        // make a key:value pair of the vertex and its
        // intex (its position in the vector) in the unordered_map
        // and also add the index in the index vector
        // the map can be used to check if the vertex is already
        // in the vector and if it is then take its position 
        // the in vector (from the map)
        // and put it in the index vector
        // the normal is averaged

        for(int j = 0; j < 3; ++j){
            if(vertex_.find(triangle[j+1]) == vertex_.end()){
                vertex.push_back(triangle[j+1]);
                vertex_[triangle[j+1]] = vertex.size() -1;
                index.push_back(vertex_[triangle[j+1]]);
                
                normal.push_back(triangle[0]);
                normals_[triangle[0]]++;
            }
            else{
                index.push_back(vertex_[triangle[j+1]]);
                normal[vertex_[triangle[j+1]]] = (triangle[0] + normal[vertex_[triangle[j+1]]]*normals_[triangle[0]])/(normals_[triangle[0]] +1);
                normals_[triangle[0]]++;
            }
        }
    }
    return;
}


void stlMeshFactory(Mesh* mesh, const char* filename, Shader* shader){
    
    vector<Float3> vertex;
    vector<Float3> normal;
    vector<uint32_t> index;

    load_stl(vertex, index, normal, filename);
    mesh->triangleCount = index.size()/3;
    
    // init vertex buffer
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = vertex.size()*sizeof(Float3);
    
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertex.data();

    mesh->device->pDevice->CreateBuffer(&desc, &InitData, &mesh->pVertices);
    
    // init normals buffer
    D3D11_BUFFER_DESC desc2 = {};
    desc2.Usage = D3D11_USAGE_DEFAULT;
    desc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc2.ByteWidth = normal.size()*sizeof(Float3);
    
    D3D11_SUBRESOURCE_DATA InitData2;
    InitData2.pSysMem = normal.data();

    mesh->device->pDevice->CreateBuffer(&desc2, &InitData2, &mesh->pNormals);
    
    // init index buffer
    D3D11_BUFFER_DESC desc3 = {};
    desc3.Usage = D3D11_USAGE_DEFAULT;
    desc3.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc3.ByteWidth = index.size()*sizeof(int);
    
    D3D11_SUBRESOURCE_DATA InitData3;
    InitData3.pSysMem = index.data();

    mesh->device->pDevice->CreateBuffer(&desc3, &InitData3, &mesh->pIndices);
    
    
    return;
}




void objMeshFactory(Mesh* mesh, const char* filename){
    string inputfile(filename);
    tinyobj::ObjReaderConfig config;
    config.vertex_color = false;

    tinyobj::ObjReader reader;
    bool notfail = reader.ParseFromFile(inputfile, config);

    vector<tinyobj::shape_t> shapes = reader.GetShapes();
    tinyobj::shape_t shape = shapes[0];

    tinyobj::attrib_t attribute = reader.GetAttrib();

    // need to copy vector<index_t> to another
    // vector for indices of vertices
    vector<int> index;
    for (tinyobj::index_t i : shape.mesh.indices){
        index.push_back(i.vertex_index);
    }

    mesh->triangleCount = index.size() / 3;

    // i don't know what to call it
    unordered_map<VertexTex, int> superIndexMap;
    vector<int> superIndex;
    vector<VertexTex> vertexTexPair;
    // put each vertex, texture pair in the vector and
    // in the subsequent loop, check if that pair already
    // is in there, if so put the index of it in index array
    for(tinyobj::index_t ind: shape.mesh.indices){
        VertexTex pair = {{attribute.vertices[ind.vertex_index*3 ], attribute.vertices[ind.vertex_index*3 +1], 
                          attribute.vertices[ind.vertex_index*3 +2]}
                          ,{attribute.texcoords[ind.texcoord_index*2], attribute.texcoords[ind.texcoord_index*2+1]}};

        if(superIndexMap.find(pair) == superIndexMap.end()){
            vertexTexPair.push_back(pair);
            superIndexMap[pair] = vertexTexPair.size() -1;
            superIndex.push_back(superIndexMap[pair]);
        }
        else{
            superIndex.push_back(superIndexMap[pair]);
        }
    }

    int max = 0;
    for (tinyobj::index_t ind: shape.mesh.indices){
        if (ind.vertex_index >= max){
            max = ind.vertex_index;
        }
    }

    

    // init vertex buffer
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = vertexTexPair.size()*sizeof(VertexTex);
    
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertexTexPair.data();

    mesh->device->pDevice->CreateBuffer(&desc, &InitData, &mesh->pVertices);

    // init index buffer
    D3D11_BUFFER_DESC desc3 = {};
    desc3.Usage = D3D11_USAGE_DEFAULT;
    desc3.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc3.ByteWidth = superIndex.size()*sizeof(int);
    
    D3D11_SUBRESOURCE_DATA InitData3;
    InitData3.pSysMem = superIndex.data();

    mesh->device->pDevice->CreateBuffer(&desc3, &InitData3, &mesh->pIndices);


    return;

}

Mesh::Mesh(const char* filename, Shader* shader, FileType type){
    this->shaders = shader;
    this->device = shader->device;
    switch (type) {
        case stl:
            stlMeshFactory(this, filename, shader);
            break;
        case obj:
            objMeshFactory(this, filename);
            break;
    }

    this->device->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    return;
}


Mesh::~Mesh(){
    pVertices->Release();
    if (pNormals != NULL) pNormals->Release();
    if (pTexCoords != NULL) pTexCoords->Release();
    pIndices->Release();
}