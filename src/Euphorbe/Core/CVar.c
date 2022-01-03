#include "CVar.h"

#include <toml.h>
#include <Euphorbe/Core/Log.h>

typedef enum E_CVarType E_CVarType;
enum E_CVarType
{
	E_CVarTypeString,
	E_CVarTypeInt,
	E_CVarTypeBool,
	E_CVarTypeDouble
};

E_CVarType E_GetCVarTypeFromString(char* str);

void E_CreateCVarSystem(char* path, E_CVarSystem* dst)
{
	dst->path = path;

	FILE* fp = NULL;
	char errbuf[200] = { 0 };

	fp = fopen(path, "r");
	if (!fp)
		E_LogError("CVAR SYSTEM ERROR: Failed to read file (path=%s)", path);

	toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
	fclose(fp);

	if (!conf)
		E_LogError("TOML READ ERROR: Failed to parse - %s", errbuf);

	toml_table_t* cvar_table = toml_table_in(conf, "CVarTable");
	if (!cvar_table)
		E_LogError("TOML READ ERROR: Failed to parse CVar Table - %s", errbuf);
	
	toml_array_t* cvar_map = toml_array_in(cvar_table, "Table");
	i32 cvar_count = toml_array_nelem(cvar_map);

	assert(cvar_count < EUPHORBE_MAX_CVAR_COUNT);
	dst->cvar_count = cvar_count;

	E_INIT_MAP(dst->cvar_map);

	for (i32 i = 0; i < cvar_count; i++) 
	{
		toml_array_t* cvar_descriptor = toml_array_at(cvar_map, i);

		toml_datum_t cvar_type = toml_string_at(cvar_descriptor, 0);
		assert(cvar_type.ok);

		toml_datum_t accessor = toml_string_at(cvar_descriptor, 1);
		assert(accessor.ok);

		E_CVarType type = E_GetCVarTypeFromString(cvar_type.u.s);
		switch (type)
		{
		case E_CVarTypeInt:
		{
			i64 value = toml_int_at(cvar_descriptor, 2).u.i;

			E_LogInfo("Loaded Integer CVar with name [%s] and value %d", accessor.u.s, value);
			E_MAP_GET(dst->cvar_map, accessor.u.s).u.i = value;
			E_MAP_GET(dst->cvar_map, accessor.u.s).ok = 1;

			free(accessor.u.s);
			free(cvar_type.u.s);
			break;
		}
		case E_CVarTypeBool:
		{
			b32 value = toml_bool_at(cvar_descriptor, 2).u.b;

			E_CVar temp = { 0 };
			temp.u.b = value;

			E_LogInfo("Loaded Boolean CVar with name [%s] and value %d", accessor.u.s, value);
			E_MAP_GET(dst->cvar_map, accessor.u.s).u.b = value;
			E_MAP_GET(dst->cvar_map, accessor.u.s).ok = 1;

			free(accessor.u.s);
			free(cvar_type.u.s);
			break;
		}
		case E_CVarTypeDouble:
		{
			f64 value = toml_double_at(cvar_descriptor, 2).u.d;

			E_CVar temp = { 0 };
			temp.u.d = value;

			E_LogInfo("Loaded Double CVar with name [%s] and value %f", accessor.u.s, value);
			E_MAP_GET(dst->cvar_map, accessor.u.s).u.d = value;
			E_MAP_GET(dst->cvar_map, accessor.u.s).ok = 1;

			free(accessor.u.s);
			free(cvar_type.u.s);
			break;
		}
		}
	}

	toml_free(conf);
}

void E_FreeCVarSystem(E_CVarSystem* sys)
{
	
}

E_CVar E_GetCVar(E_CVarSystem* sys, char* cvar_name)
{
	return E_MAP_GET(sys->cvar_map, cvar_name);
}

void E_SetCVar_Double(E_CVarSystem* sys, char* cvar_name, f64 value)
{
	E_CVar var = { 0 };
	var.u.d = value;
	E_MAP_MODIFY(sys->cvar_map, cvar_name, var);
}

void E_SetCVar_Bool(E_CVarSystem* sys, char* cvar_name, b32 value)
{
	E_CVar var = { 0 };
	var.u.b = value;
	E_MAP_MODIFY(sys->cvar_map, cvar_name, var);
}

void E_SetCVar_Int(E_CVarSystem* sys, char* cvar_name, i64 value)
{
	E_CVar var = { 0 };
	var.u.i = value;
	E_MAP_MODIFY(sys->cvar_map, cvar_name, var);
}

E_CVarType E_GetCVarTypeFromString(char* str)
{
	if (!strcmp(str, "INT"))
		return E_CVarTypeInt;
	if (!strcmp(str, "BOOL"))
		return E_CVarTypeBool;
	if (!strcmp(str, "DOUBLE"))
		return E_CVarTypeDouble;
}