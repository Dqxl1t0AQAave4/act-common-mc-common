#pragma once

#include <cstring> /* for `memcpy` */

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
    sp_process_full = true,
    
    /**
     *  perform operation anyway,
     *  return a number of actually read / written elements
     */
    sp_process_any  = false
};


/**
 *  Reads data from `src` to `dst`.
 *
 *  Returns `0` on failure, `1 <= X <= length` on success.
 */
template
<
    success_policy process_full,
    locking_policy adopt_lock,
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
inline capacity_type iobuf_read
(
    storage_type * dst,
    iobuf < capacity_type, capacity, storage_type > & src,
    capacity_type  length
)
{
    /**
     *  src.length - volatile read
     *             - cannot be decreased in any parallel context
     *  so it is safe to just read it without locking the context
     */
    capacity_type src_length = src.length;
    
    /* nothing to read, just return */
    if (src_length == 0) return 0;
    
    /* the policy does not allow us to continue, just return */
    if (process_full && (src_length < length)) return 0;
    
    capacity_type src_pos    = src.position;
    
    /**
     *  to_read = process_full ? length : min ( src_length, length )
     */
    capacity_type to_read;
    if   (process_full) to_read = length;
    else                to_read = ((src_length < length) ? src_length : length);
    
    capacity_type remaining = (capacity - src_pos);
    
    if (remaining > to_read)
    {
        /**
         *  pointers   0   r         |   w   c
         *             |   |         |   |   |
         *             |-|-|x|x|x|x|x|-|-|-|-|
         *  to_read        |< - - - >|
         *  src_length     |< - - - - - >|
         *  remaining      |< - - - - - - - >|
         */
        memcpy(dst, src + src_pos, to_read);
    
        
        /**
         *  if lock already exists (e.g. inside interrupt vector)
         *  there is a field of optimizations - we are sure there
         *  is no parallel src.length incrementation
         */
        if (static_cast<bool>(adopt_lock))
        {
            src.position = src_pos + to_read;
            src.length   = src_length - to_read;
        }
        else
        {
            lock_guard < true > guard;
            src.position = src_pos + to_read;
            src.length   -= to_read; /* may change since previous reading */
        }
    }
    else
    {
        /**
         *  pointers   0   |   w     r       c
         *             |   |   |     |       |
         *             |x|x|-|-|-|-|-|x|x|x|x|
         *  to_read    |- >|         |< - - -|
         *  src_length |- - - >|     |< - - -|
         *  remaining                |< - - >|
         */
        
        memcpy(dst, src + src_pos, remaining);
        
        src_pos = to_read - remaining;
        
        memcpy(dst + remaining, src, src_pos);
        
        if (static_cast<bool>(adopt_lock))
        {
            src.position = src_pos;
            src.length   = src_length - to_read;
        }
        else
        {
            lock_guard < true > guard;
            src.position = src_pos;
            src.length   -= to_read; /* may change since previous reading */
        }
    }
    
    return to_read;
}



/**
 *  Same as iobuf_read < sp_process_any, lp_use_lock >
 */
template
<
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
inline capacity_type iobuf_read_any
(
    storage_type * dst,
    iobuf < capacity_type, capacity, storage_type > & src,
    capacity_type  length
)
{
    return iobuf_read < sp_process_any, lp_use_lock > (dst, src, length);
}



/**
 *  Same as iobuf_read < > (&dst, src, 1), but optimized a bit.
 */
template
<
    locking_policy adopt_lock,
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
inline capacity_type iobuf_read
(
    storage_type & dst,
    iobuf < capacity_type, capacity, storage_type > & src
)
{
    /**
     *  src.length - volatile read
     *             - cannot be decreased in any parallel context
     *  so it is safe to just read it without locking the context
     */
    capacity_type src_length = src.length;
    
    /* nothing to read, just return */
    if (src_length == 0) return 0;
    
    capacity_type new_pos    = src.position;
    
    /**
     *  to_read = 1
     */
    
    dst = *(src + new_pos);
    
    ++new_pos;
    
    if (new_pos != capacity)
    {
        /**
         *  pointers   0   r |           w   c
         *             |   | |           |   |
         *             |-|-|x|-|-|-|-|-|-|-|-|
         */
        
        /**
         *  if lock already exists (e.g. inside interrupt vector)
         *  there is a field of optimizations - we are sure there
         *  is no parallel src.length incrementation
         */
        if (static_cast<bool>(adopt_lock))
        {
            src.position = new_pos;
            src.length = src_length - 1;
        }
        else
        {
            lock_guard < true > guard;
            src.position = new_pos;
            src.length   -= 1; /* may change since previous reading */
        }
    }
    else
    {
        /**
         *  pointers   0                   r c,|
         *             |                   | |
         *             |-|-|-|-|-|-|-|-|-|-|x|
         */
        
        if (static_cast<bool>(adopt_lock))
        {
            src.position = 0;
            src.length   = src_length - 1;
        }
        else
        {
            lock_guard < true > guard;
            src.position = 0;
            src.length   -= 1; /* may change since previous reading */
        }
    }
    
    return 1;
}



/**
 *  Writes data from `src` to `dst`.
 *
 *  Returns `0` on failure, `1 <= X <= length` on success.
 */
