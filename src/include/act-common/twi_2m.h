#pragma once

// Non-internal defines used:
//   REG_TWI_2M_HANDLER
//   TWI_IBUF_CONTAINER_T and TWI_OBUF_CONTAINER_T
//   TWI_LOCKING_POLICY
//   TWI_PRESCALING
//   TWI_2M_MASTER_ADDRESS

#ifndef TWI_IBUF_CONTAINER_T
#   define TWI_IBUF_CONTAINER_T array < byte, 32, byte >
#endif
#ifndef TWI_OBUF_CONTAINER_T
#   define TWI_OBUF_CONTAINER_T array < byte, 32, byte >
#endif
#ifndef TWI_LOCKING_POLICY
#   define TWI_LOCKING_POLICY lp_adopt_lock
#endif

#ifdef TWI_PRESCALING
#   define TWI_STATUS(s) (s & 0xf8) /* mask prescaler bits */
#else
#   define TWI_STATUS(s) (s) /* no masking */
#endif

#define TWI_GENERIC_STATUS ((1 << TWEN) | (1 << TWIE) | (1 << TWINT))

#include <act-common/common.h>
#include <act-common/iobuf.h>


typedef iobuf < TWI_IBUF_CONTAINER_T > twi_ibuf_t;
typedef iobuf < TWI_OBUF_CONTAINER_T > twi_obuf_t;


twi_ibuf_t twi_ibuf;
twi_obuf_t twi_obuf;


byte twi_last_sent_byte;

enum twi_mode
{
    twi_mode_not_addressed = 0,
    twi_mode_slave         = 1,
    twi_mode_master        = 2
};

byte twi_current_mode = twi_mode_not_addressed;


void _twi_touch()
{
    /* Try to enter the master mode */
    if (twi_current_mode == twi_mode_not_addressed)
    {
        if (twi_obuf.length != 0)
        {
            TWSR = TWI_GENERIC_STATUS | (1 << TWSTA);
            twi_current_mode = twi_mode_master;
        }
    }
}


/**
 *  "Touches" the TWI interface in order to
 *  take an initial action as a master if it
 *  is required.
 */
void twi_touch()
{
    lock_guard < true > guard;
    _twi_touch();
}


#ifdef REG_TWI_2M_HANDLER
#   pragma vector=TWI_vect
    __interrupt
#else
    inline
#endif
void twi_slave_interrupt_handler()
{
    byte twdr;
    twi_ibuf_t::capacity_type ist;
    twi_obuf_t::capacity_type ost;

    byte sr = TWI_STATUS(TWSR);
    switch(sr)
    {

    /* ********* Slave Receiver Mode ********* */

    /* Own SLA+W has been received; ACK has been returned */
    case 0x60:
    /* General call address has been received; ACK has been returned */
    case 0x70:
    /* Arbitration lost in SLA+R/W as Master; own SLA+W has been
       received; ACK has been returned */
    case 0x68: /* should not normally occur as we operate as a slave */
    /* Arbitration lost in SLA+R/W as Master; General call address has
       been received; ACK has been returned */
    case 0x78: /* should not normally occur as we operate as a slave */
        /* Just send ACK, clear the interrupt bit
           and switch to addressed mode */
        TWSR = TWI_GENERIC_STATUS | (1 << TWEA);
        twi_current_mode = twi_mode_slave;
        break;


    /* Previously addressed with own SLA+W; data has been received;
       ACK has been returned */
    case 0x80:
    /* Previously addressed with general call; data has been received;
       ACK has been returned */
    case 0x90:
    /* Previously addressed with own SLA+W; data has been received;
       NOT ACK has been returned */
    case 0x88:
    /* Previously addressed with general call; data has been received;
       NOT ACK has been returned */
    case 0x98:
        /* Read the data and try to insert it into the buffer */
        twdr = TWDR;
        ist = iobuf_write < TWI_LOCKING_POLICY > (twi_ibuf, twdr);
        /* If cannot, send NACK and clear the interrupt bit */
        if (ist == twi_ibuf_t::capacity_type(0))
            TWSR = TWI_GENERIC_STATUS;
        /* Otherwise send ACK and clear the interrupt bit */
        else
            TWSR = TWI_GENERIC_STATUS | (1 << TWEA);
        break;


    /* A STOP condition or repeated START condition has been
       received while still addressed as Slave */
    case 0xA0:
        /* We should restore TWEA
           and switch to not addressed mode */
        TWSR = TWI_GENERIC_STATUS | (1 << TWEA);
        twi_current_mode = twi_mode_not_addressed;
        break;


    /* ********* Master Transmitter Mode ********* */

    /* A START condition has been transmitted */
    case 0x08:
    /* A repeated START condition has been transmitted */
    case 0x10:
        /* If we are here, the output buffer should not be empty.
           Just send the address of master */
        TWDR = TWI_2M_MASTER_ADDRESS;
        TWSR = TWI_GENERIC_STATUS;
        break;


    /* SLA+W has been transmitted; ACK has been received */
    case 0x18:
    /* Data byte has been transmitted; ACK has been received */
    case 0x28:
        /* Try to get the data from the buffer and transmit it */
        ost = iobuf_read < TWI_LOCKING_POLICY > (twdr, twi_obuf);
        /* No data to send, send STOP condition */
        if (ost == twi_obuf_t::capacity_type(0))
        {
            TWSR = TWI_GENERIC_STATUS | (1 << TWSTO);
        }
        /* Send the data */
        else
        {
            TWDR = twdr;
            TWSR = TWI_GENERIC_STATUS;
            twi_last_sent_byte = twdr;
        }
        break;


    /* SLA+W has been transmitted; NOT ACK has been received */
    case 0x20:
    /* Data byte has been transmitted; NOT ACK has been received */
    case 0x30:
        /* Resend the previous data */
        TWDR = twi_last_sent_byte;
        TWSR = TWI_GENERIC_STATUS;
        break;


    /* Arbitration lost in SLA+W or data bytes */
    case 0x38:
        /* Switch to not addressed mode */
        TWSR = TWI_GENERIC_STATUS;
        twi_current_mode = twi_mode_not_addressed;
        break;


    /* ****************** Other ****************** */

    /* Unpredicted state; ignore it just clearing
       the interrupt flag and switching to not addressed mode */
    default:
        TWSR = TWI_GENERIC_STATUS;
        twi_current_mode = twi_mode_not_addressed;
        break;
    }

    /* Try to enter the master mode */
    _twi_touch();
}
