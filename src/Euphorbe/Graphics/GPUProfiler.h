#pragma once

#include <Euphorbe/Core/Common.h>
#include <Euphorbe/Core/Vector.h>

E_DEFINE_VECTOR(U64, u64);

typedef enum E_PipelineStatisticsQuery E_PipelineStatisticsQuery;
enum E_PipelineStatisticsQuery
{
	E_PipelineStatisticsQueryInputAssemblyVertices = 0x00000001,
	E_PipelineStatisticsQueryInputAssemblyPrimitives = 0x00000002,
	E_PipelineStatisticsQueryVertexShaderInvocations = 0x00000003,
	E_PipelineStatisticsQueryClippingInvocations = 0x00000020,
	E_PipelineStatisticsQueryClippingPrimitives = 0x00000040,
	E_PipelineStatisticsQueryFragmentShaderInvocations = 0x00000080,
	E_PipelineStatisticsQueryHullShaderInvocations = 0x00000100,
	E_PipelineStatisticsQueryDomainShaderInvocations = 0x00000200,
	E_PipelineStatisticsQueryComputeShaderInvocations = 0x00000400,
};

typedef struct E_PipelineStatistics E_PipelineStatistics;
struct E_PipelineStatistics
{
	E_PipelineStatisticsQuery query;
	void* rhi_handle;
};

E_PipelineStatistics* E_CreatePipelineStatistics();
void E_FreePipelineStatistics(E_PipelineStatistics* stats);
void E_GetQueryResults(E_PipelineStatistics* stats);
void E_DrawPipelineStatisticsGUI(E_PipelineStatistics* stats);

// Get Stats
u64 E_GetInputAssemblyVertices(E_PipelineStatistics* stats);
u64 E_GetInputAssemblyPrimitives(E_PipelineStatistics* stats);
u64 E_GetVertexShaderInvocations(E_PipelineStatistics* stats);
u64 E_GetClippingInvocations(E_PipelineStatistics* stats);
u64 E_GetClippingPrimitives(E_PipelineStatistics* stats);
u64 E_GetFragmentShaderInvocations(E_PipelineStatistics* stats);
u64 E_GetHullShaderInvocations(E_PipelineStatistics* stats);
u64 E_GetDomainShaderInvocations(E_PipelineStatistics* stats);
u64 E_GetComputeShaderInvocations(E_PipelineStatistics* stats);