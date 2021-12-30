#include "RenderGraph.h"

#include <Euphorbe/Core/Log.h>

E_RenderGraph* E_CreateRenderGraph()
{
	E_RenderGraph* graph = malloc(sizeof(E_RenderGraph));

	graph->node_vector.node_count = 0;
	memset(graph->node_vector.nodes, 0, sizeof(graph->node_vector.nodes));

	return graph;
}

void E_RenderGraphConnectNodes(E_RenderGraphNode* src_node, u32 src_id, E_RenderGraphNode* dst_node, u32 dst_id)
{
	E_RenderGraphNodeInput input = {0};
	input.owner = src_node;
	input.index = EUPHORBE_GET_NODE_PORT_INDEX(src_id);

	dst_node->inputs[EUPHORBE_GET_NODE_PORT_INDEX(dst_id)] = input;
	dst_node->input_count++;
}

void RecursivelyAddNodes(E_RenderGraphNode* node, E_RenderGraphNodeVector* vec)
{
	vec->nodes[vec->node_count] = node;
	vec->node_count++;

	for (i32 i = 0; i < node->input_count; i++)
	{
		if (&node->inputs[i] != NULL)
		{
			E_RenderGraphNode* owner = node->inputs[i].owner;

			if (owner == NULL) break;

			RecursivelyAddNodes(owner, vec);
		}
	}
}

E_Image* E_GetRenderGraphNodeInputImage(E_RenderGraphNodeInput* input)
{
	return input->owner->outputs[input->index];
}

void E_BuildRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info, E_RenderGraphNode* lastNode)
{
	// Add them to a temporary vector

	E_RenderGraphNodeVector temp = {0};
	temp.node_count = 0;
	memset(temp.nodes, 0, sizeof(E_RenderGraphNode*) * EUPHORBE_MAX_RENDER_GRAPH_NODES);
	RecursivelyAddNodes(lastNode, &temp);

	// Add the nodes to the graph

	for (i32 candidate_index = temp.node_count - 1; candidate_index >= 0; --candidate_index)
	{
		E_RenderGraphNode* node = temp.nodes[candidate_index];
		b32 already_in_array = 0;

		for (i32 i = 0; i < graph->node_vector.node_count; i++)
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

	// Initialize 

	for (i32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i];
		node->init_func(node, info);

		E_LogInfo("Initialised node at index %d with name %s", i, node->name);
	}
}

void E_CleanRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (i32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i];

		node->clean_func(node, info);
		free(node);
	}

	free(graph);
}

void E_ResizeRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (i32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i] ;

		node->resize_func(node, info);
	}
}

void E_ExecuteRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (i32 i = 0; i < graph->node_vector.node_count; i++)
	{
		E_RenderGraphNode* node = graph->node_vector.nodes[i];

		if (node->enabled)
			node->execute_func(node, info);
	}
}
