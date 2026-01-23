#include <shader.h>
#include <d3dcompiler.h>

Shader::Shader(ID3D11Device* device){
    this->pDevice = device;
    return;
}

ID3D11VertexShader* Shader::getVertexShader(wstring ShaderFileName){

    // if the shader isn't loaded, then load and create the shader
    if(vertexShaders.find(ShaderFileName) == vertexShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11VertexShader* vshader;
        pDevice->CreateVertexShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &vshader);

        vertexShaders[ShaderFileName] = unique_ptr<ID3D11VertexShader, Deleter<ID3D11VertexShader*>>(vshader);
    }

    return vertexShaders[ShaderFileName].get();
}

ID3D11PixelShader* Shader::getPixelShader(wstring ShaderFileName){

    if(pixelShaders.find(ShaderFileName) == pixelShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11PixelShader* pshader;
        pDevice->CreatePixelShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &pshader);

        pixelShaders[ShaderFileName] = unique_ptr<ID3D11PixelShader, Deleter<ID3D11PixelShader*>>(pshader);
    }

    return pixelShaders[ShaderFileName].get();
}

ID3D11HullShader* Shader::getHullShader(wstring ShaderFileName){
     
    if(ShaderFileName == L"") return NULL;
    
    if(hullShaders.find(ShaderFileName) == hullShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11HullShader* hshader;
        pDevice->CreateHullShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &hshader);

        hullShaders[ShaderFileName] = unique_ptr<ID3D11HullShader, Deleter<ID3D11HullShader*>>(hshader);
    }

    return hullShaders[ShaderFileName].get();   
}

ID3D11DomainShader* Shader::getDomainShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;
    

    if(domainShaders.find(ShaderFileName) == domainShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11DomainShader* dshader;
        pDevice->CreateDomainShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &dshader);

        domainShaders[ShaderFileName] = unique_ptr<ID3D11DomainShader, Deleter<ID3D11DomainShader*>>(dshader);
    }

    return domainShaders[ShaderFileName].get();   
}

ID3D11GeometryShader* Shader::getGeometryShader(wstring ShaderFileName){
    
    if(ShaderFileName == L"") return NULL;


    if(geometryShaders.find(ShaderFileName) == geometryShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11GeometryShader* gshader;
        pDevice->CreateGeometryShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &gshader);

        geometryShaders[ShaderFileName] = unique_ptr<ID3D11GeometryShader, Deleter<ID3D11GeometryShader*>>(gshader);
    }

    return geometryShaders[ShaderFileName].get();   
}

ID3DBlob* Shader::getShaderBlob(wstring ShaderFileName){

    if(shaderBlob.find(ShaderFileName) == shaderBlob.end()){
        ID3DBlob* ShaderByte;
        wstring ShaderFileNameFull = L"shaders/" + ShaderFileName;
        D3DReadFileToBlob(ShaderFileNameFull.data(), &ShaderByte);

        shaderBlob[ShaderFileName] = unique_ptr<ID3DBlob, Deleter<ID3DBlob*>>(ShaderByte);
    }
    return shaderBlob[ShaderFileName].get();
}

void Shader::HotReload()
{
    vertexShaders.clear();
    pixelShaders.clear();
    hullShaders.clear();
    domainShaders.clear();
    geometryShaders.clear();
    shaderBlob.clear();
}
