/*******************************************************************************
Copyright (c) 2006-2015 Cadence Design Systems Inc.

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
******************************************************************************/

/******************************************************************************
  Xtensa-specific API for RTOS ports.
******************************************************************************/

#ifndef __XTENSA_API_H__
#define __XTENSA_API_H__

#include <stdbool.h>
#include <xtensa/hal.h>
#include "xtensa_context.h"


/* Typedef for C-callable interrupt handler function */
typedef void (*xt_handler)(void *);

/* Typedef for C-callable exception handler function */
typedef void (*xt_exc_handler)(XtExcFrame *);


/*
-------------------------------------------------------------------------------
  Call this function to set a handler for the specified exception. The handler
  will be installed on the core that calls this function.

    n        - Exception number (type)
    f        - Handler function address, NULL to uninstall handler.

  The handler will be passed a pointer to the exception frame, which is created
  on the stack of the thread that caused the exception.

  If the handler returns, the thread context will be restored and the faulting
  instruction will be retried. Any values in the exception frame that are
  modified by the handler will be restored as part of the context. For details
  of the exception frame structure see xtensa_context.h.
-------------------------------------------------------------------------------
*/
extern xt_exc_handler xt_set_exception_handler(int n, xt_exc_handler f);


/*
-------------------------------------------------------------------------------
  Call this function to set a handler for the specified interrupt. The handler
  will be installed on the core that calls this function.

    n        - Interrupt number.
    f        - Handler function address, NULL to uninstall handler.
    arg      - Argument to be passed to handler.
-------------------------------------------------------------------------------
*/
extern xt_handler xt_set_interrupt_handler(int n, xt_handler f, void * arg);


/*
-------------------------------------------------------------------------------
  Call this function to enable the specified interrupts on the core that runs
  this code.

    mask     - Bit mask of interrupts to be enabled.
-------------------------------------------------------------------------------
*/
extern void xt_ints_on(unsigned int mask);


/*
-------------------------------------------------------------------------------
  Call this function to disable the specified interrupts on the core that runs
  this code.

    mask     - Bit mask of interrupts to be disabled.
-------------------------------------------------------------------------------
*/
extern void xt_ints_off(unsigned int mask);


/*
-------------------------------------------------------------------------------
  Call this function to set the specified (s/w) interrupt.
-------------------------------------------------------------------------------
*/
static inline void xt_set_intset(unsigned int arg)
{
    xthal_set_intset(arg);
}


/*
-------------------------------------------------------------------------------
  Call this function to clear the specified (s/w or edge-triggered)
  interrupt.
-------------------------------------------------------------------------------
*/
static inline void xt_set_intclear(unsigned int arg)
{
    xthal_set_intclear(arg);
}

/*
-------------------------------------------------------------------------------
  Call this function to get handler's argument for the specified interrupt.

    n        - Interrupt number.
-------------------------------------------------------------------------------
*/
extern void * xt_get_interrupt_handler_arg(int n);

/*
-------------------------------------------------------------------------------
  Call this function to check if the specified interrupt is free to use.

    intr       - Interrupt number.
    cpu        - cpu number.
-------------------------------------------------------------------------------
*/
bool xt_int_has_handler(int intr, int cpu);

#endif /* __XTENSA_API_H__ */
