#include "GPUProfiler.h"

#ifdef EUPHORBE_WINDOWS
	#include <Euphorbe/Graphics/Vulkan/VulkanGPUProfiler.h>
#endif

#define SPAGHETTI_CODE_KEKW(function) E_Vk_##function(stats->rhi_handle)

E_PipelineStatistics* E_CreatePipelineStatistics()
{
	E_PipelineStatistics* stats = malloc(sizeof(E_PipelineStatistics));

#ifdef EUPHORBE_WINDOWS
	stats->rhi_handle = E_Vk_CreatePipelineStatistics();
#endif

	return stats;
}

void E_FreePipelineStatistics(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_FreePipelineStatistics(stats->rhi_handle);
#endif

	free(stats);
}

void E_GetQueryResults(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_GetQueryResults(stats->rhi_handle);
#endif
}

void E_DrawPipelineStatisticsGUI(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_DrawPipelineStatisticsGUI(stats->rhi_handle);
#endif
}

u64 E_GetInputAssemblyVertices(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetInputAssemblyVertices);
#endif
}

u64 E_GetInputAssemblyPrimitives(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetInputAssemblyPrimitives);
#endif	
}

u64 E_GetVertexShaderInvocations(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetVertexShaderInvocations);
#endif
}

u64 E_GetClippingInvocations(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetClippingInvocations);
#endif
}

u64 E_GetClippingPrimitives(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetClippingPrimitives);
#endif
}

u64 E_GetFragmentShaderInvocations(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetFragmentShaderInvocations);
#endif
}

u64 E_GetHullShaderInvocations(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetHullShaderInvocations);
#endif
}

u64 E_GetDomainShaderInvocations(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetDomainShaderInvocations);
#endif
}

u64 E_GetComputeShaderInvocations(E_PipelineStatistics* stats)
{
#ifdef EUPHORBE_WINDOWS
	SPAGHETTI_CODE_KEKW(GetComputeShaderInvocations);
#endif	
}
