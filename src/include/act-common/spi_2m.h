#pragma once

// Non-internal defines used:
//   REG_SPI_2M_HANDLER
//   SPI_IBUF_CONTAINER_T and SPI_OBUF_CONTAINER_T
//   SPI_LOCKING_POLICY

#ifndef SPI_IBUF_CONTAINER_T
#   define SPI_IBUF_CONTAINER_T array < byte, 32, byte >
#endif
#ifndef SPI_OBUF_CONTAINER_T
#   define SPI_OBUF_CONTAINER_T array < byte, 32, byte >
#endif
#ifndef SPI_LOCKING_POLICY
#   define SPI_LOCKING_POLICY lp_adopt_lock
#endif

#define SPI_GENERIC_STATUS ((1 << TWEN) | (1 << SPIE))

#include <act-common/common.h>
#include <act-common/iobuf.h>


typedef iobuf < SPI_IBUF_CONTAINER_T > spi_ibuf_t;
typedef iobuf < SPI_OBUF_CONTAINER_T > spi_obuf_t;


spi_ibuf_t spi_ibuf;
spi_obuf_t spi_obuf;


/**
 *  Notifies the SPI interface in order to
 *  take an initial action if it is required.
 */
void spi_notify()
{
    lock_guard < true > guard;

    byte cr = SPCR;

    /* Not selected slave mode */
    if ((!(cr & MSTR)) && (!(PINB & PINB2)))
    {
        /* Become master */
        SPCR |= (1 << MSTR);
    }
}


#ifdef REG_SPI_2M_HANDLER
#   pragma vector=SPI_STC_vect
    __interrupt
#else
    inline
#endif
void spi_2m_interrupt_handler()
{
    byte st = SPSR;
    byte cr = SPCR;
    byte data;
    spi_obuf_t::capacity_type ost;
    if (cr & MSTR) /* Master mode */
    {
        ost = iobuf_read < SPI_LOCKING_POLICY > (data, spi_obuf);
        if (ost == spi_obuf_t::capacity_type(0))
        {
            /* Become slave */
            SPCR &= ~(1 << MSTR);
        }
        else
        {
            SPDR = data;
        }
    }
    else
    {
        data = SPDR;
        iobuf_write < SPI_LOCKING_POLICY > (spi_ibuf, data);
    }
}
