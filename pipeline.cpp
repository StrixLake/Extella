#include <pipeline.h>

unique_ptr<PipeLine> createPipeline(Pipeline_Desc desciption, Shader* shader, ResourceManager* resources, ID3D11Device* pDevice)
{
    unique_ptr<PipeLine> pipeline = std::make_unique<PipeLine>();

    pipeline->topology = desciption.topology;
    
    pipeline->vertexShader = desciption.vertex_shader;
    pipeline->pVertexShader = shader->getVertexShader(desciption.vertex_shader);
    pipeline->pHullShader = shader->getHullShader(desciption.hull_shader);
    pipeline->pDomainShader = shader->getDomainShader(desciption.domain_shader);
    pipeline->pGeometryShader = shader->getGeometryShader(desciption.geometry_shader);
    pipeline->pPixelShader = shader->getPixelShader(desciption.pixel_shader);

    pDevice->CreateRasterizerState(&desciption.raster_state, &pipeline->pRasterState);
    pDevice->CreateBlendState(&desciption.blend_state, &pipeline->pBlendState);
    pDevice->CreateDepthStencilState(&desciption.depth_stencil_state, &pipeline->pDepthStencilState);

    // get the textures to render to and create
    // an rtv for each of them
    for(int i = 0; i < 8; ++i)
    {
        string render_target = desciption.renderTargets[i];
        if(render_target != "")
        {
            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = desciption.rtv_desc[i];
            ID3D11Texture2D* pRenderTarget = resources->getTexture2D(render_target);
            ID3D11RenderTargetView* out;
            pDevice->CreateRenderTargetView(pRenderTarget, &rtvDesc, &out);
            pipeline->render_target_views[i] = out;
        }
    }

    // get the depth texture and create a depth stencil view
    ID3D11Texture2D* pDepthBuffer = resources->getTexture2D(desciption.depth_stencil_view);
    D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
    depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.Texture2D.MipSlice = 0;
    pDevice->CreateDepthStencilView(pDepthBuffer, &depthDesc, &pipeline->pDepth_stencil_view);

    return pipeline;
}


void PipeLine::setPipelineState(ID3D11DeviceContext* pContext) const
{
    pContext->IASetPrimitiveTopology( topology);

    pContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
    pContext->RSSetState(pRasterState);
    pContext->OMSetDepthStencilState(pDepthStencilState, 0);

    pContext->VSSetShader(pVertexShader, NULL, 0);
    pContext->HSSetShader(pHullShader, NULL, 0);
    pContext->DSSetShader(pDomainShader, NULL, 0);
    pContext->GSSetShader(pGeometryShader, NULL, 0);
    pContext->PSSetShader(pPixelShader, NULL, 0);

    pContext->OMSetRenderTargets(8, render_target_views.data(), pDepth_stencil_view);

    return;
}


PipeLine::~PipeLine()
{
    for(auto rtv : render_target_views)
    {
        if (rtv != NULL) rtv->Release();
    }

    if(pRasterState != NULL) pRasterState->Release();
    if(pBlendState != NULL) pBlendState->Release();
    if(pDepthStencilState != NULL) pDepthStencilState->Release();

    if(pDepth_stencil_view != NULL) pDepth_stencil_view->Release();
}