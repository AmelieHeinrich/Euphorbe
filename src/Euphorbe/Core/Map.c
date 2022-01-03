#include "Map.h"

u32 E_Hash(char* name)
{
	i32 len = strlen(name);
	u32 result = 0;
	for (i32 i = 0; i < len; i++) {
		result += name[i];
		result = (result * name[i]) % EUPHORBE_CONSTANT_MAP_SIZE;
	}
	return result;
}
