/*******************************************************************************

Copyright (c) 2006-2007 by Tensilica Inc.  ALL RIGHTS RESERVED.
These coded instructions, statements, and computer programs are the
copyrighted works and confidential proprietary information of Tensilica Inc.
They may not be modified, copied, reproduced, distributed, or disclosed to
third parties in any manner, medium, or form, in whole or in part, without
the prior written consent of Tensilica Inc.
--------------------------------------------------------------------------------

lcd-SPLC780D.h  Generic definitions for Sunplus SPLC780D LCD Controller

This is used by board-support-packages with one or more LCD displays that use
a SPLC780D controller. A BSP provides a base address for each instance of an 
SPLC780D LCD controller on the board.

Note that LCD display operation is almost totally independent of the LCD 
display, depending almost entirely on the controller. However the display
may limit the number of characters of the controller's RAM buffer that are
actually visible at one time. The length of the display's visible window
is not specifified in this controller-specific header, but comes to the 
driver from the board-specific "display.h" header.

*******************************************************************************/

#ifndef _LCD_SPLC780D_H_
#define _LCD_SPLC780D_H_


/* Offsets to controller registers from base. */
#define SPLC780D_INST   0
#define SPLC780D_DATA   (SPLC780D_INST + SPLC780D_REGSPACING)

/*
Bit fields and their values in the instruction register.
These fields are NOT orthogonal - they overlap!
Thus only one field may be written at a time, determined by the 
most-significant 1 bit in the pattern (the field selector).
All less significant bits are part of the value of the selected field.
The fields and their values are grouped together to emphasize this format.
Field selector macro names end in '_' (implying something more needs
to be ORed) and the value macros are indented. The pattern written to a
bitfield is a bitwise OR of a field selector and one or more values, eg.
  (SPLC780D_INST_ON_ | SPLC780D_INST_ON_DISPLAY | SPLC780D_INST_ON_CURSOR)
A single bit field (eg. SPCL780D_INST_HOME) need not have a value.

NOTE: Controller requires a software delay after writing to the control
or data registers. For the data register it is 38us. For the control
register it is 38us for most bit fields, with the following exceptions:
    SPLC780D_FUNC_                               100us.
    SPLC780D_INST_CLEAR, SPLC780D_INST_HOME     1520us.
For more details and reset timing, see the SUNPLUS SPLC780D data sheet.
*/

#define SPLC780D_INST_CLEAR_            0x1 /* clear (blank) display) */

#define SPLC780D_INST_HOME_             0x2 /* home cursor and shift pos */

#define SPLC780D_INST_ENTRY_            0x4 /* combine *ENTRY_* flags below */
#define     SPLC780D_INST_ENTRY_SHIFT   0x1 /* display shift on entry / not */
#define     SPLC780D_INST_ENTRY_INCR    0x2 /* cursor incr / decr */
#define     SPLC780D_INST_ENTRY_DECR      0 /* cursor incr / decr */

#define SPLC780D_INST_ON_               0x8     /* combine *ON_* flags below */
#define     SPLC780D_INST_ON_DISPLAY    0x4 /* display on / off */
#define     SPLC780D_INST_ON_CURSOR     0x2 /* cursor  on / off */
#define     SPLC780D_INST_ON_BLINK      0x1 /* blink   on / off */

#define SPLC780D_INST_SHIFT_           0x10 /* combine *SHIFT_* flags below */
#define     SPLC780D_INST_SHIFT_DISP    0x8 /* shift display / move cursor */
#define     SPLC780D_INST_SHIFT_CURS      0 /* shift display / move cursor */
#define     SPLC780D_INST_SHIFT_RIGHT   0x4 /* shift right / left */
#define     SPLC780D_INST_SHIFT_LEFT      0 /* shift right / left */

#define SPLC780D_INST_FUNC_            0x20 /* combine *FUNC_* flags below */
#define     SPLC780D_INST_FUNC_8BIT    0x10 /* data length 8 bit / 4 bit */
#define     SPLC780D_INST_FUNC_4BIT       0 /* data length 8 bit / 4 bit */
#define     SPLC780D_INST_FUNC_2LINE   0x08 /* display lines 2 / 1 */
#define     SPLC780D_INST_FUNC_1LINE      0 /* display lines 2 / 1 */
#define     SPLC780D_INST_FUNC_F5x10   0x04 /* character font 5x10 / 5x8 */
#define     SPLC780D_INST_FUNC_F5x8       0 /* character font 5x10 / 5x8 */
                                            /* font must be 5x8 for 2 lines */
#define SPLC780D_INST_CGEN_            0x40 /* set char generator address */
#define     SPLC780D_INST_CGEN_ADDR    0x3F /*   to address in this field */
#define SPLC780D_INST_DRAM_            0x80 /* set display data RAM address */
#define     SPLC780D_INST_DRAM_ADDR    0x7F /*   to address in this field */
#define     SPLC780D_INST_DRAM_LINE2   0x40 /* address offset to line 2 */
/* Controller limits */
#define SPLC780D_RAMLEN_1LINE          0x50 /* length of line in RAM (1 line) */
#define SPLC780D_RAMLEN_2LINE          0x28 /* length of line in RAM (2 line) */


#ifndef __ASSEMBLER__

/* C interface to controller registers. */
struct splc780d_s {
    splc780d_reg_t  inst;       /* instruction register */
    splc780d_reg_t  data;       /* data register */
};

typedef volatile struct splc780d_s splc780d_t;

/*
Prototypes of high level driver functions.
*/

/*
Initialize the display with the FUNC_, ENTRY_ and ON_ fields as specified in 
terms of the values above. The splc780d_init_default() macro is an example.
*/
extern void splc780d_init(splc780d_t *lcd,
                          unsigned func, unsigned entry, unsigned on);

/*
Initialize the display to default mode: 8-bit interface, 2 line, 5x8 font, 
increment cursor on entry, display on (cursor and blinking off).
*/
#define splc780d_init_default(lcd) \
    splc780d_init( lcd, \
            SPLC780D_INST_FUNC_8BIT \
            | SPLC780D_INST_FUNC_2LINE \
            | SPLC780D_INST_FUNC_F5x8, \
            SPLC780D_INST_ENTRY_INCR, \
            SPLC780D_INST_ON_DISPLAY \
            )

/*
Write a single character at a given position (chars from left, starting at 0).
Wait long enough afterward for the controller to be ready for more input.
Positions beyond the end of the display are ignored.
*/
extern void splc780d_write_char(splc780d_t *lcd, unsigned pos, const char c);

/*
Write a string to the display starting at the left (position 0). 
Blank-pad to or truncate at the end of the display (overwrites any previous 
string so don't need to blank the display first).
Wait long enough after each char for the controller to be ready for more input.
*/
extern void splc780d_write_string(splc780d_t *lcd, const char *s);

/*
Blank (clear) the entire display.
Wait long enough afterward for the controller to be ready for more input.
*/
extern void splc780d_blank(splc780d_t *lcd);

#endif /* __ASSEMBLER__ */

#endif /* _LCD_SPLC780D_H_ */

