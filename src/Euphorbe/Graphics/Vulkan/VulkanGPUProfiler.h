#pragma once

#include <Euphorbe/Graphics/GPUProfiler.h>
#include <volk.h>

typedef struct E_VulkanPipelineStatistics E_VulkanPipelineStatistics;
struct E_VulkanPipelineStatistics
{
	VkQueryPool query_pool;
	VkQueryPipelineStatisticFlagBits query_flags;
	E_VectorU64 query_vector;
};

E_VulkanPipelineStatistics* E_Vk_CreatePipelineStatistics();
void E_Vk_FreePipelineStatistics(E_VulkanPipelineStatistics* stats);
void E_Vk_GetQueryResults(E_VulkanPipelineStatistics* stats);
void E_Vk_DrawPipelineStatisticsGUI(E_VulkanPipelineStatistics* stats);

// Get Stats
u64 E_Vk_GetInputAssemblyVertices(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetInputAssemblyPrimitives(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetVertexShaderInvocations(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetClippingInvocations(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetClippingPrimitives(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetFragmentShaderInvocations(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetHullShaderInvocations(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetDomainShaderInvocations(E_VulkanPipelineStatistics* stats);
u64 E_Vk_GetComputeShaderInvocations(E_VulkanPipelineStatistics* stats);