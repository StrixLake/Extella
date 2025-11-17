#define TINYOBJLOADER_IMPLEMENTATION
#include <mesh.h>
#include <vectors.h>
#include <objLoader.h>
using int3 = DirectX::XMINT3;


void stlMeshFactory(Mesh* mesh, const char* filename){
    
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


void objMeshFactory2(Mesh* mesh, const char* filename){
    
    vector<Float3> vertex;
    vector<Float3> normals;
    vector<Float2> texCoords;
    vector<int32_t> index;

    load_obj(vertex, index, normals, texCoords, filename);
    mesh->triangleCount = index.size() / 3;

    // init index buffer
    D3D11_BUFFER_DESC indexDesc = {};
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexDesc.ByteWidth = index.size()*sizeof(int32_t);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = index.data();

    mesh->device->pDevice->CreateBuffer(&indexDesc, &indexData, &mesh->pIndices);
    
    
    // init vertex buffer
    D3D11_BUFFER_DESC vertexDesc = {};
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = vertex.size()*sizeof(Float3);

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertex.data();

    mesh->device->pDevice->CreateBuffer(&vertexDesc, &vertexData, &mesh->pVertices);
    
    if (normals.size() != 0){
    // init normal buffer
    D3D11_BUFFER_DESC normalDesc = {};
    normalDesc.Usage = D3D11_USAGE_DEFAULT;
    normalDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    normalDesc.ByteWidth = normals.size()*sizeof(Float3);

    D3D11_SUBRESOURCE_DATA normalData = {};
    normalData.pSysMem = normals.data();

    mesh->device->pDevice->CreateBuffer(&normalDesc, &normalData, &mesh->pNormals);
    }
    
    if (texCoords.size() != 0){
        // init texCoords buffer if present in the obj file
        D3D11_BUFFER_DESC tcDesc = {};
        tcDesc.Usage = D3D11_USAGE_DEFAULT;
        tcDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        tcDesc.ByteWidth = texCoords.size()*sizeof(Float2);
    
        D3D11_SUBRESOURCE_DATA tcData = {};
        tcData.pSysMem = texCoords.data();
    
        mesh->device->pDevice->CreateBuffer(&tcDesc, &tcData, &mesh->pTexCoords);
    }

    return;
}


void objMeshFactory(Mesh* mesh, const char* filename){
    string inputfile(filename);
    tinyobj::ObjReaderConfig config;
    config.vertex_color = false;

    tinyobj::ObjReader reader;
    reader.ParseFromFile(inputfile, config);

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
    unordered_map<Float5, int> superIndexMap;
    vector<int> superIndex;
    vector<Float5> vertexTexPair;
    // put each vertex, texture pair in the vector and
    // in the subsequent loop, check if that pair already
    // is in there, if so put the index of it in index array
    for(tinyobj::index_t ind: shape.mesh.indices){
        Float5 pair = {/*vertex = */{/*x = */attribute.vertices[ind.vertex_index*3 ], /*y = */attribute.vertices[ind.vertex_index*3 +1], 
                          /*z  =*/attribute.vertices[ind.vertex_index*3 +2]}
                          ,/*texcord = */{/*x = */attribute.texcoords[ind.texcoord_index*2], /*y = */attribute.texcoords[ind.texcoord_index*2+1]}};

        if(superIndexMap.find(pair) == superIndexMap.end()){
            vertexTexPair.push_back(pair);
            superIndexMap[pair] = vertexTexPair.size() -1;
            superIndex.push_back(superIndexMap[pair]);
        }
        else{
            superIndex.push_back(superIndexMap[pair]);
        }
    }

    // calculate the normal for each triangle
    // assuming the index buffer gives triangle vertices in clock wise order
    vector<Float3> normals;
    normals.resize(vertexTexPair.size());
    unordered_map<Float3, Norm> vertNorm; // per vertex normal average
    for(int i = 0; i < superIndex.size() /3; i++){
        int3 triangle = {/*x*/superIndex[i*3], /*y*/superIndex[i*3+1], /*z*/superIndex[i*3+2]};
        Float3 edge1 = vertexTexPair[triangle.y].vertex - vertexTexPair[triangle.x].vertex;
        Float3 edge2 = vertexTexPair[triangle.z].vertex - vertexTexPair[triangle.x].vertex;
        Float3 normal = Float3::normalize(Float3::cross(edge1, edge2));
        
        if(vertNorm.find(vertexTexPair[triangle.x].vertex) == vertNorm.end()){
            vertNorm[vertexTexPair[triangle.x].vertex] = {/*normal*/ normal, /*count*/ 1};
            normals[triangle.x] = normal;
        }
        else{
            Norm runningNormal = vertNorm[vertexTexPair[triangle.x].vertex];
            runningNormal.normal = (runningNormal.normal*runningNormal.count + normal)/(runningNormal.count+1);
            runningNormal.count += 1;
            vertNorm[vertexTexPair[triangle.x].vertex] = runningNormal;
            normals[triangle.x] = runningNormal.normal;
        }
        if(vertNorm.find(vertexTexPair[triangle.y].vertex) == vertNorm.end()){
            vertNorm[vertexTexPair[triangle.y].vertex] = {/*normal*/ normal, /*count*/ 1};
            normals[triangle.y] = normal;
        }
        else{
            Norm runningNormal = vertNorm[vertexTexPair[triangle.y].vertex];
            runningNormal.normal = (runningNormal.normal*runningNormal.count + normal)/(runningNormal.count+1);
            runningNormal.count += 1;
            vertNorm[vertexTexPair[triangle.y].vertex] = runningNormal;
            normals[triangle.y] = runningNormal.normal;
        }
        if(vertNorm.find(vertexTexPair[triangle.z].vertex) == vertNorm.end()){
            vertNorm[vertexTexPair[triangle.z].vertex] = {/*normal*/ normal, /*count*/ 1};
            normals[triangle.z] = normal;
        }
        else{
            Norm runningNormal = vertNorm[vertexTexPair[triangle.z].vertex];
            runningNormal.normal = (runningNormal.normal*runningNormal.count + normal)/(runningNormal.count+1);
            runningNormal.count += 1;
            vertNorm[vertexTexPair[triangle.z].vertex] = runningNormal;
            normals[triangle.z] = runningNormal.normal;
        }
    }

    // init vertex buffer
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = vertexTexPair.size()*sizeof(Float5);
    
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertexTexPair.data();

    mesh->device->pDevice->CreateBuffer(&desc, &InitData, &mesh->pVertices);

    // init index buffer
    D3D11_BUFFER_DESC desc2 = {};
    desc2.Usage = D3D11_USAGE_DEFAULT;
    desc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc2.ByteWidth = superIndex.size()*sizeof(int);
    
    D3D11_SUBRESOURCE_DATA InitData2;
    InitData2.pSysMem = superIndex.data();

    mesh->device->pDevice->CreateBuffer(&desc2, &InitData2, &mesh->pIndices);
    
    // init normal buffer
    D3D11_BUFFER_DESC desc3 = {};
    desc3.Usage = D3D11_USAGE_DEFAULT;
    desc3.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc3.ByteWidth = normals.size()*sizeof(Float3);
    
    D3D11_SUBRESOURCE_DATA InitData3;
    InitData3.pSysMem = normals.data();

    mesh->device->pDevice->CreateBuffer(&desc3, &InitData3, &mesh->pNormals);


    return;

}

Mesh::Mesh(const char* filename, Shader* shader, FileType type){
    this->shaders = shader;
    this->device = shader->device;
    switch (type) {
        case stl:
            stlMeshFactory(this, filename);
            break;
        case obj:
            objMeshFactory2(this, filename);
            break;
    }

    this->device->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    return;
}


Mesh::~Mesh(){
    pVertices->Release();
    pNormals->Release();
    if (pTexCoords != NULL) pTexCoords->Release();
    pIndices->Release();
}