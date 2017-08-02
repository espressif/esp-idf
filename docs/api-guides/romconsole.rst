*****************
ESP32 ROM console
*****************

When an ESP32 is unable to boot from flash ROM (and the fuse disabling it hasn't been blown), it boots into a rom console. The console
is based on TinyBasic, and statements entered should be in the form of BASIC statements. As is common in the BASIC language, without a 
preceeding line number, commands entered are executed immediately; lines with a prefixed line number are stored as part of a program.

Full list of supported statements and functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

System
------

- BYE		- *exits Basic, reboots ESP32, retries booting from flash*
- END 		- *stops execution from the program, also "STOP"*
- MEM		- *displays memory usage statistics*
- NEW		- *clears the current program*
- RUN		- *executes the current program*

IO, Documentation
-----------------

- PEEK( address )	- *get a 32-bit value from a memory address*
- POKE			- *write a 32-bit value to memory*
- USR(addr, arg1, ..) - *Execute a machine language function*
- PRINT expression	- *print out the expression, also "?"*
- PHEX expression	- *print expression as a hex number*
- REM stuff		- *remark/comment, also "'"*

Expressions, Math
-----------------

- A=V, LET A=V	- *assign value to a variable*
- +, -, \*, / - *Math*
- <,<=,=,<>,!=,>=,> - *Comparisons*
- ABS( expression )  - *returns the absolute value of the expression*
- RSEED( v ) - *sets the random seed to v*
- RND( m ) - *returns a random number from 0 to m*
- A=1234 - * Assign a decimal value*
- A=&h1A2 - * Assign a hex value*
- A=&b1001 - *Assign a binary value*

Control
-------

- IF expression THEN statement - *perform statement if expression is true*
- FOR variable = start TO end	- *start for block*
- FOR variable = start TO end STEP value - *start for block with step*
- NEXT - *end of for block*
- GOTO linenumber - *continue execution at this line number*
- GOSUB linenumber - *call a subroutine at this line number*
- RETURN	- *return from a subroutine*
- DELAY		- *Delay a given number of milliseconds*

Pin IO
------
- IODIR		- *Set a GPIO-pin as an output (1) or input (0)*
- IOSET		- *Set a GPIO-pin, configured as output, to high (1) or low (0)*
- IOGET		- *Get the value of a GPIO-pin*


Example programs
~~~~~~~~~~~~~~~~

Here are a few example commands and programs to get you started...

Read UART_DATE register of uart0
--------------------------------

::

	> PHEX PEEK(&h3FF40078)
	15122500

Set GPIO2 using memory writes to GPIO_OUT_REG
---------------------------------------------

Note: you can do this easier with the IOSET command

::

	> POKE &h3FF44004,PEEK(&h3FF44004) OR &b100

Get value of GPIO0
------------------

::

	> IODIR 0,0
	> PRINT IOGET(0)
	0

Blink LED
---------

Hook up an LED between GPIO2 and ground. When running the program, the LED should blink 10 times.

::

	10 IODIR 2,1
	20 FOR A=1 TO 10
	30 IOSET 2,1
	40 DELAY 250
	50 IOSET 2,0
	60 DELAY 250
	70 NEXT A
	RUN

Credits
~~~~~~~

The ROM console is based on "TinyBasicPlus" by Mike Field and Scott Lawrence, which is based on "68000 TinyBasic" by Gordon Brandly

