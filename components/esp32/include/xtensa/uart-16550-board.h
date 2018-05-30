/*******************************************************************************
Copyright (c) 2006-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
These coded instructions, statements, and computer programs are the
copyrighted works and confidential proprietary information of Tensilica Inc.
They may not be modified, copied, reproduced, distributed, or disclosed to
third parties in any manner, medium, or form, in whole or in part, without
the prior written consent of Tensilica Inc.
--------------------------------------------------------------------------------

uart-16550-board.h Board-specific UART info on these boards:
	Avnet AV60 (XT-AV60)
	Avnet AV110 (XT-AV110)
	Avnet AV200 (XT-AV200)
	Xilinx ML605 (XT-ML605)
	Xilinx KC705 (XT-KC705)

Interface between board-independent driver and board-specific header.

This is used by a board-independent 16550 UART driver to obtain board-specific
information about 1 instance of the 16550 UART on the board, such as the device
register base address and spacing (a function of how the address lines are 
connected on the board) and the frequency of the UART clock. The driver does
not refer directly to the board-specific header, which therefore is not 
constrained to use macro names consistent with other boards.

!! Must not contain any board-specific macro names (only UART specific). !!

Included at compile-time via an include path specific to the board.

These boards contain a single 16550 UART implemented on the FPGA.
Their clock frequency comes from the board's core clock (not its own crystal)
which depends on the core config so is not a constant. Obtained via the BSP.

*******************************************************************************/

#ifndef _UART_16550_BOARD_H
#define _UART_16550_BOARD_H

#include <xtensa/xtbsp.h>               /* BSP API */
#include <xtensa/board.h>               /* Board info */


/* Base address of UART's registers. */
#ifdef UART16550_VADDR
#define UART_16550_REGBASE UART16550_VADDR
#endif

/* 
The UART's registers are connected at word addresses on these boards.
Each byte-wide register appears as the least-significant-byte (LSB) of the 
word regardless of the endianness of the processor.
*/
#define UART_16550_REGSPACING 4
typedef unsigned uart16550_reg_t;

/* UART Clock Frequency in Hz */
#define UART_16550_XTAL_FREQ xtbsp_clock_freq_hz()

/* UART Interrupt Number */
#ifdef UART16550_INTNUM
#define UART_16550_INTNUM UART16550_INTNUM
#endif


/* Include generic information shared by all boards that use this device. */
#include    <xtensa/uart-16550.h>

#endif /* _UART_16550_BOARD_H */

