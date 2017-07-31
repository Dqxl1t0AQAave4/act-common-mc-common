#pragma once

// Requires REG_USART_RXC_HANDLER and REG_USART_UDRE_HANDLER be defined
// Requires IBUF_CONTAINER_T and OBUF_CONTAINER_T be defined
// Requires USART_LOCKING_POLICY be defined

#ifndef IBUF_CONTAINER_T
#   define IBUF_CONTAINER_T array < byte, 32, byte >
#endif
#ifndef OBUF_CONTAINER_T
#   define OBUF_CONTAINER_T array < byte, 32, byte >
#endif
#ifndef USART_LOCKING_POLICY
#   define USART_LOCKING_POLICY lp_adopt_lock
#endif

#include <act-common/common.h>
#include <act-common/iobuf.h>


typedef iobuf < IBUF_CONTAINER_T > usart_ibuf_t;
typedef iobuf < OBUF_CONTAINER_T > usart_obuf_t;


usart_ibuf_t usart_ibuf;
usart_obuf_t usart_obuf;


#ifdef REG_USART_RXC_HANDLER
#   pragma vector=USART_RXC_vect
    __interrupt
#else
    inline
#endif
void usart_rxc_interrupt_handler()
{
    /* Must read the data anyway
       in order to suppress unnecessary interrupts */
    byte udr = UDR;
    
    iobuf_write < USART_LOCKING_POLICY > (usart_ibuf, udr);
}


#ifdef REG_USART_RXC_HANDLER
#   pragma vector=USART_UDRE_vect
    __interrupt
#else
    inline
#endif
void usart_udre_interrupt_handler()
{
    byte udr;
    usart_obuf_t::capacity_type has_data = iobuf_read < USART_LOCKING_POLICY > (udr, usart_obuf);
    
    if (has_data == OBUF_SIZE_T(0) /* false */)
    {
        /* Must do this manually since UDRIE must
           be cleared if output buffer becomes empty
           in order to suppress unnecessary series of interrupts */
        UCSRB &= ~(1 << UDRIE); /* Disable Data Reg. Empty interrupt */
    }
    else
    {
        UDR = udr;
    }
}
