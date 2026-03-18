#include <shader.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

Shader_Reflection_Desc Shader::reflect(ID3DBlob* shaderBlob)
{
    Shader_Reflection_Desc out_description = {};

    
    ID3D11ShaderReflection* pReflection = NULL;
    D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), __uuidof(ID3D11ShaderReflection), (void**)&pReflection);
    
    D3D11_SHADER_DESC shader_desc = {};    
    pReflection->GetDesc(&shader_desc);

    // first, build the semantic array for the
    // input layout desc
    // we only need the semantic name and semantic index
    // the stride data will come from the mesh
    // <semantic, index>
    vector<pair<string, int>> semantics;

    for(unsigned int i = 0; i < shader_desc.InputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC signature_desc = {};
        pReflection->GetInputParameterDesc(i, &signature_desc);
        semantics.push_back({string(signature_desc.SemanticName), signature_desc.SemanticIndex});
    }

    out_description.input_semantics = semantics;

    // next, we build the texture array
    // and the const buffer array
    // we build them at the same time because the slot
    // bind info is contained in the bind desc and not
    // const buffer desc
    vector<pair<string, int>> textures;
    for(unsigned int i = 0; i < shader_desc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC bind_desc = {};
        pReflection->GetResourceBindingDesc(i, &bind_desc);
        if(bind_desc.Type == D3D_SIT_TEXTURE)
        {
            textures.push_back({bind_desc.Name, bind_desc.BindPoint});
        }

        else if(bind_desc.Type == D3D_SIT_CBUFFER)
        {
            Shader_Reflection_Desc::ConstBuffer cb = {};
            cb.name = bind_desc.Name;
            cb.slot = bind_desc.BindPoint;
            out_description.constBuffers.push_back(std::move(cb));
        }
    }

    out_description.textures = textures;

    // next, we build the variable array for each const buffer
    for(auto &constBuffer : out_description.constBuffers)
    {
        ID3D11ShaderReflectionConstantBuffer* pConstBufferReflection = pReflection->GetConstantBufferByName(constBuffer.name.data());
        
        D3D11_SHADER_BUFFER_DESC buffer_desc = {};
        pConstBufferReflection->GetDesc(&buffer_desc);

        for(unsigned int i = 0; i < buffer_desc.Variables; ++i)
        {
            ID3D11ShaderReflectionVariable* pVariableReflection = pConstBufferReflection->GetVariableByIndex(i);
            // get the variable description for its name   
            D3D11_SHADER_VARIABLE_DESC variable_desc = {};
            pVariableReflection->GetDesc(&variable_desc);

            ID3D11ShaderReflectionType* pVariableType = pVariableReflection->GetType();
            // get the type description
            D3D11_SHADER_TYPE_DESC type_desc = {};
            pVariableType->GetDesc(&type_desc);

            HLSLTypes type = HLSLTypes::FLOAT;

            if(type_desc.Class == D3D_SVC_SCALAR) type = HLSLTypes::FLOAT;

            if(type_desc.Class == D3D_SVC_VECTOR)
            {
                switch (type_desc.Columns) 
                {
                    case 2:
                        type = HLSLTypes::FLOAT2;
                        break;
                    case 3:
                        type = HLSLTypes::FLOAT3;
                        break;
                    case 4:
                        type = HLSLTypes::FLOAT4;
                        break;
                }
            }

            if(type_desc.Class == D3D_SVC_MATRIX_ROWS || type_desc.Class == D3D_SVC_MATRIX_COLUMNS)
            {
                type = HLSLTypes::MATRIX;
            }
            

            constBuffer.variables.push_back({variable_desc.Name, type});
        }
        
    }

    pReflection->Release();

    return out_description;

}