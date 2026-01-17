#pragma once
#include <delegate.h>
#include <mesh.h>

BaseWorker* createTransformation(Mesh* mesh, ShaderResources* resources);

BaseWorker* createVertex(Mesh* mesh, ShaderResources* resources, wstring vertexShader);

BaseWorker* createTessellation(Mesh* mesh, ShaderResources* resources, wstring hullShader, wstring domainShader);

BaseWorker* createGeometry(Mesh* mesh, ShaderResources* resources, wstring geometryShader);

BaseWorker* createPixel(Mesh* mesh, ShaderResources* resources, wstring pixelShader);