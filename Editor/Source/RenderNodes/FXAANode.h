#pragma once

#include <Euphorbe/Graphics/RenderGraph.h>

enum FXAANodeInput
{
    FXAANodeInput_Color = EUPHORBE_DECLARE_NODE_INPUT(0),
};

enum FXAANodeOutput
{
    FXAANodeOutput_ImageOut = EUPHORBE_DECLARE_NODE_OUTPUT(0),
};

E_RenderGraphNode* CreateFXAANode();
void FXAANodeDrawGUI(E_RenderGraphNode* node);