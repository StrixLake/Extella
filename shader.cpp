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

ID3DBlob* compileBlob(wstring ShaderFileName, const char entryPoint[], const char target[])
{
    wstring ShaderFullName = L"shaders/" + ShaderFileName + L".hlsl";

    // compile the loaded source code to a d3dblob
    ID3DBlob* compiledShader = NULL;
    ID3DBlob* errorMsg;
    D3DCompileFromFile(ShaderFullName.data(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, 0, 0, &compiledShader, &errorMsg);

    Handle_Err_Msg(errorMsg);

    return compiledShader;
}

Shader::Shader(ID3D11Device* device){
    this->pDevice = device;
    return;
}

ID3D11VertexShader* Shader::getVertexShader(wstring ShaderFileName){

    // if the shader isn't loaded, then compile and create the shader
    if(vertexShaders.find(ShaderFileName) == vertexShaders.end()){

        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "VS_MAIN", "vs_5_0");

        ID3D11VertexShader* vshader;
        pDevice->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &vshader);

        vertexShaders[ShaderFileName] = unique_ptr<ID3D11VertexShader, Deleter<ID3D11VertexShader*>>(vshader);
        compiledShader->Release();
    }

    return vertexShaders[ShaderFileName].get();
}

ID3D11PixelShader* Shader::getPixelShader(wstring ShaderFileName){

    if(pixelShaders.find(ShaderFileName) == pixelShaders.end())
    {
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "PS_MAIN", "ps_5_0");

        ID3D11PixelShader* pshader;
        pDevice->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &pshader);

        pixelShaders[ShaderFileName] = unique_ptr<ID3D11PixelShader, Deleter<ID3D11PixelShader*>>(pshader);
        compiledShader->Release();
    }

    return pixelShaders[ShaderFileName].get();
}

ID3D11HullShader* Shader::getHullShader(wstring ShaderFileName){
     
    if(ShaderFileName == L"") return NULL;
    
    if(hullShaders.find(ShaderFileName) == hullShaders.end())
    {    
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "HS_MAIN", "hs_5_0");

        ID3D11HullShader* hshader;
        pDevice->CreateHullShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &hshader);

        hullShaders[ShaderFileName] = unique_ptr<ID3D11HullShader, Deleter<ID3D11HullShader*>>(hshader);
        compiledShader->Release();
    }

    return hullShaders[ShaderFileName].get();   
}

ID3D11DomainShader* Shader::getDomainShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;
    

    if(domainShaders.find(ShaderFileName) == domainShaders.end())
    {    
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "DS_MAIN", "ds_5_0");

        ID3D11DomainShader* dshader;
        pDevice->CreateDomainShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &dshader);

        domainShaders[ShaderFileName] = unique_ptr<ID3D11DomainShader, Deleter<ID3D11DomainShader*>>(dshader);
        compiledShader->Release();
    }

    return domainShaders[ShaderFileName].get();   
}

ID3D11GeometryShader* Shader::getGeometryShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;


    if(geometryShaders.find(ShaderFileName) == geometryShaders.end())
    {
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "GS_MAIN", "gs_5_0");

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
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "VS_MAIN", "vs_5_0");

        if(compiledShader == NULL) return NULL;

        vertexShaderBlob[ShaderFileName] = unique_ptr<ID3DBlob, Deleter<ID3DBlob*>>(compiledShader);
    }

    return vertexShaderBlob[ShaderFileName].get();
}

// iterate all the shader objects in the map
// and compile the shader of the filename
// if compilation is successfull (returned shader != null)
// then erase that key value pair from the map
template<typename T>
void hotReload(unordered_map<wstring, T>& TShader, const char entryPoint[], const char target[])
{
    for(auto shader = TShader.begin(); shader != TShader.end();)
    {
        ID3DBlob* compiledShader = compileBlob(shader->first, entryPoint, target);
        if(compiledShader != NULL)
        {
            shader = TShader.erase(shader);
        }
        else {
            ++shader;
        }
    }
}

void Shader::HotReload()
{
    hotReload(vertexShaders, "VS_MAIN", "vs_5_0");
    hotReload(pixelShaders, "PS_MAIN", "ps_5_0");
    hotReload(hullShaders, "HS_MAIN", "hs_5_0");
    hotReload(domainShaders, "DS_MAIN", "ds_5_0");
    hotReload(geometryShaders, "GS_MAIN", "gs_5_0");
    hotReload(vertexShaderBlob, "VS_MAIN", "vs_5_0");
}
