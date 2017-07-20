#pragma once


#include <intrinsics.h>


/**
 *  The class is like std::lock_guard<>, but
 *  for microcontroller.
 *
 *  It enters critical section on creation
 *  by clearing interrupt flag and leaves critical
 *  section on descruction by restoring interrupt flag.
 *
 *  lock_guard < false > is supposed to be more
 *  lightweight as it does not save interrupt flag
 *  state, just clears on creation and sets on destruction.
 *
 *  Usage example:
 *
 *      {
 *          lock_guard < true > guard;
 *
 *          // enters critical section
 *
 *          shared_variable += some_value;
 *          // this code will not be interrupted
 *          another_shared_variable += another_value - shared_variable;
 *
 *          // leaves critical section
 *      }
 */


template < bool restore_interrupt >
class lock_guard;



template < >
class lock_guard < true >
{
private:
    const unsigned char saved_interrupt_state;
public:
    lock_guard()
        : saved_interrupt_state(__save_interrupt())
    {
        __disable_interrupt();
    }
    ~lock_guard()
    {
        __restore_interrupt(saved_interrupt_state);
    }
};



template < >
class lock_guard < false >
{
public:
    lock_guard()
    {
        __disable_interrupt();
    }
    ~lock_guard()
    {
        __enable_interrupt();
    }
};