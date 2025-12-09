#define TINYOBJLOADER_IMPLEMENTATION
#include <mesh.h>
#include <vectors.h>
#include <objLoader.h>
#include <stb_image.h>
#include <filesystem>
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

 void constuctTextures(Mesh* mesh, std::filesystem::path textureFile){
        int x, y, z;
        uint8_t* image2 = stbi_load(textureFile.string().data(), &x, &y, &z, 3);
        uint8_t* image = (uint8_t*) malloc(x*y*4);
        uint8_t* image1 = (uint8_t*) malloc(x*y*4);
        uint8_t* imaget = image;

        // invert the image as stbi loads the image upside down
        for (int i = 0; i < x*y*3; i += 3){
            memcpy(imaget, image2, 3);
            imaget[3] = 255;
            imaget += 4;
            image2 += 3;
        }
        image1 += (x*y*4 -1);
        for(int i = 0; i < y; ++i){
            image1 -= x*4;
            memcpy(image1, image, x*4);
            image += x*4;
        }
        

        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        mesh->device->pDevice->CreateSamplerState( &sampDesc, &mesh->pSampler);

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = x;
        desc.Height = y;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.SampleDesc = {1,0};
        D3D11_SUBRESOURCE_DATA init;
        init.pSysMem = image1;
        init.SysMemPitch = x*4;
        mesh->device->pDevice->CreateTexture2D(&desc, &init, &mesh->pTexture);
        mesh->device->pDevice->CreateShaderResourceView(mesh->pTexture, NULL, &mesh->pTextureView);

        return;
 }

void createDefaultLayout(Mesh* mesh){

    D3D11_INPUT_ELEMENT_DESC layout[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                            0, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
                                            1, 0, 
                                            D3D11_INPUT_PER_VERTEX_DATA, 0},
                                            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
                                                 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    mesh->device->pDevice->CreateInputLayout(layout, 3, mesh->shaders->getShaderBlob(L"shaders/vshader.cso")->GetBufferPointer(), 
                                                   mesh->shaders->getShaderBlob(L"shaders/vshader.cso")->GetBufferSize(), &mesh->pLayout);

    return;
}

Mesh::Mesh(const char* filename, Shader* shader, FileType type, const wchar_t* textureFile){
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

    device->pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // create the camera transformation buffers
    D3D11_BUFFER_DESC cBuffer = {};
    cBuffer.Usage = D3D11_USAGE_DEFAULT;
    cBuffer.ByteWidth = sizeof(XMMATRIX)*3;
    cBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->pDevice->CreateBuffer(&cBuffer, NULL, &transformBuffer);

    if (textureFile != NULL){
        constuctTextures(this, textureFile);
    }

    createDefaultLayout(this);

    return;
}

void Mesh::Draw(array<XMMATRIX,3> camera, float time){
    
    extension->extensionWork(camera, time);

    device->pContext->UpdateSubresource(transformBuffer, 0, NULL, 
                                            camera.data(), 0, 0);
        
    device->pContext->IASetInputLayout(pLayout);

    UINT offset = 0;
    UINT stride = sizeof(DirectX::XMFLOAT3);
    device->pContext->IASetVertexBuffers(0, 1, &pVertices, &stride, &offset);
    stride = sizeof(DirectX::XMFLOAT2);
    device->pContext->IASetVertexBuffers(1, 1, &pTexCoords, &stride, &offset);
    stride = sizeof(DirectX::XMFLOAT3);
    device->pContext->IASetVertexBuffers(2, 1, &pNormals, &stride, &offset);

    device->pContext->IASetIndexBuffer(pIndices, DXGI_FORMAT_R32_UINT, 0);

    device->pContext->VSSetShader(shaders->getVertexShader(L"shaders/vshader.cso"), NULL, 0);
    device->pContext->PSSetShader(shaders->getPixelShader(L"shaders/pshader.cso"), NULL, 0);

    device->pContext->VSSetConstantBuffers(0, 1, &transformBuffer);

    device->pContext->PSSetSamplers(0, 1, &pSampler);
    device->pContext->PSSetShaderResources(0, 1, &pTextureView);

    device->pContext->DrawIndexed(triangleCount*3, 0, 0);
}

Mesh::~Mesh(){
    if (pVertices != NULL) pVertices->Release();
    if (pNormals != NULL) pNormals->Release();
    if (pTexCoords != NULL) pTexCoords->Release();
    if (pIndices != NULL) pIndices->Release();

    if (pSampler != NULL) pSampler->Release();
    if (pTexture != NULL) pTexture->Release();
    if (pTextureView != NULL) pTextureView->Release();
    if (transformBuffer != NULL) transformBuffer->Release();
    if (pLayout != NULL) pLayout->Release();

    if(extension != NULL) delete extension;
    if (next != NULL) delete next;
}

