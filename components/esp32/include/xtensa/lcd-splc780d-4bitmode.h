/*******************************************************************************

Copyright (c) 2009-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
These coded instructions, statements, and computer programs are the
copyrighted works and confidential proprietary information of Tensilica Inc.
They may not be modified, copied, reproduced, distributed, or disclosed to
third parties in any manner, medium, or form, in whole or in part, without
the prior written consent of Tensilica Inc.
--------------------------------------------------------------------------------

lcd-SPLC780D-4bitmode.h  Generic definitions for Sunplus SPLC780D LCD Controller
operating in 4 bit mode.

This is used by board-support-packages with one or more LCD displays that use
a SPLC780D controller in 4 bit mode. A BSP provides a base address for each 
instance of an SPLC780D LCD controller on the board.

Note that LCD display operation is almost totally independent of the LCD 
display, depending almost entirely on the controller. However the display
may limit the number of characters of the controller's RAM buffer that are
actually visible at one time. The length of the display's visible window
is not specifified in this controller-specific header, but comes to the 
driver from the board-specific "display.h" header.

*******************************************************************************/

#ifndef _LCD_SPLC780D_4BIT_H_
#define _LCD_SPLC780D_4BIT_H_


/* Offsets to controller registers from base. */
#define SPLC780D_4BIT_INST   0
#define SPLC780D_4BIT_DATA   (SPLC780D_4BIT_INST + SPLC780D_4BIT_REGSPACING)


#define SPLC780D_4BIT_INST_INIT1             0xFF /* First command in 
                                                     init sequence */
#define SPLC780D_4BIT_INST_INIT2             0x30 /* Second command in 
                                                     init sequence, 
                                                     issued 3 times */
#define SPLC780D_4BIT_INST_INIT3             0x20 /* Third and last command 
                                                     in init sequence */
#define SPLC780D_4BIT_INST_CLEAR             0x01 /* clear (blank) display) */
#define SPLC780D_4BIT_INST_SET_MODE          0x28 /* Set LCD mode. Supported
                                                     setting is 4 bit data 
                                                     length, 2 lines, 5*8 */
#define SPLC780D_4BIT_INST_DSPLY_ON          0x0C /* Set Display ON */
#define SPLC780D_4BIT_INST_CRSR_INC          0x06 /* Set cursor moving direction
                                                     as increment */

#define SPLC780D_4BIT_LINET_ADDR             0x80 /* clear (blank) display) */
#define SPLC780D_4BIT_LINEB_ADDR             0xC0 /* clear (blank) display) */

#ifndef __ASSEMBLER__

/* C interface to controller registers. */
struct splc780d_4bit_s {
    splc780d_4bit_reg_t  inst;       /* instruction register */
    splc780d_4bit_reg_t  data;       /* data register */
};

typedef volatile struct splc780d_4bit_s splc780d_4bit_t;

/*
Prototypes of high level driver functions.
*/

/* Write an instruction byte to LCD, result in two back to back writes since the
 * LCD is hooked up in 4 bit mode*/
extern void lcd_write_inst_byte(splc780d_4bit_t *lcd, unsigned char inst);

/* Write a data byte to LCD, result in two back to back writes since the
 * LCD is hooked up in 4 bit mode*/
extern void lcd_write_data_byte(splc780d_4bit_t *lcd, unsigned char data);

/*
Initialize the display with default settings. 
*/
extern void splc780d_4bit_init_default(splc780d_4bit_t *lcd);

/*
Write a single character at a given position (chars from left, starting at 0).
Wait long enough afterward for the controller to be ready for more input.
Positions beyond the end of the display are ignored.
*/
extern void splc780d_4bit_write_char(splc780d_4bit_t *lcd, unsigned pos, const char c);

/*
Write a string to the display starting at the left (position 0). 
Blank-pad to or truncate at the end of the display (overwrites any previous 
string so don't need to blank the display first).
Wait long enough after each char for the controller to be ready for more input.
*/
extern void splc780d_4bit_write_string(splc780d_4bit_t *lcd, const char *s);

/*
Blank (clear) the entire display.
Wait long enough afterward for the controller to be ready for more input.
*/
extern void splc780d_4bit_blank(splc780d_4bit_t *lcd);

#endif /* __ASSEMBLER__ */

#endif /* _LCD_SPLC780D_4BIT_H_ */

