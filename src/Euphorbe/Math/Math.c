#include "Math.h"

static b8 rand_seeded = 0;

i32 E_Random()
{
    if (!rand_seeded) {
        srand((u32)platform_get_absolute_time());
        rand_seeded = 1;
    }
    return rand();
}

i32 E_RandomRange(i32 min, i32 max)
{
    if (!rand_seeded) {
        srand((u32)platform_get_absolute_time());
        rand_seeded = 1;
    }
    return (rand() % (max - min + 1)) + min;
}

f32 E_FRandom()
{
    return (float)E_Random() / (f32)RAND_MAX;
}

f32 E_FRandomRange(f32 min, f32 max)
{
    return min + ((float)E_Random() / ((f32)RAND_MAX / (max - min)));
}
