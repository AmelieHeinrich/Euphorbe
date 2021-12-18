#include "ShaderCompiler.h"

#include <stdio.h>
#include <shaderc/shaderc.h>

#include <Euphorbe/Core/Log.h>

shaderc_shader_kind E_ShaderKindToShaderC(E_ShaderType type)
{
	switch (type)
	{
	case E_ShaderTypeVertex:
		return shaderc_vertex_shader;
	case E_ShaderTypeFragment:
		return shaderc_fragment_shader;
	case E_ShaderTypeGeometry:
		return shaderc_geometry_shader;
	case E_ShaderTypeCompute:
		return shaderc_compute_shader;
	}

	return 0;
}

char* E_ReadFile(const char* path, i32* output_size)
{
	FILE* file = fopen(path, "rb");

	if (!file)
	{
		E_LogError("FILE READ ERROR (path=%s)", path);
		assert(false);
		return NULL;
	}
	else
	{
		long currentpos = ftell(file);
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		fseek(file, currentpos, SEEK_SET);

		u32 filesizepadded = (size % 4 == 0 ? size * 4 : (size + 1) * 4) / 4;

		char* buffer = malloc(filesizepadded);
		if (buffer)
		{
			fread(buffer, size, sizeof(char), file);
			fclose(file);

			*output_size = size;
			return buffer;
		}
		else
		{
			fclose(file);
			E_LogError("FILE READ ERROR: Failed to allocate file output buffer!");
			*output_size = -1;
			assert(0);
			return NULL;
		}
	}

	return NULL;
}

void E_CompileShader(char* source, i32 source_size, E_Shader* shader)
{
	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	shaderc_compile_options_t options = shaderc_compile_options_initialize();

	shaderc_compile_options_set_source_language(options, shaderc_source_language_glsl);
	shaderc_compile_options_set_target_spirv(options, shaderc_spirv_version_1_0);
	shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, 0);
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level_size);

	shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source, source_size, E_ShaderKindToShaderC(shader->type), "shader", "main", options);
	
	shaderc_compilation_status status = shaderc_result_get_compilation_status(result);
	if (status != shaderc_compilation_status_success)
	{
		E_LogError("SHADERC ERROR: %s", shaderc_result_get_error_message(result));
		assert(false);
	}
	
	shader->code = (u8*)shaderc_result_get_bytes(result);
	shader->code_size = shaderc_result_get_length(result);

	shaderc_compiler_release(compiler);
}
