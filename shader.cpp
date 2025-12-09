#include <shader.h>
#include <d3dcompiler.h>

Shader::Shader(DXDevice* device){
    this->device = device;
    return;
}

ID3D11VertexShader* Shader::getVertexShader(wstring ShaderFileName){

    // if the shader isn't loaded, then load and create the shader
    if(vertexShaders.find(ShaderFileName) == vertexShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11VertexShader* vshader;
        device->pDevice->CreateVertexShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &vshader);

        vertexShaders[ShaderFileName] = vshader;
    }

    return vertexShaders[ShaderFileName];
}

ID3D11PixelShader* Shader::getPixelShader(wstring ShaderFileName){

    if(pixelShaders.find(ShaderFileName) == pixelShaders.end()){
        ID3DBlob *ShaderByte = getShaderBlob(ShaderFileName);

        ID3D11PixelShader* pshader;
        device->pDevice->CreatePixelShader(ShaderByte->GetBufferPointer(), ShaderByte->GetBufferSize(), NULL, &pshader);

        pixelShaders[ShaderFileName] = pshader;
    }

    return pixelShaders[ShaderFileName];
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
    for (auto shaders : shaderBlob){
        shaders.second->Release();
    }
    
    return;
}