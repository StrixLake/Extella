#include <shader.h>
#include <d3dcompiler.h>

inline void Handle_Err_Msg(ID3DBlob* errorMsg)
{
    if(errorMsg != NULL)
    {
        char* error = (char*)errorMsg->GetBufferPointer();
        std::string msg(error, errorMsg->GetBufferSize());
        OutputDebugString(msg.data());
        errorMsg->Release();
    }
}

Shader::Shader(ID3D11Device* device){
    this->pDevice = device;
    return;
}

ID3D11VertexShader* Shader::getVertexShader(wstring ShaderFileName){

    // if the shader isn't loaded, then compile and create the shader
    if(vertexShaders.find(ShaderFileName) == vertexShaders.end()){
        
        wstring ShaderFullName = L"shaders/" + ShaderFileName + L".hlsl";

        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = NULL;
        ID3DBlob* errorMsg;
        D3DCompileFromFile(ShaderFullName.data(), NULL, NULL, "VS_MAIN", "vs_5_0", 0, 0, &compiledShader, &errorMsg);

        Handle_Err_Msg(errorMsg);

        if(compiledShader == NULL) return NULL;

        ID3D11VertexShader* vshader;
        pDevice->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &vshader);

        vertexShaders[ShaderFileName] = unique_ptr<ID3D11VertexShader, Deleter<ID3D11VertexShader*>>(vshader);
        compiledShader->Release();
    }

    return vertexShaders[ShaderFileName].get();
}

ID3D11PixelShader* Shader::getPixelShader(wstring ShaderFileName){

    if(pixelShaders.find(ShaderFileName) == pixelShaders.end()){
        
        wstring ShaderFullName = L"shaders/" + ShaderFileName + L".hlsl";

        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = NULL;
        ID3DBlob* errorMsg;
        D3DCompileFromFile(ShaderFullName.data(), NULL, NULL, "PS_MAIN", "ps_5_0", 0, 0, &compiledShader, &errorMsg);

        Handle_Err_Msg(errorMsg);

        if(compiledShader == NULL) return NULL;

        ID3D11PixelShader* pshader;
        pDevice->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &pshader);

        pixelShaders[ShaderFileName] = unique_ptr<ID3D11PixelShader, Deleter<ID3D11PixelShader*>>(pshader);
        compiledShader->Release();
    }

    return pixelShaders[ShaderFileName].get();
}

ID3D11HullShader* Shader::getHullShader(wstring ShaderFileName){
     
    if(ShaderFileName == L"") return NULL;
    
    if(hullShaders.find(ShaderFileName) == hullShaders.end()){
        
        wstring ShaderFullName = L"shaders/" + ShaderFileName + L".hlsl";

        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = NULL;
        ID3DBlob* errorMsg;
        D3DCompileFromFile(ShaderFullName.data(), NULL, NULL, "HS_MAIN", "hs_5_0", 0, 0, &compiledShader, &errorMsg);

        Handle_Err_Msg(errorMsg);

        if(compiledShader == NULL) return NULL;

        ID3D11HullShader* hshader;
        pDevice->CreateHullShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &hshader);

        hullShaders[ShaderFileName] = unique_ptr<ID3D11HullShader, Deleter<ID3D11HullShader*>>(hshader);
        compiledShader->Release();
    }

    return hullShaders[ShaderFileName].get();   
}

ID3D11DomainShader* Shader::getDomainShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;
    

    if(domainShaders.find(ShaderFileName) == domainShaders.end()){
        
        wstring ShaderFullName = L"shaders/" + ShaderFileName + L".hlsl";

        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = NULL;
        ID3DBlob* errorMsg;
        D3DCompileFromFile(ShaderFullName.data(), NULL, NULL, "DS_MAIN", "ds_5_0", 0, 0, &compiledShader, &errorMsg);

        Handle_Err_Msg(errorMsg);

        if(compiledShader == NULL) return NULL;

        ID3D11DomainShader* dshader;
        pDevice->CreateDomainShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &dshader);

        domainShaders[ShaderFileName] = unique_ptr<ID3D11DomainShader, Deleter<ID3D11DomainShader*>>(dshader);
        compiledShader->Release();
    }

    return domainShaders[ShaderFileName].get();   
}

ID3D11GeometryShader* Shader::getGeometryShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;


    if(geometryShaders.find(ShaderFileName) == geometryShaders.end()){
        
        wstring ShaderFullName = L"shaders/" + ShaderFileName + L".hlsl";

        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = NULL;
        ID3DBlob* errorMsg;
        D3DCompileFromFile(ShaderFullName.data(), NULL, NULL, "GS_MAIN", "gs_5_0", 0, 0, &compiledShader, &errorMsg);

        Handle_Err_Msg(errorMsg);

        if(compiledShader == NULL) return NULL;

        ID3D11GeometryShader* gshader;
        pDevice->CreateGeometryShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &gshader);

        geometryShaders[ShaderFileName] = unique_ptr<ID3D11GeometryShader, Deleter<ID3D11GeometryShader*>>(gshader);
        compiledShader->Release();
    }

    return geometryShaders[ShaderFileName].get();   
}

ID3DBlob* Shader::getVertexShaderBlob(wstring ShaderFileName){

    if(vertexShaderBlob.find(ShaderFileName) == vertexShaderBlob.end())
    {
        wstring ShaderFullName = L"shaders/" + ShaderFileName + L".hlsl";
        
        ID3DBlob* compiledShader = NULL;
        ID3DBlob* errorMsg;
        D3DCompileFromFile(ShaderFullName.data(), NULL, NULL, "VS_MAIN", "vs_5_0", 0, 0, &compiledShader, &errorMsg);

        Handle_Err_Msg(errorMsg);

        if(compiledShader == NULL) return NULL;

        vertexShaderBlob[ShaderFileName] = unique_ptr<ID3DBlob, Deleter<ID3DBlob*>>(compiledShader);
    }

    return vertexShaderBlob[ShaderFileName].get();
}

void Shader::HotReload()
{
    vertexShaders.clear();
    pixelShaders.clear();
    hullShaders.clear();
    domainShaders.clear();
    geometryShaders.clear();
    vertexShaderBlob.clear();
}
