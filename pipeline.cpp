#include <pipeline.h>

unique_ptr<PipeLine> createPipeline(Pipeline_Desc desciption, Shader* shader, ShaderResources* resources, ID3D11Device* pDevice)
{
    unique_ptr<PipeLine> pipeline = std::make_unique<PipeLine>();

    pipeline->topology = desciption.topology;
    
    pipeline->pVertexShader = shader->getVertexShader(desciption.vertex_shader);
    pipeline->pHullShader = shader->getHullShader(desciption.hull_shader);
    pipeline->pDomainShader = shader->getDomainShader(desciption.domain_shader);
    pipeline->pGeometryShader = shader->getGeometryShader(desciption.geometry_shader);
    pipeline->pPixelShader = shader->getPixelShader(desciption.pixel_shader);

    pDevice->CreateRasterizerState(&desciption.raster_state, &pipeline->pRasterState);
    pDevice->CreateBlendState(&desciption.blend_state, &pipeline->pBlendState);
    pDevice->CreateDepthStencilState(&desciption.depth_stencil_state, &pipeline->pDepthStencilState);

    // work in progress
    for(int i = 0; i < 8; ++i)
    {
        string render_target = desciption.renderTargets[i];
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = desciption.rtv_desc[i];
        
    }

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