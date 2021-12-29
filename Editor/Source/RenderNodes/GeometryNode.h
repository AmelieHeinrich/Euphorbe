#ifndef EDITOR_GEOMETRY_NODE_H
#define EDITOR_GEOMETRY_NODE_H

#include <Euphorbe/Graphics/RenderGraph.h>
#include <Euphorbe/Resource/Resource.h>

E_RenderGraphNode* CreateGeometryNode();
E_Material* GetGeometryNodeMaterial(E_RenderGraphNode* node);

#endif