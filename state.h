#pragma once
#include <device.h>
#include <mesh.h>
#include <watch.h>
#include <shader.h>
#include <unordered_map>
using std::unordered_map;

class State
{
public:
    DXDevice* pDevice;
    Mesh* mesh;
    Shader* shaders;
    Watch* stopwatch;
    unordered_map<wstring, float> variables;

    ~State()
    {
        delete pDevice;
        delete mesh;
        delete stopwatch;
        delete shaders;

    }
};