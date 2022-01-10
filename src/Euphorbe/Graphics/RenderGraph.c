#include "RenderGraph.h"

#include <Euphorbe/Core/Log.h>

E_RenderGraph* E_CreateRenderGraph()
{
	E_RenderGraph* graph = malloc(sizeof(E_RenderGraph));

	graph->node_vector.node_count = 0;
	graph->node_vector.nodes = malloc(sizeof(E_RenderGraphNode*) * EUPHORBE_MAX_RENDER_GRAPH_NODES);

	return graph;
}

void E_RenderGraphConnectNodes(E_RenderGraphNode* src_node, u32 src_id, E_RenderGraphNode* dst_node, u32 dst_id)
{
	assert(src_node);
	assert(dst_node);
	assert(!EUPHORBE_IS_NODE_PORT_INPUT(src_id)); // Make sure srcId is an output - to be used as an input
	assert(EUPHORBE_IS_NODE_PORT_INPUT(dst_id)); // Make sure dstId is an input port.

	dst_node->inputs[EUPHORBE_GET_NODE_PORT_INDEX(dst_id)].owner = src_node;
	dst_node->inputs[EUPHORBE_GET_NODE_PORT_INDEX(dst_id)].index = EUPHORBE_GET_NODE_PORT_INDEX(src_id);
	dst_node->input_count++;
}

void RecursivelyAddNodes(E_RenderGraphNode* node, E_RenderGraphNodeVector* vec)
{
	if (node)
	{
		vec->nodes[vec->node_count] = node;
		vec->node_count++;

		for (u32 i = 0; i < node->input_count; ++i)
		{
			E_RenderGraphNode* owner = node->inputs[i].owner;

			if (!owner) break;

			RecursivelyAddNodes(owner, vec);
		}
	}
}

E_Image* E_GetRenderGraphNodeInputImage(E_RenderGraphNodeInput* input)
{
	assert(input->owner);
	return input->owner->outputs[input->index];
}

void E_BuildRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info, E_RenderGraphNode* last_node)
{
	// Add the nodes to a temporary vector

	E_RenderGraphNodeVector* temp = malloc(sizeof(E_RenderGraphNodeVector));
	temp->nodes = malloc(sizeof(E_RenderGraphNode*) * EUPHORBE_MAX_RENDER_GRAPH_NODES);
	temp->node_count = 0;
	RecursivelyAddNodes(last_node, temp);

	// Add the nodes to the graph

	for (i32 candidate_index = temp->node_count - 1; candidate_index >= 0; --candidate_index)
	{
		E_RenderGraphNode* node = temp->nodes[candidate_index];
		b32 already_in_array = 0;

		for (u32 i = 0; i < graph->node_vector.node_count; i++)
		{
			if (graph->node_vector.nodes[i] == node) // If the node is already in the array, don't add it.
			{
				already_in_array = 1;
				break;
			}
		}

		if (!already_in_array)
		{
			graph->node_vector.nodes[graph->node_vector.node_count] = node;
			graph->node_vector.node_count++;
		}
	}

	free(temp->nodes);
	free(temp);

	// Initialize 

	for (u32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i];
		node->init_func(node, info);
	}

	E_LogInfo("RENDER GRAPH BUILD: Built render graph with %d nodes", graph->node_vector.node_count);
}

void E_CleanRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (u32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i];

		node->clean_func(node, info);
		free(node);
	}

	free(graph->node_vector.nodes);
	free(graph);
}

void E_ResizeRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (u32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i] ;

		node->resize_func(node, info);
	}
}

void E_ExecuteRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (u32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i];

		if (node->enabled)
			node->execute_func(node, info);
	}
}
