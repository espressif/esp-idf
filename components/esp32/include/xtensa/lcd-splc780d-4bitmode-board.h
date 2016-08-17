/*******************************************************************************
Copyright (c) 2009-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
These coded instructions, statements, and computer programs are the
copyrighted works and confidential proprietary information of Tensilica Inc.
They may not be modified, copied, reproduced, distributed, or disclosed to
third parties in any manner, medium, or form, in whole or in part, without
the prior written consent of Tensilica Inc.
--------------------------------------------------------------------------------

lcd-splc780d-4bitmode-board.h    Board-specific LCD info for these boards:
	Avnet AV110 (XT-AV110)
	Xilinx ML605 (XT-ML605)
	Xilinx KC705 (XT-KC705)

Interface between board-independent driver and board-specific header.

This is used by a board-independent SPLC780D LCD controller (4 bit mode)
driver to obtain board-specific information about LCD displays on the board, 
such as the controller register base address and spacing (a function of how 
the address lines are connected on the board) and length of the visible window 
of the display (a function of the LCD panel the controller drives). 
The driver doesnot refer directly to the board-specific header, which therefore is not 
constrained to use macro names consistent with other boards.

!! Must not contain any board-specific macro names (only controller specific) !!

Included at compile-time via an include path specific to the board.

The listed boards contain a single MYTech MOC-16216B-B display driven by 
a Sunplus SPLC870D controller.

*******************************************************************************/

#ifndef _LCD_SPLC780D_4BIT_BOARD_H
#define _LCD_SPLC780D_4BIT_BOARD_H

#include <xtensa/board.h>              /* Board info */


/* Base address of the controller's registers. */
#ifdef SPLC780D_4BIT_VADDR
#define SPLC780D_4BIT_REGBASE SPLC780D_4BIT_VADDR
#endif

/* 
The controller's registers are connected at word addresses on these boards.
Each byte-wide register appears as the least-significant-byte (LSB) of the 
word regardless of the endianness of the processor (so if using word accesses 
then endianness doesn't matter).
*/
#define SPLC780D_4BIT_REGSPACING 4
typedef unsigned splc780d_4bit_reg_t;

/* Include generic information shared by all boards that use this device. */
#include    <xtensa/lcd-splc780d-4bitmode.h>


/* Display limits of the LCD panel. */
#define DISPLAY_VISIBLE_LEN 16  /* length (chars) of visible window */

#endif /* _LCD_SPLC780D_4BIT_BOARD_H */

