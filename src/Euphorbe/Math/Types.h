#ifndef EUPHORBE_MATH_TYPES_H
#define EUPHORBE_MATH_TYPES_H

#include <Euphorbe/Core/Common.h>

typedef union V2_Union {
    f32 data[2];
    struct {
        union {
            f32 x, r, s, u;
        };
        union {
            f32 y, g, t, v;
        };
    };
} V2;

typedef union V3_Union {
    f32 data[3];
    struct {
        union {
            f32 x, r, s, u;
        };
        union {
            f32 y, g, t, v;
        };
        union {
            f32 z, b, p, w;
        };
    };
} V3;

typedef union V4_Union {
    f32 data[4];
    union {
        struct {
            union {
                f32 x, r, s;
            };
            union {
                f32 y, g, t;
            };
            union {
                f32 z, b, p;
            };
            union {
                f32 w, a, q;
            };
        };
    };
} V4;

typedef V4 Quaternion;

typedef union M4_Union {
    f32 data[16];
} M4;

#endif