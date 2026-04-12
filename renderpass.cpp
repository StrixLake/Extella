#include <renderpass.h>

RenderPass::RenderPass(GPUMesh* mesh, PipeLine* pipeline, Material* material, ResourceManager* manager, Shader* shaderManager, const unordered_map<wstring, float>& global_variables, ID3D11Device* pDevice)
    : material(material), mesh(mesh), pipeline(pipeline)
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
                ptxtr = manager->getTexture2D("default");
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
    auto constBufferInit = [&global_variables, manager](const Shader_Reflection_Desc& reflectionDesc, pair<ID3D11Buffer*, vector<const float*>>& HostVar)
    {
        for(auto& constBuffer : reflectionDesc.constBuffers)
        {
            if(constBuffer.name == "HostVariable")
            {
                HostVar.first = manager->getConstBuffer();
                
                for(auto& variable : constBuffer.variables)
                {
                    wstring hostVariable = wstring(variable.name.begin(), variable.name.end());
                    HostVar.second.push_back(&global_variables.at(hostVariable));
                    
                }
            }
            if(constBuffer.name == "Material")
            {
                // implementation not complete. I am tired.
                // I just want to render a frame first   
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
    for(auto semantic : vertexReflection.input_semantics)
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
        vertex_buffers.push_back(mesh->vertex_buffers[semantic.first].vertex_buffer.get());
        stride.push_back(mesh->vertex_buffers[semantic.first].stride);
        input_layout.push_back(layout);
    }

    ID3DBlob* pVertexShaderBlob = shaderManager->getVertexShaderBlob(pipeline->vertexShader);
    ID3D11InputLayout* pLayout;
    pDevice->CreateInputLayout(input_layout.data(), input_layout.size(), pVertexShaderBlob->GetBufferPointer(),
                                pVertexShaderBlob->GetBufferSize(), &pLayout);

    pInputLayout = uniq_com_ptr<ID3D11InputLayout>(pLayout);
    
}

