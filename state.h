#pragma once
#include <device.h>
#include <mesh.h>
#include <watch.h>
#include <shader.h>
#include <unordered_map>
#include <resources.h>
#include <query.h>
using std::unordered_map;

class State
{
public:
    DXDevice* pDevice;
    Mesh* mesh;
    Shader* shaders;
    Watch* stopwatch;
    ShaderResources* resources;
    Query* query;
    unordered_map<wstring, float> variables;

    ~State()
    {
        delete pDevice;
        delete mesh;
        delete stopwatch;
        delete shaders;
        delete resources;
        delete query;
    }
};