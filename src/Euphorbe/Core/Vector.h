#ifndef EUPHORBE_VECTOR_H
#define EUPHORBE_VECTOR_H

#include <Euphorbe/Core/Common.h>

#define E_DEFINE_VECTOR(name, type) typedef struct E_Vector##name E_Vector##name;\
							 struct E_Vector##name\
							 {\
								##type *data;\
								##type size_check;\
								u32 used;\
								u32 size;\
							 };

#define E_INIT_VECTOR(vec, init_size) vec.data = malloc(init_size * sizeof(vec.size_check)); vec.size = init_size; vec.used = 0
#define E_FREE_VECTOR(vec) free(vec.data); vec.size = vec.used = 0
#define E_PUSH_BACK_VECTOR(vec, elem) if (vec.used >= vec.size) { vec.size *= 2; vec.data = realloc(vec.data, vec.size * sizeof(vec.size_check)); } vec.data[vec.used++] = elem
#define E_POP_BACK_VECTOR(vec) memset(&vec.data[vec.used], 0, sizeof(vec.size_check)); vec.used--
#define E_SIZE_VECTOR(vec) vec.size
#define E_CAPACITY_VECTOR(vec) vec.size

#endif