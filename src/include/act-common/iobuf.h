#pragma once

#include <act-common/common.h>
#include <act-common/array.h>
#include <act-common/lock_guard.h>


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



/* =========================== user-side =================================== */


/**
 *  Locking policy.
 */
enum locking_policy
{
  
    /* use existing one         */
    lp_adopt_lock = true,
    
    /* perform locking manually */
    lp_use_lock   = false

};


/**
 *  Success policy.
 */
enum success_policy
{
  
    /**
     *  perform operation if and only if the buffer
     *  has enough space to read / write an exact
     *  specified amount of elements,
     *  return either 0 or the requested amount of elements
     */
    sp_read_full = true,
    
    /**
     *  perform operation anyway,
     *  return a number of actually read / written elements
     */
    sp_read_any  = false
};
