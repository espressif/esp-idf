/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef STACK_MACROS_H
#define STACK_MACROS_H

/*
 * Call the stack overflow hook function if the stack of the task being swapped
 * out is currently overflowed, or looks like it might have overflowed in the
 * past.
 *
 * Setting configCHECK_FOR_STACK_OVERFLOW to 1 will cause the macro to check
 * the current stack state only - comparing the current top of stack value to
 * the stack limit.  Setting configCHECK_FOR_STACK_OVERFLOW to greater than 1
 * will also cause the last few stack bytes to be checked to ensure the value
 * to which the bytes were set when the task was created have not been
 * overwritten.  Note this second test does not guarantee that an overflowed
 * stack will always be recognised.
 */

/*-----------------------------------------------------------*/

#if( configCHECK_FOR_STACK_OVERFLOW == 0 )

    /* FreeRTOSConfig.h is not set to check for stack overflows. */
    #define taskFIRST_CHECK_FOR_STACK_OVERFLOW()
    #define taskSECOND_CHECK_FOR_STACK_OVERFLOW()

#endif /* configCHECK_FOR_STACK_OVERFLOW == 0 */
/*-----------------------------------------------------------*/

#if( configCHECK_FOR_STACK_OVERFLOW == 1 )

    /* FreeRTOSConfig.h is only set to use the first method of
    overflow checking. */
    #define taskSECOND_CHECK_FOR_STACK_OVERFLOW()

#endif
/*-----------------------------------------------------------*/

#if( ( configCHECK_FOR_STACK_OVERFLOW > 0 ) && ( portSTACK_GROWTH < 0 ) )

/* Only the current stack state is to be checked. */
    #define taskFIRST_CHECK_FOR_STACK_OVERFLOW()                                                        \
    {                                                                                                 \
        /* Is the currently saved stack pointer within the stack limit? */                            \
        if( pxCurrentTCB[ xPortGetCoreID() ]->pxTopOfStack <= pxCurrentTCB[ xPortGetCoreID() ]->pxStack )                                       \
        {                                                                                             \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB[ xPortGetCoreID() ], pxCurrentTCB[ xPortGetCoreID() ]->pcTaskName );   \
        }                                                                                             \
    }

#endif /* configCHECK_FOR_STACK_OVERFLOW > 0 */
/*-----------------------------------------------------------*/

#if( ( configCHECK_FOR_STACK_OVERFLOW > 0 ) && ( portSTACK_GROWTH > 0 ) )

/* Only the current stack state is to be checked. */
    #define taskFIRST_CHECK_FOR_STACK_OVERFLOW()                                                        \
    {                                                                                                 \
                                                                                                      \
        /* Is the currently saved stack pointer within the stack limit? */                            \
        if( pxCurrentTCB[ xPortGetCoreID() ]->pxTopOfStack >= pxCurrentTCB[ xPortGetCoreID() ]->pxEndOfStack )                                  \
        {                                                                                             \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB[ xPortGetCoreID() ], pxCurrentTCB[ xPortGetCoreID() ]->pcTaskName );   \
        }                                                                                             \
    }

#endif /* configCHECK_FOR_STACK_OVERFLOW == 1 */
/*-----------------------------------------------------------*/

#if ( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) && ( portSTACK_GROWTH < 0 ) )

    #define taskSECOND_CHECK_FOR_STACK_OVERFLOW()                                                                                       \
    {                                                                                                 \
    static const uint8_t ucExpectedStackBytes[] = { tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,     \
                                                    tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,     \
                                                    tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,     \
                                                    tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,     \
                                                    tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE };   \
                                                                                                      \
                                                                                                                                        \
        /* Has the extremity of the task stack ever been written over? */                                                               \
        if( memcmp( ( void * ) pxCurrentTCB[ xPortGetCoreID() ]->pxStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) != 0 )          \
        {                                                                                             \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB[ xPortGetCoreID() ], pxCurrentTCB[ xPortGetCoreID() ]->pcTaskName );                                   \
        }                                                                                             \
    }

#endif /* #if( configCHECK_FOR_STACK_OVERFLOW > 1 ) */
/*-----------------------------------------------------------*/

#if ( ( configCHECK_FOR_STACK_OVERFLOW > 1 ) && ( portSTACK_GROWTH > 0 ) )

    #define taskSECOND_CHECK_FOR_STACK_OVERFLOW()                                                                                       \
    {                                                                                                                                     \
        int8_t *pcEndOfStack = ( int8_t * ) pxCurrentTCB[ xPortGetCoreID() ]->pxEndOfStack;                                                                     \
        static const uint8_t ucExpectedStackBytes[] = { tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE,   \
                                                        tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE, tskSTACK_FILL_BYTE }; \
                                                                                                                                          \
                                                                                                                                          \
        pcEndOfStack -= sizeof( ucExpectedStackBytes );                                                                                   \
                                                                                                                                          \
        /* Has the extremity of the task stack ever been written over? */                                                                 \
        if( memcmp( ( void * ) pcEndOfStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) != 0 )                     \
        {                                                                                                                                 \
            vApplicationStackOverflowHook( ( TaskHandle_t ) pxCurrentTCB[ xPortGetCoreID() ], pxCurrentTCB[ xPortGetCoreID() ]->pcTaskName );                                   \
        }                                                                                                                                 \
    }

#endif /* #if( configCHECK_FOR_STACK_OVERFLOW > 1 ) */

#endif /* STACK_MACROS_H */
