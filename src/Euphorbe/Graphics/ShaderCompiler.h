#pragma once

#include <Euphorbe/Core/Common.h>

typedef enum E_ShaderType E_ShaderType;
enum E_ShaderType
{
	E_ShaderTypeVertex = 0x00000001,
	E_ShaderTypeGeometry = 0x00000008,
	E_ShaderTypeFragment = 0x00000010,
	E_ShaderTypeCompute = 0x00000020,
	E_ShaderTypeTaskNV = 0x00000040,
	E_ShaderTypeMeshNV = 0x00000080,
};

typedef struct E_Shader E_Shader;
struct E_Shader
{
	E_ShaderType type;
	u8* code;
	i64 code_size;
};

char* E_ReadFile(const char* path, i32* output_size);
void E_CompileShader(char* source, i32 source_size, E_Shader* shader);