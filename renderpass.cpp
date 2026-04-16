#include <renderpass.h>

RenderPass::RenderPass(GPUMesh* mesh, PipeLine* pipeline, Material* material, ResourceManager* manager, Shader* shaderManager, const unordered_map<wstring, float>& global_variables, ID3D11Device* pDevice)
    : material(material), mesh(mesh), pipeline(pipeline),
      rotationX(global_variables.at(L"rotationX")), rotationY(global_variables.at(L"rotationY"))
{
    assert(mesh != NULL);
    assert(pipeline != NULL);
    assert(material != NULL);
    assert(shaderManager != NULL);

    // get the shader reflection description of each stage
    const Shader_Reflection_Desc& vertexReflection = shaderManager->getShaderReflection({pipeline->vertexShader, "vertex"});
    const Shader_Reflection_Desc& hullReflection = shaderManager->getShaderReflection({pipeline->hullShader, "hull"});
    const Shader_Reflection_Desc& domainReflection = shaderManager->getShaderReflection({pipeline->domainShader, "domain"});
    const Shader_Reflection_Desc& geometryReflection = shaderManager->getShaderReflection({pipeline->geometryShader, "geometry"});
    const Shader_Reflection_Desc& pixelReflection = shaderManager->getShaderReflection({pipeline->pixelShader, "pixel"});

    // for each reflection description, get the texture name and it's slot
    // check if that texture exists in materials
    // if not, get the default texture
    // if it does, get the name of that texture from the material
    // use that handle to get the texture from the resource manager
    // and create it's srv and put it with the associated slot in the srv array
    // for that shader stage
    auto textureArrayInit = [manager, material, pDevice](const Shader_Reflection_Desc& reflectionDesc, vector<pair<int, uniq_com_ptr<ID3D11ShaderResourceView>>>& textureArray)
    {
        // texture is <string(name), int(slot)>
        for(auto texture : reflectionDesc.textures)
        {
            ID3D11Texture2D* ptxtr;
            if(material->material_textures.find(texture.first) == material->material_textures.end())
            {
                // material not found, use the default texture from manager
                ptxtr = manager->getTexture2D("mesh/white.png");
            }
            else
            {
                ptxtr = manager->getTexture2D(material->material_textures[texture.first]);
            }

            ID3D11ShaderResourceView* pSRV;
            pDevice->CreateShaderResourceView(ptxtr, NULL, &pSRV);
            textureArray.push_back({texture.second,uniq_com_ptr<ID3D11ShaderResourceView>(pSRV)});
        }
    };

    textureArrayInit(vertexReflection, vertexTextures);
    textureArrayInit(hullReflection, hullTextures);
    textureArrayInit(domainReflection, domainTextures);
    textureArrayInit(geometryReflection, geometryTextures);
    textureArrayInit(pixelReflection, pixelTextures);
    
    // similar to textureArrayInit, intialise the const buffer
    // array for all shader stages
    // implementation 
    auto constBufferInit = [&global_variables, manager, this](const Shader_Reflection_Desc& reflectionDesc, tuple<int, ID3D11Buffer*, vector<const float*>>& HostVar)
    {
        for(auto& constBuffer : reflectionDesc.constBuffers)
        {
            if(constBuffer.name == "HostVariable")
            {
                get<0>(HostVar) = constBuffer.slot;
                get<1>(HostVar) = manager->getConstBuffer();
                
                for(auto& variable : constBuffer.variables)
                {
                    wstring hostVariable = wstring(variable.name.begin(), variable.name.end());
                    get<2>(HostVar).push_back(&global_variables.at(hostVariable));
                    
                }
            }
            if(constBuffer.name == "Material")
            {
                materialBuffer = manager->getConstBuffer();
            }
        }
    };
    
    constBufferInit(vertexReflection, vertexHostVar);
    constBufferInit(hullReflection, hullHostVar);
    constBufferInit(domainReflection, domainHostVar);
    constBufferInit(geometryReflection, geometryHostVar);
    constBufferInit(pixelReflection, pixelHostVar);
    
    transformationBuffer = manager->getConstBuffer();


    // now we need to build the input assembler layout
    // semantic is pair<string semantic, int index>
    vector<D3D11_INPUT_ELEMENT_DESC> input_layout;
    
    unsigned int i = 0;
    for(auto& semantic : vertexReflection.input_semantics)
    {
        D3D11_INPUT_CLASSIFICATION classification = D3D11_INPUT_PER_VERTEX_DATA;
        unsigned int instance_step = 0;
        if(semantic.first.at(0) == 'i') 
        {
            classification = D3D11_INPUT_PER_INSTANCE_DATA;
            instance_step = 1;
        }
        D3D11_INPUT_ELEMENT_DESC layout = {semantic.first.data(), static_cast<UINT>(semantic.second),
                                            mesh->vertex_buffers.at(semantic.first).format, i,
                                            0, classification,
                                            instance_step};
        ++i;
        vertex_buffers.push_back(mesh->vertex_buffers.at(semantic.first).vertex_buffer.get());
        stride.push_back(mesh->vertex_buffers.at(semantic.first).stride);
        input_layout.push_back(layout);
    }

    ID3DBlob* pVertexShaderBlob = shaderManager->getVertexShaderBlob(pipeline->vertexShader);
    ID3D11InputLayout* pLayout;
    pDevice->CreateInputLayout(input_layout.data(), input_layout.size(), pVertexShaderBlob->GetBufferPointer(),
                                pVertexShaderBlob->GetBufferSize(), &pLayout);

    pInputLayout = uniq_com_ptr<ID3D11InputLayout>(pLayout);
    
}

