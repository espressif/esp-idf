/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR uint8_t
#define portFLOAT float
#define portDOUBLE double
#define portLONG int32_t
#define portSHORT int16_t
#define portSTACK_TYPE uint8_t
#define portBASE_TYPE int
// interrupt module will mask interrupt with priority less than threshold
#define RVHAL_EXCM_LEVEL 4

typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef unsigned portBASE_TYPE UBaseType_t;

#if (configUSE_16_BIT_TICKS == 1)
typedef uint16_t TickType_t;
#define portMAX_DELAY (TickType_t)0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY (TickType_t)0xffffffffUL
#endif
/*------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH (-1)
#define portTICK_PERIOD_MS ((TickType_t)(1000 / configTICK_RATE_HZ))
#define portBYTE_ALIGNMENT 16
/*-----------------------------------------------------------*/

#define portCRITICAL_NESTING_IN_TCB 0

/*
 * Send an interrupt to another core in order to make the task running
 * on it yield for a higher-priority task.
 */
void vPortYieldOtherCore(BaseType_t coreid);

/*
 Callback to set a watchpoint on the end of the stack. Called every context switch to change the stack
 watchpoint around.
 */
void vPortSetStackWatchpoint(void *pxStackStart);

/*
 * Returns true if the current core is in ISR context; low prio ISR, med prio ISR or timer tick ISR. High prio ISRs
 * aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 */
BaseType_t xPortInIsrContext(void);

/*
 * This function will be called in High prio ISRs. Returns true if the current core was in ISR context
 * before calling into high prio ISR context.
 */
BaseType_t xPortInterruptedFromISRContext(void);

/* "mux" data structure (spinlock) */
typedef struct {
    /* owner field values:
     * 0                - Uninitialized (invalid)
     * portMUX_FREE_VAL - Mux is free, can be locked by either CPU
     * CORE_ID_REGVAL_PRO / CORE_ID_REGVAL_APP - Mux is locked to the particular core
     *
     *
     * Any value other than portMUX_FREE_VAL, CORE_ID_REGVAL_PRO, CORE_ID_REGVAL_APP indicates corruption
     */
    uint32_t owner;
    /* count field:
     * If mux is unlocked, count should be zero.
     * If mux is locked, count is non-zero & represents the number of recursive locks on the mux.
     */
    uint32_t count;
} portMUX_TYPE;

#define portMUX_FREE_VAL SPINLOCK_FREE

/* Special constants for vPortCPUAcquireMutexTimeout() */
#define portMUX_NO_TIMEOUT SPINLOCK_WAIT_FOREVER /* When passed for 'timeout_cycles', spin forever if necessary */
#define portMUX_TRY_LOCK SPINLOCK_NO_WAIT        /* Try to acquire the spinlock a single time only */

// Keep this in sync with the portMUX_TYPE struct definition please.
#define portMUX_INITIALIZER_UNLOCKED                                                                                   \
    { .owner = portMUX_FREE_VAL, .count = 0, }

/* Scheduler utilities. */
extern void vPortYield(void);
extern void vPortYieldFromISR(void);

#define portYIELD() vPortYield()
#define portYIELD_FROM_ISR() vPortYieldFromISR()

/* Yielding within an API call (when interrupts are off), means the yield should be delayed
   until interrupts are re-enabled.
   To do this, we use the "cross-core" interrupt as a trigger to yield on this core when interrupts are re-enabled.This
   is the same interrupt & code path which is used to trigger a yield between CPUs, although in this case the yield is
   happening on the same CPU.
*/
#define portYIELD_WITHIN_API() portYIELD()
/*-----------------------------------------------------------*/

/* Critical section management. */
extern int vPortSetInterruptMask(void);
extern void vPortClearInterruptMask(int);

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) void vFunction(void* pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) void vFunction(void* pvParameters)

void vApplicationSleep(TickType_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(idleTime) vApplicationSleep(idleTime)

#define portNOP() //__asm volatile  ( " nop " )

#define portVALID_TCB_MEM(ptr)   // esp_ptr_byte_accessible(ptr)
#define portVALID_STACK_MEM(ptr) // esp_ptr_byte_accessible(ptr)

#endif //__ASSEMBLER__

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
