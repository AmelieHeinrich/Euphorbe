#include "VulkanGPUProfiler.h"

#include "VulkanRenderer.h"

#include <cimgui.h>

#define E_STATS_ARRAY(idx) return stats->query_vector.data[idx];
#define E_STATS_GUI(idx) stats->query_vector.data[idx]

E_VulkanPipelineStatistics* E_Vk_CreatePipelineStatistics()
{
	E_VulkanPipelineStatistics* stats = malloc(sizeof(E_VulkanPipelineStatistics));
	E_INIT_VECTOR(stats->query_vector, 10);

	VkQueryPoolCreateInfo query_pool_info = {0};
	query_pool_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	query_pool_info.queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;
	query_pool_info.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT |
										 VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;
	query_pool_info.queryCount = 9;

	stats->query_flags = (E_PipelineStatisticsQuery)query_pool_info.pipelineStatistics;

	VkResult res = vkCreateQueryPool(rhi.device.handle, &query_pool_info, NULL, &stats->query_pool);
	assert(res == VK_SUCCESS);

	return stats;
}

void E_Vk_FreePipelineStatistics(E_VulkanPipelineStatistics* stats)
{
	E_FREE_VECTOR(stats->query_vector);
	vkDestroyQueryPool(rhi.device.handle, stats->query_pool, NULL);
	free(stats);
}

void E_Vk_GetQueryResults(E_VulkanPipelineStatistics* stats)
{
	vkGetQueryPoolResults(
		rhi.device.handle,
		stats->query_pool,
		0,
		1,
		stats->query_vector.size * sizeof(uint64_t),
		stats->query_vector.data,
		sizeof(uint64_t),
		VK_QUERY_RESULT_64_BIT);
}

void E_Vk_DrawPipelineStatisticsGUI(E_VulkanPipelineStatistics* stats)
{
	igText("Input Assembly Vertices: %u", E_STATS_GUI(0));
	igText("Input Assembly Primitives: %u", E_STATS_GUI(1));
	igText("Vertex Shader Invocations: %u", E_STATS_GUI(2));
	igText("Clipping Invocations: %u", E_STATS_GUI(3));
	igText("Clipping Primitives: %u", E_STATS_GUI(4));
	igText("Fragment Shader Invocations: %u", E_STATS_GUI(5));
	igText("Hull Shader Invocations: %u", E_STATS_GUI(6));
	igText("Domain Shader Invocations: %u", E_STATS_GUI(7));
	igText("Compute Shader Invocations: %u", E_STATS_GUI(8));
}

u64 E_Vk_GetInputAssemblyVertices(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(0)
}

u64 E_Vk_GetInputAssemblyPrimitives(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(1)
}

u64 E_Vk_GetVertexShaderInvocations(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(2)
}

u64 E_Vk_GetClippingInvocations(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(3)
}

u64 E_Vk_GetClippingPrimitives(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(4)
}

u64 E_Vk_GetFragmentShaderInvocations(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(5)
}

u64 E_Vk_GetHullShaderInvocations(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(6)
}

u64 E_Vk_GetDomainShaderInvocations(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(7)
}

u64 E_Vk_GetComputeShaderInvocations(E_VulkanPipelineStatistics* stats)
{
	E_STATS_ARRAY(8)
}
