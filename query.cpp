#include <query.h>

Query::Query(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : pDevice(pDevice), pContext(pContext)
{
    D3D11_QUERY_DESC queryDesc = {D3D11_QUERY_TIMESTAMP,0};

    pDevice->CreateQuery(&queryDesc, &this->timeStampBegin);
    pDevice->CreateQuery(&queryDesc, &this->timeStampEnd);
    
    queryDesc = {D3D11_QUERY_TIMESTAMP_DISJOINT, 0};
    pDevice->CreateQuery(&queryDesc, &this->timeStampDisjoint);
}

void Query::begin()
{
    // only begin the query if a query has not been
    // made already
    if(!isQueryMade)
    {
        pContext->Begin(timeStampDisjoint);
        pContext->End(timeStampBegin);
    }
}

void Query::end(unordered_map<wstring, float>& variables)
{
    // make the timeStampEnd query if a query hasn't been started
    // and mark all the queries as started
    if(!isQueryMade)
    {
        pContext->End(timeStampEnd);
        pContext->End(timeStampDisjoint);
        isQueryMade = true;
    }
    if(isQueryMade)
    {
        // first check query status to not stall the gpu
        // and then get the data
        HRESULT beginData = pContext->GetData(timeStampBegin, NULL, 0, 0);
        HRESULT disjointData = pContext->GetData(timeStampDisjoint, NULL, 0, 0);
        HRESULT endData = pContext->GetData(timeStampEnd, NULL, 0, 0);

        // if all 3 queries return S_OK, only then we get the data for
        // all of them
        if(beginData == S_OK && disjointData == S_OK && endData == S_OK)
        {
            isQueryMade = false;

            UINT64 beginTick;
            pContext->GetData(timeStampBegin, &beginTick, sizeof(UINT64), 0);
            UINT64 endTick;
            pContext->GetData(timeStampEnd, &endTick, sizeof(UINT64), 0);
            D3D11_QUERY_DATA_TIMESTAMP_DISJOINT dis = {};
            pContext->GetData(timeStampDisjoint, &dis, sizeof(dis), 0);

            if(!dis.Disjoint)
            {
                float frameTime = (float)(endTick-beginTick)/dis.Frequency;
                frameTime *= 1000;
                variables[L"Frame Time"] = frameTime;
            }
        }
    }
}

Query::~Query()
{
    timeStampBegin->Release();
    timeStampEnd->Release();
    timeStampDisjoint->Release();
}