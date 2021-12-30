#ifndef FINAL_BLIT_NODE_H
#define FINAL_BLIT_NODE_H

#include <Euphorbe/Graphics/RenderGraph.h>

enum FinalBlitNodeInput
{
	FinalBlitNodeInput_ImageIn = EUPHORBE_DECLARE_NODE_INPUT(0)
};

E_RenderGraphNode* CreateFinalBlitNode();

#endif