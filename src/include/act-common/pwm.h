#pragma once

// Use OCR2 to set current TOP value for Timer/Counter2
// Use TC2_PRESCALE to define prescaling you need (see 18.11.1 datasheet sec.)
// Requires TC2_PRESCALE be defined

#include <act-common/common.h>

inline __monitor void init_tc2_wg()
{
    /* Setup non-inverting Fast PWM and prescale */
    TCCR2 = (1<<WGM21)|(1<<WGM20)|(1<<COM21)|(0<<COM20)|(TC2_PRESCALE<<CS20);
}