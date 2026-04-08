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

        vertexShaders[ShaderFileName] = {uniq_com_ptr<ID3D11VertexShader>(vshader), uniq_com_ptr<ID3DBlob>(compiledShader)};
    }

    return vertexShaders[ShaderFileName].first.get();
}

ID3D11PixelShader* Shader::getPixelShader(wstring ShaderFileName){

    if(pixelShaders.find(ShaderFileName) == pixelShaders.end())
    {
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "PS_MAIN", "ps_5_0");

        ID3D11PixelShader* pshader;
        pDevice->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &pshader);

        pixelShaders[ShaderFileName] = {uniq_com_ptr<ID3D11PixelShader>(pshader), uniq_com_ptr<ID3DBlob>(compiledShader)};
    }

    return pixelShaders[ShaderFileName].first.get();
}

ID3D11HullShader* Shader::getHullShader(wstring ShaderFileName){
     
    if(ShaderFileName == L"") return NULL;
    
    if(hullShaders.find(ShaderFileName) == hullShaders.end())
    {    
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "HS_MAIN", "hs_5_0");

        ID3D11HullShader* hshader;
        pDevice->CreateHullShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &hshader);

        hullShaders[ShaderFileName] = {uniq_com_ptr<ID3D11HullShader>(hshader), uniq_com_ptr<ID3DBlob>(compiledShader)};
    }

    return hullShaders[ShaderFileName].first.get();   
}

ID3D11DomainShader* Shader::getDomainShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;
    

    if(domainShaders.find(ShaderFileName) == domainShaders.end())
    {    
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "DS_MAIN", "ds_5_0");

        ID3D11DomainShader* dshader;
        pDevice->CreateDomainShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &dshader);

        domainShaders[ShaderFileName] = {uniq_com_ptr<ID3D11DomainShader>(dshader), uniq_com_ptr<ID3DBlob>(compiledShader)};
    }

    return domainShaders[ShaderFileName].first.get();   
}

ID3D11GeometryShader* Shader::getGeometryShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;


    if(geometryShaders.find(ShaderFileName) == geometryShaders.end())
    {
        // compile the loaded source code to a d3dblob
        ID3DBlob* compiledShader = compileBlob(ShaderFileName, "GS_MAIN", "gs_5_0");

        ID3D11GeometryShader* gshader;
        pDevice->CreateGeometryShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &gshader);

        geometryShaders[ShaderFileName] = {uniq_com_ptr<ID3D11GeometryShader>(gshader), uniq_com_ptr<ID3DBlob>(compiledShader)};
    }

    return geometryShaders[ShaderFileName].first.get();   
}

ID3DBlob* Shader::getVertexShaderBlob(wstring vertexShader){

    assert(vertexShaders.find(vertexShader) != vertexShaders.end());

    return vertexShaders[vertexShader].second.get();
}

Shader_Reflection_Desc Shader::getShaderReflection(pair<wstring, string> shader)
{
    if(reflections.find(shader) == reflections.end())
    {
        ID3DBlob* shader_blob = NULL;
        if(shader.second == "vertex") shader_blob = vertexShaders[shader.first].second.get();
        else if(shader.second == "pixel") shader_blob = pixelShaders[shader.first].second.get();
        else if(shader.second == "hull") shader_blob = hullShaders[shader.first].second.get();
        else if(shader.second == "domain") shader_blob = domainShaders[shader.first].second.get();
        else if(shader.second == "geometry") shader_blob = geometryShaders[shader.first].second.get();

        assert(shader_blob != NULL);

        Shader_Reflection_Desc out_desc = reflect(shader_blob);
        out_desc.shader_stage = shader.second;

        reflections[shader] = out_desc;
    }

    return reflections[shader];
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
    
}
