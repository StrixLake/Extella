#include <DirectXMath.h>
#include <array>
#include <string>
#include <unordered_map>
using std::unordered_map;
using std::wstring;
using std::array;
using DirectX::XMMATRIX;

class BaseWorker{
public:
    virtual void extensionWork(array<XMMATRIX, 3> &camera, unordered_map<wstring, float> &variables) = 0;
    virtual ~BaseWorker() = default;
};