#pragma once
#include <delegate.h>
#include <mesh.h>

BaseWorker* createTransformation(Mesh* mesh, ShaderResources* resources);

BaseWorker* createVertex(Mesh* mesh, ShaderResources* resources, wstring vertexShader);

BaseWorker* createTessellation(Mesh* mesh, ShaderResources* resources, wstring hullShader, wstring domainShader);

BaseWorker* createGeometry(Mesh* mesh, ShaderResources* resources, wstring geometryShader);

BaseWorker* createPixel(Mesh* mesh, ShaderResources* resources, wstring pixelShader);

BaseWorker* rasterMode(Mesh* mesh, D3D11_FILL_MODE fill_mode, D3D11_CULL_MODE cull_mode);

BaseWorker* createVertexInstanced(Mesh* mesh, ShaderResources* resources, wstring vertexShader);

BaseWorker* createUnifiedShader(Mesh* mesh, wstring ushader, string stages, vector<string> constVariables = {}, vector<string> constBuffers = {"transformMatrix"});

BaseWorker* createBlendState(Mesh* mesh);