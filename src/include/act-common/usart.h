#pragma once

// Requires IBUF_SIZE and OBUF_SIZE be defined
// Requires BAUD_RATE, FOSC be defined

/*
 * #30 <bug>
 *
 *     Race encountered on `iobuf_state`.
 *
 *     Split it on `ibuf_state` and `obuf_state`.
 *     Merge macro state constants (IBUF_NEMPTY, OBUF_NEMPTY
 *     -> IOBUF_NEMPTY).
 */




#include "common.h"




/* Disables RXCIE, executes code, enables RXCIE */
#define NO_RX_COMPLETE(code) \
    UCSRB &= ~(1 << RXCIE); /* Disable RX Complete interrupt */ \
    code \
    UCSRB |= (1 << RXCIE); /* Enable RX Complete interrupt */

/* Disables UDRIE, executes code, enables UDRIE */
#define NO_DATA_REG_EMPTY(code) \
    UCSRB &= ~(1 << UDRIE); /* Disable Data Reg. Empty interrupt */ \
    code \
    UCSRB |= (1 << UDRIE); /* Enable Data Reg. Empty interrupt */




/*
 * Marked `volatile` to prevent data caching
 * and other effects as the data is to be accessed
 * from both the interrupt and user code.
 */

volatile byte ibuf[IBUF_SIZE];
volatile byte obuf[OBUF_SIZE];

__tiny volatile byte ibuf_size = 0;
__tiny volatile byte obuf_size = 0;

__tiny volatile byte ibuf_head = 0;
__tiny volatile byte ibuf_tail = 0;
__tiny volatile byte obuf_head = 0;
__tiny volatile byte obuf_tail = 0;





/*
 * Reads a number of bytes from input buffer.
 *
 * Returns true if all bytes were actually read, false otherwise
 */
inline bool _iread(byte *out, byte size)
{
    if (size > ibuf_size) return false;
    for (byte i = 0; i < size; i++)
    {
       out[i] = ibuf[ibuf_head++];
       if (ibuf_head == IBUF_SIZE) ibuf_head = 0;
    }
    ibuf_size -= size;
    return true;
}





/* Accessed from the interrupt */
/*
 * Writes the byte to input buffer.
 *
 * Executes `on_success` on success, `on_failure` on failure.
 */
#define _iwrite(in, on_success, on_failure) \
    if (ibuf_size != IBUF_SIZE) /* not full */ \
    { \
        ibuf[ibuf_tail++] = in; /* write byte */ \
        ++ibuf_size; \
        if (ibuf_tail == IBUF_SIZE) ibuf_tail = 0; \
        on_success \
    } \
    else \
    { \
        on_failure \
    }





/* Accessed from the interrupt */
/*
 * Reads a byte from output buffer.
 *
 * Executes `on_success` on success, `on_failure` on failure.
 */
#define _oread(out, on_success, on_failure) \
    if (obuf_size != 0) /* not empty */ \
    { \
        out = obuf[obuf_head++]; /* read byte */ \
        --obuf_size; \
        if (obuf_head == OBUF_SIZE) obuf_head = 0; \
        on_success \
    } \
    else \
    { \
        on_failure \
    }





/*
 * Writes the byte to output buffer.
 *
 * Returns true if byte was actually written, false otherwise
 */
inline bool _owrite(const byte *in, byte size)
{
    if (size > OBUF_SIZE - obuf_size) return false;
    for (byte i = 0; i < size; i++)
    {
        obuf[obuf_tail++] = in[i];
        if (obuf_tail == OBUF_SIZE) obuf_tail = 0;
    }
    obuf_size += size;
    return true;
}





inline byte isize()
{
    NO_RX_COMPLETE(
        byte result = ibuf_size;
    )
    return result;
}





#pragma vector=USART_RXC_vect
__interrupt void usart_rxc_interrupt_handler()
{
    NO_RX_COMPLETE(
        /* Allow nested interrupts */
        __enable_interrupt();
        
        /* Must read the data anyway
           in order to suppress unnecessary interrupts */
        byte udr = UDR;
        _iwrite(udr, , ); /* Push data to the input buffer */
    )
}

#pragma vector=USART_UDRE_vect
__interrupt void usart_udre_interrupt_handler()
{
    /* Must do this manually since UDRIE must
       be cleared if output buffer become empty
       in order to suppress unnecessary series of interrupts */
    UCSRB &= ~(1 << UDRIE); /* Disable Data Reg. Empty interrupt */
    
    /* Allow nested interrupts */
    __enable_interrupt();
    
    /* Pop data from the output buffer to UDR */
    _oread(
        UDR,
        UCSRB |= (1 << UDRIE); /* Enable Data Reg. Empty interrupt on success */,
        /* do nothing on failure */
    );
}




// ========================================================================
// ===================== User API =========================================
// ========================================================================




inline bool transmit(const byte in)
{
    NO_DATA_REG_EMPTY(
        bool result = _owrite(&in, 1);
    );
    return result;
}

inline bool transmit_all(const byte *in, byte size)
{
    NO_DATA_REG_EMPTY(
        bool result = _owrite(in, size);
    );
    return result;
}

inline bool receive(byte *out)
{
    NO_RX_COMPLETE(
        bool result = _iread(out, 1);
    );
    return result;
}

inline bool receive_all(byte *out, byte size)
{
    NO_RX_COMPLETE(
        bool result = _iread(out, size);
    );
    return result;
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
