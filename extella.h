#pragma once
#define EXPORT extern "C" __declspec(dllexport)
#include <dxgi1_2.h>

// contains the api for the renderer
// this api is shared by both Ataraxia and
// win32 front end of Extella

EXPORT void InitializeRenderer(IDXGISurface2** pSurface, IDXGIDevice2** pDevice);

EXPORT void render();

EXPORT void release();

EXPORT SAFEARRAY* getGlobalVariables();

EXPORT void setVariable(BSTR variable, float value);

EXPORT float getVariable(BSTR variable);

EXPORT void hotReload();