void RenderPass::execute(const DirectX::XMMATRIX& ViewProjMatrix, ID3D11DeviceContext* pContext)
{
    pipeline->setPipelineState(pContext);

    // set the SRVs
    for(auto &srv : vertexTextures)
    {
        ID3D11ShaderResourceView* pSrv = srv.second.get();
        pContext->VSSetShaderResources(srv.first, 1, &pSrv);
    }
    for(auto &srv : hullTextures)
    {
        ID3D11ShaderResourceView* pSrv = srv.second.get();
        pContext->HSSetShaderResources(srv.first, 1, &pSrv);
    }
    for(auto &srv : domainTextures)
    {
        ID3D11ShaderResourceView* pSrv = srv.second.get();
        pContext->DSSetShaderResources(srv.first, 1, &pSrv);
    }
    for(auto &srv : geometryTextures)
    {
        ID3D11ShaderResourceView* pSrv = srv.second.get();
        pContext->GSSetShaderResources(srv.first, 1, &pSrv);
    }
    for(auto &srv : pixelTextures)
    {
        ID3D11ShaderResourceView* pSrv = srv.second.get();
        pContext->PSSetShaderResources(srv.first, 1, &pSrv);
    }


    // update and set the Transform const buffer for all stages
    DirectX::XMMATRIX transform = mesh->worldMatrix 
                                * DirectX::XMMatrixRotationY(rotationX/100)
                                * DirectX::XMMatrixRotationX(rotationY/100)
                                * ViewProjMatrix;

    DirectX::XMMATRIX transformArray[2] = {DirectX::XMMatrixTranspose(transform)};

    pContext->UpdateSubresource(transformationBuffer, 0, NULL, transformArray, 0, 0);

    pContext->VSSetConstantBuffers(0, 1, &transformationBuffer);
    pContext->HSSetConstantBuffers(0, 1, &transformationBuffer);
    pContext->DSSetConstantBuffers(0, 1, &transformationBuffer);
    pContext->GSSetConstantBuffers(0, 1, &transformationBuffer);
    pContext->PSSetConstantBuffers(0, 1, &transformationBuffer);

    // update and set the host variable const buffer for all stages
    auto updateHostVarArray = [pContext](const tuple<int, ID3D11Buffer*, vector<const float*>> &hostVar)
    {
        if(get<1>(hostVar) == NULL) return;
        vector<float> variableArray;
        for(const float* i : get<2>(hostVar)) variableArray.push_back(*i);
        variableArray.resize(128/sizeof(float));
        pContext->UpdateSubresource(get<1>(hostVar), 0, NULL, variableArray.data(), 0, 0);
    };

    updateHostVarArray(vertexHostVar);
    updateHostVarArray(hullHostVar);
    updateHostVarArray(domainHostVar);
    updateHostVarArray(geometryHostVar);
    updateHostVarArray(pixelHostVar);

    // update the material constant buffer
    if(materialBuffer != NULL)
    {
        pContext->UpdateSubresource(materialBuffer, 0, NULL, &material->constMaterial, 0, 0);
        pContext->PSSetConstantBuffers(7, 1, &materialBuffer);
    }

    if(get<1>(vertexHostVar) != NULL) pContext->VSSetConstantBuffers(get<0>(vertexHostVar), 1, &get<1>(vertexHostVar));
    if(get<1>(hullHostVar) != NULL) pContext->HSSetConstantBuffers(get<0>(hullHostVar), 1, &get<1>(hullHostVar));
    if(get<1>(domainHostVar) != NULL) pContext->DSSetConstantBuffers(get<0>(domainHostVar), 1, &get<1>(domainHostVar));
    if(get<1>(geometryHostVar) != NULL) pContext->GSSetConstantBuffers(get<0>(geometryHostVar), 1, &get<1>(geometryHostVar));
    if(get<1>(pixelHostVar) != NULL) pContext->PSSetConstantBuffers(get<0>(pixelHostVar), 1, &get<1>(pixelHostVar));

    // setup the input assembler
    // set the input layout and the vertex buffers
    vector<unsigned int> offsets( vertex_buffers.size(), 0);
    pContext->IASetInputLayout(pInputLayout.get());
    pContext->IASetIndexBuffer(mesh->index_buffer.get(), DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetVertexBuffers(0, vertex_buffers.size(), vertex_buffers.data(), stride.data(), offsets.data());

    pContext->DrawIndexedInstanced(mesh->triangle_count*3, mesh->instance_count, 0, 0, 0);
}