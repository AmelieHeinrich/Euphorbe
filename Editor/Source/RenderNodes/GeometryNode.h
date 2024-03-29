#pragma once

#include <Euphorbe/Graphics/RenderGraph.h>
#include <Euphorbe/Resource/Resource.h>
#include <Euphorbe/Graphics/CommandBuffer.h>

enum GeometryNodeOutput
{
	GeometryNodeOutput_Color = EUPHORBE_DECLARE_NODE_OUTPUT(0),
	GeometryNodeOutput_Depth = EUPHORBE_DECLARE_NODE_OUTPUT(1)
};

E_RenderGraphNode* CreateGeometryNode();
E_ResourceFile* GetGeometryNodeMaterial(E_RenderGraphNode* node);
void GeometryNodeDrawGUI(E_RenderGraphNode* node);