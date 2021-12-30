#ifndef EDITOR_HDR_NODE_H
#define EDITOR_HDR_NODE_H

#include <Euphorbe/Graphics/RenderGraph.h>

enum HDRNodeInput
{
	HDRNodeInput_Geometry = EUPHORBE_DECLARE_NODE_INPUT(0)
};

enum HDRNodeOutput
{
	HDRNodeOutput_Color = EUPHORBE_DECLARE_NODE_OUTPUT(0)
};

E_RenderGraphNode* CreateHDRNode();
void HDRNodeDrawGUI(E_RenderGraphNode* node);

#endif