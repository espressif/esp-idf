/*******************************************************************************

  Copyright (c) 2006-2009 Tensilica Inc.

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

xtbsp.h     Xtensa Board Support Package API

This API defines a minimal set of board-support functions that every supported
Xtensa board is expected to provide in the board-support-package (BSP) library
associated with the board-specific LSP. Only basic board functions are provided
in this board-independent API. API functions not applicable to a board must be
stubbed in its BSP library. More complex operations must use a board-specific
interface. Functions are grouped by type of peripheral device.

*******************************************************************************/

#ifndef _XTBSP_H_
#define _XTBSP_H_


#ifdef __cplusplus
extern  "C" {
#endif

  
/*******************************************************************************
BOARD INITIALIZATION.
The board with all its devices is initialized by xtbsp_board_init().
Individual devices represented by this API can be reinitialized at any 
time by calling their inidividual device init functions (grouped with 
other device functions). This might be useful to (say) change the baud 
rate of the UART.
*/


/*
Initialize the board. Must call before any other API function.
Iniitializes BSP, board in general, and all devices on the board.
*/
extern void xtbsp_board_init(void);


/*******************************************************************************
BOARD CHARACTERISTICS and CONVENIENCE FUNCTIONS.
Board support functions not associated with a particular peripheral device.
*/

/*
Return a short string representing the type of board.
If the board has a display, the string must fit on a single line.
*/
extern const char * xtbsp_board_name(void);

/*
Hardware reset the entire board (if possible). Does not return if successful.
If this function returns, it is stubbed out or not possible with this board.
*/
extern void xtbsp_board_reset(void);

/*
Return the clock frequency in Hertz. May be constant or computed.
*/
extern unsigned xtbsp_clock_freq_hz(void);

/*
Return the clock period in picoseconds. May be constant or computed.
*/
extern unsigned xtbsp_clock_period_ps(void);

/*
Spin (at least) a number of cycles per the processor's CCOUNT register.
Unlike a s/w delay loop, the duration is not affected by compiler
optimization or interrupts completed within the delay period.
If the processor doesn't have CCOUNT, a s/w delay loop is used to obtain 
a rough approximation of the cycle count.
*/
extern void xtbsp_delay_cycles(unsigned cycles);

/*
Spin at least a number of nanoseconds (approximate and err in the high side).
BSP implementation should do this efficiently, avoiding integer overflow or 
excessive loss of precision, run-time division or floating point. 
Function implementation (vs. macro) allows BSP to optimize for the clock 
frequency by pre-computing (or using constant) scale factors.
*/
extern void xtbsp_delay_ns(unsigned ns);


/*******************************************************************************
C LIBRARY SUPPORT.
These functions are called by the C library libgloss interface.
Their names are predetermined apart from this BSP API.
*/

/*
Initialize the board. Called by C library initialization code.
Usually simply calls xtbsp_board_init().
*/
extern void board_init(void);

/*
(Wait for and) Input a single byte from the default character I/O 
device. Return -1 if there is no input device. 
This device is usually a UART and this function calls xtbsp_uart_getchar().
On some boards (eg.) it might be a directly connected keyboard.
*/
extern int inbyte(void);

/*
Output a single char to the default character I/O device (and wait 
until it's been taken). 
This device is usually a UART and this function calls xtbsp_uart_putchar().
On some boards (eg.) it might be a directly connected bit-mapped screen.
*/
extern void outbyte(int c);


/*******************************************************************************
UART (SERIAL I/O).
Supports a single UART in a simple polling mode and provides control of
receiver and transmitter data interrupts (client must provide handler).
Provides a mapping to processor interrupt number which can be used with
the HAL to control processor interrupt enable (INTENABLE) etc.
*/

/* Bitmasks to identify UART interrupts. */
typedef enum xtbsp_uart_int {
    xtbsp_uart_int_rx = 1<<0,
    xtbsp_uart_int_tx = 1<<1,
    /* mask of all valid interrupt bits */
    xtbsp_uart_int_all = (1<<2)-1
} xtbsp_uart_int;

/*
Return non-zero if the board has a UART.
*/
extern int xtbsp_uart_exists(void);

/*
Initialize the UART:
  parity = 0 (none), 1 (odd), or 2 (even).
  nstop  = 1 or 2 (stop bits).
  ndata  = 7 or 8 (data bits).
Disables all UART interrupts.
Returns non-zero if failed (perhaps due to unsupported parameter values).
Must call before any of the following functions.
*/
extern int xtbsp_uart_init(unsigned baud, unsigned ndata, 
                           unsigned parity, unsigned nstop);
#define xtbsp_uart_init_default() xtbsp_uart_init(38400, 8, 0, 1)

/*
(Wait for and) Input a single char from the UART.
Any pending xtbsp_uart_int_rx interrupt is cleared.
*/
extern char xtbsp_uart_getchar(void);

/*
(Wait for transmitter ready and) Output a single char to the UART.
Any pending xtbsp_uart_int_tx interrupt is cleared.
*/
extern void xtbsp_uart_putchar(const char c);

/*
Return true (non-zero) if a character has been received and is ready
to be input by xtbsp_uart_getchar() without waiting, else return 0.
*/
extern int xtbsp_uart_get_isready(void);

/*
Return non-zero if a character may be output by xtbsp_uart_putchar() 
without waiting, else return 0.
Any pending xtbsp_uart_int_tx interrupt is cleared.
*/
extern int xtbsp_uart_put_isready(void);

/*
Return the enable status of all UART interrupts represented by this API,
that is those with bits defined in type xtbsp_uart_int (1 bit = enabled).
This is the enable status at the device, not the processor's INTENABLE.
*/
extern xtbsp_uart_int xtbsp_uart_int_enable_status(void);

/*
Enable selected UART interrupts at the device.
*/
extern void xtbsp_uart_int_enable(const xtbsp_uart_int mask);

/*
Disable selected UART interrupts at the device.
*/
extern void xtbsp_uart_int_disable(const xtbsp_uart_int mask);

/*
Return the interrupt number (0..31) to which the selected UART interrupt
is connected. May be used with the link-time HAL to obtain more information,
eg. Xthal_intlevel_mask[xtbsp_uart_int_number(xtbsp_uart_int_rx)]
This information can be used to control the processor's INTENABLE, etc.
Result is -1 if not connected, undefined if mask has more than 1 bit set.
*/
extern int xtbsp_uart_int_number(const xtbsp_uart_int mask);


/*******************************************************************************
DISPLAY.
Supports a single display that can render a series of ASCII characters.
Functions are provided to perform generic display tasks such as display
a string, display character by character, or blank the display.
Chars are 7-bit printable ASCII. Strings are C style NUL \0 terminated.
These functions busy-wait for any required timing delays so the caller does
not have to deal with timing. Some displays require long delays which in 
some client applications warrant a board and RTOS specific approach to 
driving the display, however that is beyond the scope of this API.
*/

/*
Return non-zero if board has a display.
*/
extern int xtbsp_display_exists(void);

/*
Initialize the display. Must call before any of the following functions.
*/
extern void xtbsp_display_init(void);

/*
Display a single char at position pos (0 is leftmost). Other positions are
left untouched. Positions beyond the width of the display are ignored.
*/
extern void xtbsp_display_char(unsigned pos, const char c);

/*
Display a string. Blank-pad to or truncate at the end of the display
(overwrites any previous string so don't need to blank display first).
*/
extern void xtbsp_display_string(const char *s);

/*
Blank (clear) the entire display.
*/
extern void xtbsp_display_blank(void);



#ifdef __cplusplus
}
#endif

#endif /* _XTBSP_H_ */
