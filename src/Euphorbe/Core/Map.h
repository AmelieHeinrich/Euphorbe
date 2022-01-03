#ifndef EUPHORBE_MAP_H
#define EUPHORBE_MAP_H

#include <Euphorbe/Core/Common.h>

#define EUPHORBE_CONSTANT_MAP_SIZE 128

#define E_DEFINE_MAP_T(name, type) typedef struct E_Map##name E_Map##name;\
							 struct E_Map##name\
							 {\
								##type data[EUPHORBE_CONSTANT_MAP_SIZE];\
								##type size_check;\
							 };

u32 E_Hash(char* name);

#define E_INIT_MAP(map) memset(map.data, 0, sizeof(map.size_check) * EUPHORBE_CONSTANT_MAP_SIZE);
#define E_HASH_MAP(map, name) E_Hash(name)

#define E_MAP_INSERT(map, name, value) i32 index = E_HASH_MAP(map, name);\
									   map.data[index] = value;

#define E_MAP_GET(map, name) map.data[E_HASH_MAP(map, name)]
#define E_MAP_REMOVE(map, name) memset(&E_MAP_GET(map, name), 0, sizeof(map.size_check))
#define E_MAP_MODIFY(map, name, value) map.data[E_HASH_MAP(map, name)] = value
		
#endif