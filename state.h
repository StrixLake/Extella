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
    vector<RenderPass> renderpasses;
    Shader* shaders;
    ResourceManager* resources;
    Query* query;
    unordered_map<wstring, float> variables;

    ~State()
    {
        delete pDevice;
        delete shaders;
        delete resources;
        delete query;
    }
};