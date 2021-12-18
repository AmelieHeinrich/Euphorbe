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

typedef struct E_Shader E_Shader;
struct E_Shader
{
	E_ShaderType type;
	u8* code;
	i64 code_size;
};

char* E_ReadFile(const char* path, i32* output_size);
void E_CompileShader(char* source, i32 source_size, E_Shader* shader);

#endif