#include "RenderGraph.h"

E_RenderGraph* E_CreateRenderGraph()
{
	E_RenderGraph* graph = malloc(sizeof(E_RenderGraph));

	graph->node_count = 0;
	memset(graph->nodes, 0, sizeof(graph->nodes));

	return graph;
}

void E_CleanRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (i32 i = 0; i < graph->node_count; i++)
	{
		E_RenderGraphNode* node = graph->nodes[i];

		node->clean_func(node, info);
		free(node);
	}

	free(graph);
}

void E_ResizeRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (i32 i = 0; i < graph->node_count; i++)
	{
		E_RenderGraphNode* node = graph->nodes[i];

		node->resize_func(node, info);
	}
}

void E_ExecuteRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info)
{
	for (i32 i = 0; i < graph->node_count; i++)
	{
		E_RenderGraphNode* node = graph->nodes[i];

		if (node->enabled)
			node->execute_func(node, info);
	}
}

void E_AddNodeToRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info, E_RenderGraphNode* node)
{
	graph->nodes[graph->node_count] = node;
	graph->nodes[graph->node_count]->init_func(graph->nodes[graph->node_count], info);
	graph->node_count++;
}

E_RenderGraphNode* E_GetRenderGraphNode(E_RenderGraph* graph, char* name)
{
	for (i32 i = 0; i < graph->node_count; i++)
	{
		if (!strcmp(name, graph->nodes[i]->name))
			return graph->nodes[i];
	}
}
