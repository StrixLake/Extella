#include <renderpass.h>
#include <DirectXMath.h>

cbufLambda RenderPass::getCBufferStruct(string bufferName, const unordered_map<wstring, float>& global_variables)
{   

    // we define a lambda function for each constant buffer
    // that a shader may have
    unordered_map<string, cbufLambda> cbufLambdas;

    cbufLambdas["CameraPosition"] = [&distance = global_variables.at(L"Distance"),
                                      &rotationX = global_variables.at(L"rotationX"),
                                      &rotationY = global_variables.at(L"rotationY")]
                                      (ID3D11Buffer* cBuffer, ID3D11DeviceContext* pContext)
    {
        struct CameraBuffer
        {
            DirectX::XMVECTOR cameraPosition;
            char pad[112];
        };
        // because the world is rotated, the camera is rotated in the inverse direction to match
        // the position it should be in the non rotated world
        // as if the camera was rotated instead of the world

        CameraBuffer camera  = {DirectX::XMVectorSet(0., 0, -distance/10, 0.), {}};
        
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(rotationX/100) * DirectX::XMMatrixRotationX(rotationY/100);
        camera.cameraPosition = DirectX::XMVector4Transform(camera.cameraPosition, DirectX::XMMatrixInverse(NULL, rotation));
        
        pContext->UpdateSubresource(cBuffer, 0, NULL, &camera, 0, 0);

    };

    cbufLambdas["Transformation"] = [&worldTransform = this->mesh->worldMatrix,
                                     &rotationX = global_variables.at(L"rotationX"),
                                     &rotationY = global_variables.at(L"rotationY"),
                                     &distance = global_variables.at(L"Distance")]
                                    (ID3D11Buffer* cBuffer, ID3D11DeviceContext* pContext)
    {
        struct Transformation
        {
            DirectX::XMMATRIX transform;
            DirectX::XMMATRIX proj;
        };

        Transformation transformation = {worldTransform, {}};
        DirectX::XMMATRIX ViewProj = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0., 0, -distance/10, 0.),
                                                                    DirectX::XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ),
                                                                    DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f )) // camera matrix
                                                                    * DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 
                                                                                                        (float)WIDTH/HEIGHT, 
                                                                                                        0.01, 1000.);

        transformation.transform *= DirectX::XMMatrixRotationY(rotationX/100)
                                    * DirectX::XMMatrixRotationX(rotationY/100)
                                    * ViewProj;

        transformation.transform = DirectX::XMMatrixTranspose(transformation.transform);
        transformation.proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(rotationX/100)
                                                         * DirectX::XMMatrixRotationX(rotationY/100)
                                                         *DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4,
                                                                                             (float)WIDTH/HEIGHT,
                                                                                             0.01, 1000.));

        pContext->UpdateSubresource(cBuffer, 0, NULL, &transformation, 0, 0);
    };

    cbufLambdas["Material"] = [material = this->material->constMaterial]
                                (ID3D11Buffer* cBuffer, ID3D11DeviceContext* pContext)
    {
        pContext->UpdateSubresource(cBuffer, 0, NULL, &material, 0, 0);
    };

    return cbufLambdas.at(bufferName);
}