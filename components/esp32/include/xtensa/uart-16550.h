/*******************************************************************************

  Copyright (c) 2006-2007 Tensilica Inc.

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

--------------------------------------------------------------------------------

uart-16550.h    Generic definitions for National Semiconductor 16550 UART

This is used by board-support-packages with one or more 16550 compatible UARTs.
A BSP provides a base address for each instance of a 16550 UART on the board.

Note that a 16552 DUART (Dual UART) is simply two instances of a 16550 UART.

*******************************************************************************/

#ifndef _UART_16550_H_
#define _UART_16550_H_

/* C interface to UART registers. */
struct uart_dev_s {
  union {
    uart16550_reg_t rxb;    /* DLAB=0: receive buffer, read-only */
    uart16550_reg_t txb;    /* DLAB=0: transmit buffer, write-only */
    uart16550_reg_t dll;    /* DLAB=1: divisor, LS byte latch */
  } w0;
  union {
    uart16550_reg_t ier;    /* DLAB=0: interrupt-enable register */
    uart16550_reg_t dlm;    /* DLAB=1: divisor, MS byte latch */
  } w1;

  union {
    uart16550_reg_t isr;    /* DLAB=0: interrupt status register, read-only */
    uart16550_reg_t fcr;    /* DLAB=0: FIFO control register, write-only */
    uart16550_reg_t afr;    /* DLAB=1: alternate function register */
  } w2;

  uart16550_reg_t lcr;      /* line control-register, write-only */
  uart16550_reg_t mcr;      /* modem control-regsiter, write-only */
  uart16550_reg_t lsr;      /* line status register, read-only */
  uart16550_reg_t msr;      /* modem status register, read-only */
  uart16550_reg_t scr;      /* scratch regsiter, read/write */
};


#define _RXB(u) ((u)->w0.rxb)
#define _TXB(u) ((u)->w0.txb)
#define _DLL(u) ((u)->w0.dll)
#define _IER(u) ((u)->w1.ier)
#define _DLM(u) ((u)->w1.dlm)
#define _ISR(u) ((u)->w2.isr)
#define _FCR(u) ((u)->w2.fcr)
#define _AFR(u) ((u)->w2.afr)
#define _LCR(u) ((u)->lcr)
#define _MCR(u) ((u)->mcr)
#define _LSR(u) ((u)->lsr)
#define _MSR(u) ((u)->msr)
#define _SCR(u) ((u)->scr)

typedef volatile struct uart_dev_s uart_dev_t;

/* IER bits */
#define RCVR_DATA_REG_INTENABLE 0x01
#define XMIT_HOLD_REG_INTENABLE 0x02
#define RCVR_STATUS_INTENABLE   0x04
#define MODEM_STATUS_INTENABLE  0x08

/* FCR bits */
#define _FIFO_ENABLE     0x01
#define RCVR_FIFO_RESET  0x02
#define XMIT_FIFO_RESET  0x04
#define DMA_MODE_SELECT  0x08
#define RCVR_TRIGGER_LSB 0x40
#define RCVR_TRIGGER_MSB 0x80

/* AFR bits */
#define AFR_CONC_WRITE  0x01
#define AFR_BAUDOUT_SEL 0x02
#define AFR_RXRDY_SEL   0x04

/* ISR bits */
#define INT_STATUS(r)   ((r)&1)
#define INT_PRIORITY(r) (((r)>>1)&0x7)

/* LCR bits */
#define WORD_LENGTH(n)  (((n)-5)&0x3)
#define STOP_BIT_ENABLE 0x04
#define PARITY_ENABLE   0x08
#define EVEN_PARITY     0x10
#define FORCE_PARITY    0x20
#define XMIT_BREAK      0x40
#define DLAB_ENABLE     0x80

/* MCR bits */
#define _DTR 0x01
#define _RTS 0x02
#define _OP1 0x04
#define _OP2 0x08
#define LOOP_BACK 0x10

/* LSR Bits */
#define RCVR_DATA_READY 0x01
#define OVERRUN_ERROR   0x02
#define PARITY_ERROR    0x04
#define FRAMING_ERROR   0x08
#define BREAK_INTERRUPT 0x10
#define XMIT_HOLD_EMPTY 0x20
#define XMIT_EMPTY      0x40
#define FIFO_ERROR      0x80
#define RCVR_READY(u)   (_LSR(u)&RCVR_DATA_READY)
#define XMIT_READY(u)   (_LSR(u)&XMIT_HOLD_EMPTY)

/* MSR bits */
#define _RDR       0x01
#define DELTA_DSR  0x02
#define DELTA_RI   0x04
#define DELTA_CD   0x08
#define _CTS       0x10
#define _DSR       0x20
#define _RI        0x40
#define _CD        0x80


/* Compute 16-bit divisor for baudrate generator, with rounding:  */
#define UART_DIVISOR(clock,baud)    (((clock)/16 + (baud)/2)/(baud))

/* Prototypes of driver functions */
extern void     uart16550_init( uart_dev_t *u, unsigned baud, unsigned ndata,
                                unsigned parity, unsigned nstop );
extern void     uart16550_out( uart_dev_t *u, char c );
extern char     uart16550_in( uart_dev_t *u );
extern unsigned uart16550_measure_sys_clk( uart_dev_t *u );

#endif /* _UART_16550_H_ */
