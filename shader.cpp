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

        vertexShaders[ShaderFileName] = vshader;
    }

    return vertexShaders[ShaderFileName];
}

ID3D11PixelShader* Shader::getPixelShader(wstring ShaderFileName){

    if(pixelShaders.find(ShaderFileName) == pixelShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11PixelShader* pshader;
        pDevice->CreatePixelShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &pshader);

        pixelShaders[ShaderFileName] = pshader;
    }

    return pixelShaders[ShaderFileName];
}

ID3D11HullShader* Shader::getHullShader(wstring ShaderFileName){
    
    if(hullShaders.find(ShaderFileName) == hullShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11HullShader* hshader;
        pDevice->CreateHullShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &hshader);

        hullShaders[ShaderFileName] = hshader;
    }

    return hullShaders[ShaderFileName];   
}

ID3D11DomainShader* Shader::getDomainShader(wstring ShaderFileName){
    
    if(domainShaders.find(ShaderFileName) == domainShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11DomainShader* dshader;
        pDevice->CreateDomainShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &dshader);

        domainShaders[ShaderFileName] = dshader;
    }

    return domainShaders[ShaderFileName];   
}

ID3DBlob* Shader::getShaderBlob(wstring ShaderFileName){

    if(shaderBlob.find(ShaderFileName) == shaderBlob.end()){
        ID3DBlob* ShaderByte;
        D3DReadFileToBlob(ShaderFileName.data(), &ShaderByte);

        shaderBlob[ShaderFileName] = ShaderByte;
    }
    return shaderBlob[ShaderFileName];
}

Shader::~Shader(){
    for (auto vshaders : vertexShaders){
        vshaders.second->Release();
    }
    for (auto pshaders : pixelShaders){
        pshaders.second->Release();
    }
    for (auto hshaders : hullShaders){
        hshaders.second->Release();
    }
    for (auto dshaders : domainShaders){
        dshaders.second->Release();
    }
    for (auto shaders : shaderBlob){
        shaders.second->Release();
    }
    
    return;
}