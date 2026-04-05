#pragma once
#include <d3d11.h>
#include <device.h>
#include <shader.h>
#include <resources.h>

constexpr D3D11_RASTERIZER_DESC Default_Raster_State = {D3D11_FILL_SOLID, D3D11_CULL_BACK,
                                                        false, 0, 0.f,
                                                        0.f, true, false,
                                                        false, false};

constexpr D3D11_BLEND_DESC Default_Blend_State = {false, false,
                                                {{false,D3D11_BLEND_ONE,
                                                D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD,
                                                D3D11_BLEND_ONE, D3D11_BLEND_ZERO,
                                                D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL}}};

constexpr D3D11_RASTERIZER_DESC Wireframe_Raster_State = {D3D11_FILL_WIREFRAME, D3D11_CULL_NONE,
                                                        false, 0, 0.f,
                                                        0.f, true, false,
                                                        false, true};

constexpr D3D11_RASTERIZER_DESC Wireframe_Raster_State1 = {D3D11_FILL_WIREFRAME, D3D11_CULL_BACK,
                                                        false, 0, 0.f,
                                                        0.f, true, false,
                                                        false, true};

constexpr D3D11_RASTERIZER_DESC NoCull_Raster_State = {D3D11_FILL_SOLID, D3D11_CULL_NONE,
                                                        false, 0, 0.f,
                                                        0.f, true, false,
                                                        false, false};

constexpr D3D11_BLEND_DESC Transparent_Blend_State = {false, false,
                                                    {{true,D3D11_BLEND_SRC_ALPHA,
                                                    D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
                                                    D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA,
                                                    D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL}}};

constexpr D3D11_BLEND_DESC Additive_Blend_State = {false, false,
                                                    {{true,D3D11_BLEND_ONE,
                                                    D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
                                                    D3D11_BLEND_ONE, D3D11_BLEND_ONE,
                                                    D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL}}};

constexpr D3D11_DEPTH_STENCIL_DESC Default_Depth_Stencil = {true, D3D11_DEPTH_WRITE_MASK_ALL,
                                                            D3D11_COMPARISON_LESS, false,
                                                            D3D11_DEFAULT_STENCIL_READ_MASK, D3D11_DEFAULT_STENCIL_WRITE_MASK,
                                                            {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                                                            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS},
                                                            {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                                                            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS}};

constexpr D3D11_DEPTH_STENCIL_DESC NoWrite_Depth_Stencil = {true, D3D11_DEPTH_WRITE_MASK_ZERO,
                                                            D3D11_COMPARISON_LESS, false,
                                                            D3D11_DEFAULT_STENCIL_READ_MASK, D3D11_DEFAULT_STENCIL_WRITE_MASK,
                                                            {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                                                            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS},
                                                            {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                                                            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS}};

constexpr D3D11_DEPTH_STENCIL_DESC NoTest_Depth_Stencil = {false, D3D11_DEPTH_WRITE_MASK_ZERO,
                                                            D3D11_COMPARISON_LESS, false,
                                                            D3D11_DEFAULT_STENCIL_READ_MASK, D3D11_DEFAULT_STENCIL_WRITE_MASK,
                                                            {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                                                            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS},
                                                            {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                                                            D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS}};


struct Pipeline_Desc
{
    D3D11_RASTERIZER_DESC raster_state = Default_Raster_State;
    D3D11_BLEND_DESC blend_state = Default_Blend_State;
    D3D11_DEPTH_STENCIL_DESC depth_stencil_state = Default_Depth_Stencil;

    string depth_stencil_view;
    array<string, 8> renderTargets;
    array<D3D11_RENDER_TARGET_VIEW_DESC,8> rtv_desc;

    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    wstring vertex_shader;
    wstring hull_shader;
    wstring domain_shader;
    wstring geometry_shader;
    wstring pixel_shader;
};


class PipeLine
{
    ID3D11VertexShader* pVertexShader = NULL;
    ID3D11HullShader* pHullShader = NULL;
    ID3D11DomainShader* pDomainShader = NULL;
    ID3D11GeometryShader* pGeometryShader = NULL;
    ID3D11PixelShader* pPixelShader = NULL;

    array<ID3D11RenderTargetView*, 8> render_target_views;
    ID3D11DepthStencilView* pDepth_stencil_view = NULL;

    D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    ID3D11RasterizerState* pRasterState = NULL;
    ID3D11BlendState* pBlendState = NULL;
    ID3D11DepthStencilState* pDepthStencilState = NULL;

public:
    friend unique_ptr<PipeLine> createPipeline(Pipeline_Desc descriptuon, Shader* shader, ResourceManager* resources, ID3D11Device* pDevice);

    void setPipelineState(ID3D11DeviceContext* pContext) const;

    ~PipeLine();

};

unique_ptr<PipeLine> createPipeline(Pipeline_Desc description, Shader* shader, ResourceManager* resources, ID3D11Device* pDevice);