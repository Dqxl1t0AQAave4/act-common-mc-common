#pragma once

// Requires REG_USART_RXC_HANDLER and REG_USART_UDRE_HANDLER be defined
// Requires IBUF_SIZE_T and OBUF_SIZE_T be defined
// Requires IBUF_SIZE and OBUF_SIZE be defined
// Requires BAUD_RATE, FOSC be defined

#ifndef IBUF_SIZE_T
#   define IBUF_SIZE_T byte
#endif
#ifndef OBUF_SIZE_T
#   define OBUF_SIZE_T byte
#endif
#ifndef IBUF_SIZE
#   define IBUF_SIZE IBUF_SIZE_T(32)
#endif
#ifndef OBUF_SIZE
#   define OBUF_SIZE IBUF_SIZE_T(32)
#endif

#include <act-common/common.h>
#include <act-common/iobuf.h>


typedef iobuf < IBUF_SIZE_T, IBUF_SIZE, byte > usart_ibuf_t;
typedef iobuf < OBUF_SIZE_T, OBUF_SIZE, byte > usart_obuf_t;


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
    IBUF_SIZE_T udr = UDR;
    
    iobuf_write < lp_adopt_lock > (usart_ibuf, udr);
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
    OBUF_SIZE_T has_data = iobuf_read < lp_adopt_lock > (udr, usart_obuf);
    
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


/* Disable interrupts (see ATmega8A datasheet) */
inline __monitor void usart_init()
{
    const int ubrr = FOSC/16/BAUD_RATE-1;
    /* Set baud rate */
    UBRRH = (unsigned char) (ubrr>>8);
    UBRRL = (unsigned char) ubrr;
    /* Enable receiver and transmitter */
    UCSRB = (1<<RXEN)|(1<<TXEN);
    /* Set frame format: 8data, 1stop bit, parity odd bit */
    UCSRC = (1<<URSEL)|(3<<UCSZ0)|(3<<UPM0);
     /* Enable RX Complete and Data Reg. Empty interrupt */
    UCSRB |= (1<<RXCIE)|(1<<UDRIE);
}
