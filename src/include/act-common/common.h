#pragma once

#include <ioavr.h>
#include <intrinsics.h>

// INT_MIN is -32767 to be sure that INT_MIN + INT_MAX = 0
#define INT_MAX 32767
#define INT_MIN -32767

#define ABS(a) ((a) < 0 ? -(a) : (a))

typedef unsigned char byte;
typedef signed char sbyte;
#ifndef DLIB
typedef byte bool;
#define true 1
#define false 0
#endif


inline byte ulog2 (unsigned int u)
{
    byte s, t;

    t = (u > 0xffu) << 3; u >>= t;
    s = (u > 0xfu ) << 2; u >>= s, t |= s;
    s = (u > 0x3u ) << 1; u >>= s, t |= s;

    return (t | (u >> 1));
}

inline int safe_add(int a, int b)
{
    if (!(a >= 0 && b >= 0) && !(a <= 0 && b <= 0))
    {
        return a + b;
    }
    int aa = ABS(a), ab = ABS(b);
    if (INT_MAX - aa >= ab)
    {
        return a + b;
    }
    else
    {
        return (aa > ab ? (a < 0 ? INT_MIN : INT_MAX) : (b < 0 ? INT_MIN : INT_MAX));
    }
}