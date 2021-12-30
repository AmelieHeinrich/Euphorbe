#ifndef EDITOR_GEOMETRY_NODE_H
#define EDITOR_GEOMETRY_NODE_H

#include <Euphorbe/Graphics/RenderGraph.h>
#include <Euphorbe/Resource/Resource.h>

enum GeometryNodeOutput
{
	GeometryNodeOutput_Color = EUPHORBE_DECLARE_NODE_OUTPUT(0),
	GeometryNodeOutput_Depth = EUPHORBE_DECLARE_NODE_OUTPUT(1)
};

E_RenderGraphNode* CreateGeometryNode();
E_Material* GetGeometryNodeMaterial(E_RenderGraphNode* node);
void EnableGeometryNodeSkybox(E_RenderGraphNode* node, b32 enable);

#endif