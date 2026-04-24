#pragma once
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <boost/unordered_map.hpp>
#include <resources.h>
#include <renderpass.h>
#include <query.h>
using boost::unordered_map;

class State
{
public:
    DXDevice* pDevice;
    Shader* shaders;
    ResourceManager* resources;
    Query* query;
    unordered_map<wstring, float> variables;
    unordered_map<string, GPUMesh> meshes;
    unordered_map<string, Material> materials;
    unordered_map<string, unique_ptr<PipeLine>> pipelines;
    vector<RenderPass> renderpasses;

    ~State()
    {
        delete pDevice;
        delete shaders;
        delete resources;
        delete query;
    }
};