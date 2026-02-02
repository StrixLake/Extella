#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <string>
using std::wstring;
using std::unordered_map;

class Query
{
    bool isQueryMade = false;
public:
    Query(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

    void begin();
    void end(unordered_map<wstring, float>& variables);

    // right now we will only hold a single pair
    // of timestamp and disjoint query interface
    ID3D11Query* timeStampBegin = NULL;
    ID3D11Query* timeStampEnd = NULL;
    ID3D11Query* timeStampDisjoint = NULL;

    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;

    ~Query();
};