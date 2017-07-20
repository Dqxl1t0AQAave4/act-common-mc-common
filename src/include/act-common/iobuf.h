#pragma once

#include <act-common/common.h>
#include <act-common/array.h>


/* =========================== class ======================================= */


/**
 *  Buffer structure:
 *      - c = capacity
 *      - r = position
 *      - w = (position + length) mod capacity
 *
 *  0. let `===` be `identically equal`, `same as`
 *
 *  1. (position + length < capacity) === (remaining > length)
 *
 *      pointers  0   r             w   c
 *                |   |             |   |
 *      data      |-|-|-|-|-|-|-|-|-|-|-|   e.g. byte[11]
 *      capacity  |< - - - - - - - - - >|   = 11 elements
 *      length        |< - - - - - >|       = 7  elements
 *      remaining     |< - - - - - - - >|   = 9  elements
 *
 *  2. (position + length > capacity) === (remaining < length)
 *
 *      pointers  0       w     r       c
 *                |       |     |       |
 *      data      |-|-|-|-|-|-|-|-|-|-|-|   e.g. byte[11]
 *      capacity  |< - - - - - - - - - >|   = 11 elements
 *      length    |- - - >|     |< - - -|   = 8  elements
 *      remaining               |< - - >|   = 4  elements
 *
 *  3. `read (1 element)` operation
 *
 *      - (r++) mod capacity === (position++, length--) mod capacity
 *
 *  4. `write (1 element)` operation
 *
 *      - (w++) mod capacity === (length++) mod capacity
 *  
 */
template
<
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
struct iobuf
    : public array < capacity_type, capacity, storage_type >
{
    volatile capacity_type position;
    volatile capacity_type length;
    
    iobuf() : position(0), length(0) { }
};


