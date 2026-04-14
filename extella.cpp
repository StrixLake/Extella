#define EXPORT extern "C" __declspec(dllexport)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Windows.h>
#include <device.h>
#include <mesh.h>
#include <shader.h>
#include <state.h>
#include <resources.h>
#include <query.h>
#include <extella.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

State* state;

void initRender();

EXPORT void InitializeRenderer(IDXGISurface2** pSurface, IDXGIDevice2** pDevice){

    DXDevice *device = new DXDevice();
    device->SetTargets();
    
    Query* query = new Query(device->pDevice, device->pContext);
    
    Shader *shaders = new Shader(device->pDevice);
    
    ResourceManager* manager = new ResourceManager(device->pDevice, device->pContext);
    
    // create the depth buffer
    D3D11_TEXTURE2D_DESC render_desc = {};
    render_desc.Width = WIDTH;
    render_desc.Height = HEIGHT;
    render_desc.MipLevels = 1;
    render_desc.ArraySize = 1;
    render_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    render_desc.Usage = D3D11_USAGE_DEFAULT;
    render_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    render_desc.SampleDesc = {1,0};
    manager->createTexture2D(render_desc, "depth buffer");
    
    // create the frame render target
    render_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    render_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    render_desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    manager->createTexture2D(render_desc, "render frame");
    
    
    state = new State();
    state->pDevice = device;
    state->resources = manager;
    state->shaders = shaders;
    state->query = query;
    
    ID3D11Texture2D* pRender = manager->getTexture2D("render frame");

    pRender->QueryInterface(__uuidof(IDXGISurface2), (void**)pSurface);
    device->pDevice->QueryInterface(__uuidof(IDXGIDevice2), (void**)pDevice);

    state->variables[L"Distance"] = 100;
    state->variables[L"aspect ratio"] = (float)WIDTH/HEIGHT;
    state->variables[L"rotationY"] = 0;
    state->variables[L"rotationX"] = 0;

    initRender();

    return;
}


void renderer(State* state);

EXPORT void render(){
    renderer(state);
}

EXPORT void release() {
    delete state;
}

EXPORT SAFEARRAY* getGlobalVariables(){
    int elements = 6;
    
    const wchar_t* x[] = {L"Distance", L"Position X",
                          L"Position Y", L"Position Z",
                          L"in-tes", L"Instances"};

    SAFEARRAY* ar = SafeArrayCreateVector(VT_BSTR, 0, elements);
    
    for(long i = 0; i < elements; ++i)
    {
        BSTR data = SysAllocString(x[i]);
        SafeArrayPutElement(ar, &i, data);
        SysFreeString(data);
    }

    return ar;
}

EXPORT void setVariable(BSTR variable, float value){
    // since BSTR is wchar_t*
    // we can use it directly as the key
    if(state != NULL) state->variables[variable] = value;
}

EXPORT float getVariable(BSTR variable)
{
    return state->variables[variable];
}

EXPORT void hotReload()
{
    state->shaders->HotReload();
}


void initRender()
{
    // create a pipeline
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    Pipeline_Desc default_pipeline = {};
    default_pipeline.depth_stencil_view = "depth buffer";
    default_pipeline.vertex_shader = L"spot";
    default_pipeline.pixel_shader = L"spot";
    default_pipeline.renderTargets[0] = "render frame";
    default_pipeline.rtv_desc[0] = rtvDesc;

    unique_ptr<PipeLine> pipeline = createPipeline(default_pipeline, state->shaders, state->resources, state->pDevice->pDevice);

    state->pipelines.push_back(std::move(pipeline));

    // create the gpumesh and material
    pair<vector<Mesh>, vector<Material>> mesh_material = load_obj("mesh/296.obj");

    for(Mesh& mesh : mesh_material.first)
    {
        state->meshes.push_back(convert_mesh(mesh, state->pDevice->pDevice));
    }
    for(Material& material : mesh_material.second)
    {
        state->resources->createTexturesFromMaterial(material);
        state->materials[material.material_name] = std::move(material);
    }

    state->resources->createTexture2DfromImage("mesh/default_.png");

    // create a render pass for all the gpu meshes in the vector
    for(GPUMesh& gpumesh : state->meshes)
    {
        state->renderpasses.push_back(RenderPass(&gpumesh, state->pipelines[0].get(), &state->materials.at(gpumesh.prefered_material),
                                        state->resources, state->shaders, state->variables, state->pDevice->pDevice));
    }

    // hopefully that should be all
}