#include <device.h>
#include <mesh.h>
#include <state.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#define MOVE 0.01


void renderer(State* state){


    state->resources->clearRTV(state->pDevice->pContext);

    state->query->begin();

    for(RenderPass& renderpass : state->renderpasses)
    {
        renderpass.execute(state->pDevice->pContext);
    }

    state->query->end(state->variables);
    
    return;
}