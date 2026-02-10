#pragma once
#include <DirectXMath.h>
#include <array>
#include <string>
#include <boost/unordered_map.hpp>
using boost::unordered_map;
using std::wstring;
using std::array;
using DirectX::XMMATRIX;
using std::vector;

class BaseWorker{
    
public:
    // this method is invoked right after the extension
    // is added to the delegate
    virtual void postConstruction() = 0;

    // this method is invoked right after the draw method and
    // before any work is done by the mesh instance
    virtual void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables) = 0;

    // this method is invoked right before pContext->Draw
    // to finish any remaining work
    virtual void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables) = 0;

    virtual ~BaseWorker() = default;
};


class Delegate{

    vector<BaseWorker*> invocationList;
public:
    void operator+=(BaseWorker* subscriber);

    void prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables);

    void postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables);

    ~Delegate();
};
