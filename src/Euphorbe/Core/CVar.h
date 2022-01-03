#ifndef EUPHORBE_CVAR_H
#define EUPHORBE_CVAR_H

#include "Map.h"

#define EUPHORBE_MAX_CVAR_NAME 128
#define EUPHORBE_MAX_CVAR_COUNT 512

typedef struct E_CVar E_CVar;
struct E_CVar
{
	u32 ok;
	union {
		i64 i;
		b32 b;
		f64 d;
	} u;
};

E_DEFINE_MAP_T(CVar, E_CVar)

typedef struct E_CVarSystem E_CVarSystem;
struct E_CVarSystem
{
	E_MapCVar cvar_map;
	i32 cvar_count;

	char* path;
};

void E_CreateCVarSystem(char* path, E_CVarSystem* dst);
void E_FreeCVarSystem(E_CVarSystem* sys);
E_CVar E_GetCVar(E_CVarSystem* sys, char* cvar_name);

//
void E_SetCVar_Double(E_CVarSystem* sys, char* cvar_name, f64 value);
void E_SetCVar_Bool(E_CVarSystem* sys, char* cvar_name, b32 value);
void E_SetCVar_Int(E_CVarSystem* sys, char* cvar_name, i64 value);

#endif