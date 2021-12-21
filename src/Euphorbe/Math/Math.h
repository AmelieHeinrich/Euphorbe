#ifndef EUPHORBE_MATH_H
#define EUPHORBE_MATH_H

#include <math.h>

#include "Types.h"

#define E_PI 3.14159265358979323846f
#define E_PI_2 2.0f * K_PI
#define E_HALF_PI 0.5f * K_PI
#define E_QUARTER_PI 0.25f * K_PI
#define E_ONE_OVER_PI 1.0f / K_PI
#define E_ONE_OVER_TWO_PI 1.0f / K_PI_2
#define E_SQRT_TWO 1.41421356237309504880f
#define E_SQRT_THREE 1.73205080756887729352f
#define E_SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define E_SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define E_DEG2RAD_MULTIPLIER K_PI / 180.0f
#define E_RAD2DEG_MULTIPLIER 180.0f / K_PI
#define E_SEC_TO_MS_MULTIPLIER 1000.0f
#define E_MS_TO_SEC_MULTIPLIER 0.001f
#define E_INFINITY 1e30f
#define E_FLOAT_EPSILON 1.192092896e-07f

i32 E_Random();
i32 E_RandomRange(i32 min, i32 max);

f32 E_FRandom();
f32 E_FRandomRange(f32 min, f32 max);

inline V2 V2Create(f32 x, f32 y) {
	return (V2) {
		x, y
	};
}

inline V2 V2Zero() {
	return (V2) { 0.0f, 0.0f };
}

inline V2 V2One() {
	return (V2) { 1.0f, 1.0f };
}

inline V2 V2Up() {
	return (V2) { 0.0f, 1.0f };
}

inline V2 V2Down() {
	return (V2) { 0.0f,-1.0f };
}

inline V2 V2Left() {
	return (V2) {-1.0f, 0.0f };
}

inline V2 V2Right() {
	return (V2) { 1.0f, 0.0f };
}

inline V2 V2Add(V2 left, V2 right) {
	return (V2) {
		left.x + right.x,
		left.y + right.y
	};
}

inline V2 V2Sub(V2 left, V2 right) {
	return (V2) {
		left.x - right.x,
		left.y - right.y
	};
}

inline V2 V2Mul(V2 left, V2 right) {
	return (V2) {
		left.x * right.x,
		left.y * right.y
	};
}

inline V2 V2Div(V2 left, V2 right) {
	return (V2) {
		left.x / right.x,
		left.y / right.y
	};
}

inline f32 V2LengthSquared(V2 left) {
	return left.x * left.x + left.y * left.y;
}

inline f32 V2Length(V2 left) {
	return (f32)sqrt(V2LengthSquared(left));
}

inline void V2Normalize(V2* left) {
	const f32 length = V2Length(*left);
	left->x /= length;
	left->y /= length;
}

inline V2 V2Normalized(V2 left) {
	V2Normalize(&left);
	return left;
}

inline f32 V2Distance(V2 left, V2 right) {
	V2 d = (V2) {
		left.x - right.x,
		left.y - right.y 
	};
	return V2Length(d);
}

inline V3 V3Create(f32 x, f32 y, f32 z) {
	return (V3) {
		x, y, z
	};
}

inline V3 V3FromV4(V4 vec) {
	return (V3) {
		vec.x, vec.y, vec.z
	};
}

inline V3 V3Zero() {
	return V3Create(0.0f, 0.0f, 0.0f);
}

inline V3 V3One() {
	return V3Create(1.0f, 1.0f, 1.0f);
}

inline V3 V3Up() {
	return V3Create(0.0f, 1.0f, 0.0f);
}

inline V3 V3Down() {
	return V3Create(0.0f, -1.0f, 0.0f);
}

inline V3 V3Left() {
	return V3Create(-1.0f, 0.0f, 0.0f);
}

inline V3 V3Right() {
	return V3Create(1.0f, 0.0f, 0.0f);
}

inline V3 V3Forward() {
	return V3Create(0.0f, 0.0f, 1.0f);
}

inline V3 V3Back() {
	return V3Create(0.0f, 0.0f, -1.0f);
}

inline V3 V3Add(V3 left, V3 right) {
	return (V3) {
		left.x + right.x,
		left.y + right.y,
		left.z + right.z
	};
}

inline V3 V3Sub(V3 left, V3 right) {
	return (V3) {
		left.x - right.x,
		left.y - right.y,
		left.z - right.z
	};
}

inline V3 V3Mul(V3 left, V3 right) {
	return (V3) {
		left.x * right.x,
		left.y * right.y,
		left.z * right.z
	};
}

inline V3 V3MulScalar(V3 left, f32 scalar) {
	return (V3) {
		left.x * scalar,
		left.y * scalar,
		left.z * scalar
	};
}

inline V3 V3Div(V3 left, V3 right) {
	return (V3) {
		left.x / right.x,
		left.y / right.y,
		left.z / right.z
	};
}

inline f32 V3LenghtSquared(V3 left) {
	return left.x * left.x + left.y * left.y + left.z * left.z;
}

inline f32 V3Length(V3 left) {
	return (f32)sqrt(V3LenghtSquared(left));
}

inline void V3Normalize(V3* vector) {
	const f32 length = V3Length(*vector);
	vector->x /= length;
	vector->y /= length;
	vector->z /= length;
}

inline V3 V3Normalized(V3 vector) {
	V3Normalize(&vector);
	return vector;
}

inline f32 V3Dot(V3 left, V3 right) {
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline V3 V3Cross(V3 left, V3 right) {
	return (V3) {
		left.y * right.z - left.z * right.y,
		left.z * right.x - left.x * right.z,
		left.x * right.y - left.y * right.x
	};
}

inline f32 V3Distance(V3 left, V3 right) {
	V3 d = (V3) {
		left.x - right.x,
		left.y - right.y,
		left.z - right.z 
	};
	return V3Length(d);
}

inline V4 V4FromV3(V3 vec) {
	return (V4) {
		vec.x, vec.y, vec.z, 0.0f
	};
}

#endif