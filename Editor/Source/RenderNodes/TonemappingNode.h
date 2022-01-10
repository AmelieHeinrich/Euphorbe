#pragma once

#include <Euphorbe/Graphics/RenderGraph.h>

enum TonemappingNodeInput
{
	TonemappingNodeInput_Geometry = EUPHORBE_DECLARE_NODE_INPUT(0)
};

enum TonemappingNodeOutput
{
	TonemappingNodeOutput_Color = EUPHORBE_DECLARE_NODE_OUTPUT(0)
};

E_RenderGraphNode* CreateTonemappingNode();
void TonemappingNodeDrawGUI(E_RenderGraphNode* node);