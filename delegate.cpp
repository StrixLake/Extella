#include <delegate.h>

void Delegate::operator+=(BaseWorker* subscriber)
{
    subscriber->postConstruction();
    invocationList.push_back(subscriber);
}

void Delegate::prePipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
{
    for(BaseWorker* subscriber : invocationList)
    {
        subscriber->prePipelineSetup(camera, variables);
    }
}

void Delegate::postPipelineSetup(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables)
{
    for(BaseWorker* subscriber : invocationList)
    {
        subscriber->postPipelineSetup(camera, variables);
    }
}

Delegate::~Delegate()
{
    for(BaseWorker* subscriber : invocationList)
    {
        delete subscriber;
    }
}