template
<
    success_policy process_full,
    locking_policy adopt_lock,
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
inline capacity_type iobuf_write
(
    iobuf < capacity_type, capacity, storage_type > & dst,
    storage_type * src,
    capacity_type  length
)
{
    capacity_type dst_length;
    capacity_type dst_pos; /* initially -- just position */
    
    if (static_cast<bool>(adopt_lock))
    {
        dst_pos = dst.position;
        dst_length = dst.length;
    }
    else
    {
        lock_guard < true > guard;
        dst_pos = dst.position;
        dst_length = dst.length;
    }
    
    /* nothing to write, just return */
    if (dst_length == capacity) return 0;
    
    /* initially -- just free_space */
    capacity_type to_write = (capacity - dst_length);
    
    /* the policy does not allow us to continue, just return */
    if (process_full && (to_write < length)) return 0;
    
    /**
     *  to_write = process_full ? length : min ( free_space, length )
     */
    if (process_full || to_write > length) to_write = length;
    
    capacity_type remaining = (capacity - dst_pos);
    
    /**
     *  dst_pos = (position + length) mod capacity
     */
    if (dst_length >= remaining)
    {
        /**
         *  pointers   0   w             r   c
         *             |   |             |   |
         *             |-|-|-|-|-|-|-|-|-|-|-|
         *  length     |- >|             |< -|
         *  remaining                    |< >|
         */
        dst_pos = (dst_length - remaining);
    }
    else
    {
        /**
         *  pointers   0   r             w   c
         *             |   |             |   |
         *             |-|-|-|-|-|-|-|-|-|-|-|
         *  length         |< - - - - - >|    
         *  remaining      |< - - - - - - - >|
         */
        dst_pos = (dst_pos + dst_length);
    }
    
    remaining = (capacity - dst_pos);
    
    if (remaining > to_write)
    {
        /**
         *  pointers   0   w         |   r   c
         *             |   |         |   |   |
         *             |-|-|x|x|x|x|x|-|-|-|-|
         *  to_write       |< - - - >|
         *  free_space     |< - - - - - >|
         *  remaining      |< - - - - - - - >|
         */
        memcpy(dst + dst_pos, src, to_write);
    }
    else
    {
        /**
         *  pointers   0   |   r     w       c
         *             |   |   |     |       |
         *             |x|x|-|-|-|-|-|x|x|x|x|
         *  to_write   |- >|         |< - - -|
         *  free_space |- - - >|     |< - - -|
         *  remaining                |< - - >|
         */
        
        memcpy(dst + dst_pos, src, remaining);
        
        dst_pos = to_write - remaining;
        
        memcpy(dst, src + remaining, dst_pos);
    }
    
    /**
     *  if lock already exists (e.g. inside interrupt vector)
     *  there is a field of optimizations - we are sure there
     *  is no parallel dst.length decrementation
     */
    if (static_cast<bool>(adopt_lock))
    {
        dst.length = dst_length + to_write;
    }
    else
    {
        lock_guard < true > guard;
        dst.length += to_write; /* may change since previous reading */
    }
    
    return to_write;
}



/**
 *  Same as iobuf_write < sp_process_any, lp_use_lock >
 */
template
<
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
inline capacity_type iobuf_write_any
(
    iobuf < capacity_type, capacity, storage_type > & dst,
    storage_type * src,
    capacity_type  length
)
{
    return iobuf_write < sp_process_any, lp_use_lock > (dst, src, length);
}



/**
 *  Same as iobuf_write < > (dst, &src, 1), but optimized a bit.
 */
template
<
    locking_policy adopt_lock,
    typename capacity_type,
    capacity_type capacity,
    typename storage_type
>
inline capacity_type iobuf_write
(
    iobuf < capacity_type, capacity, storage_type > & dst,
    storage_type & src
)
{
    capacity_type dst_length;
    capacity_type dst_pos; /* initially -- just position */
    
    if (static_cast<bool>(adopt_lock))
    {
        dst_pos = dst.position;
        dst_length = dst.length;
    }
    else
    {
        lock_guard < true > guard;
        dst_pos = dst.position;
        dst_length = dst.length;
    }
    
    /* nothing to write, just return */
    if (dst_length == capacity) return 0;
    
    /**
     *  to_write = 1
     */
    
    capacity_type remaining = (capacity - dst_pos);
    
    /**
     *  dst_pos = (position + length) mod capacity
     */
    if (dst_length >= remaining)
    {
        /**
         *  pointers   0   w             r   c
         *             |   |             |   |
         *             |-|-|-|-|-|-|-|-|-|-|-|
         *  length     |- >|             |< -|
         *  remaining                    |< >|
         */
        dst_pos = (dst_length - remaining);
    }
    else
    {
        /**
         *  pointers   0   r             w   c
         *             |   |             |   |
         *             |-|-|-|-|-|-|-|-|-|-|-|
         *  length         |< - - - - - >|    
         *  remaining      |< - - - - - - - >|
         */
        dst_pos = (dst_pos + dst_length);
    }
    
    *(dst + dst_pos) = src;
    
    /**
     *  if lock already exists (e.g. inside interrupt vector)
     *  there is a field of optimizations - we are sure there
     *  is no parallel dst.length decrementation
     */
    if (static_cast<bool>(adopt_lock))
    {
        dst.length = dst_length + 1;
    }
    else
    {
        lock_guard < true > guard;
        dst.length += 1; /* may change since previous reading */
    }
    
    return 1;
}
