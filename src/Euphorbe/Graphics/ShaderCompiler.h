#ifndef EUPHORBE_SHADER_COMPILER_H
#define EUPHORBE_SHADER_COMPILER_H

#include <Euphorbe/Core/Common.h>

typedef enum E_ShaderType E_ShaderType;
enum E_ShaderType
{
	E_ShaderTypeVertex,
	E_ShaderTypeFragment,
	E_ShaderTypeGeometry,
	E_ShaderTypeCompute
};

char* E_ReadFile(const char* path, i32* output_size);
const u8* E_CompileSPIRV(char* source, i32 source_size, E_ShaderType shader_type, i64* output_spirv_size);

#endif