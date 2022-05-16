/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution and was contributed
    to the project by Technolution B.V. (www.technolution.nl,
    freertos-riscv@technolution.eu) under the terms of the FreeRTOS
    contributors license.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*-----------------------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V port.
 *----------------------------------------------------------------------*/

#include "sdkconfig.h"
#include <string.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#include "riscv/rvruntime-frames.h"
#include "riscv/riscv_interrupts.h"
#include "riscv/interrupt.h"
#include "esp_private/crosscore_int.h"
#include "esp_attr.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_debug_helpers.h"
#include "esp_log.h"
#include "FreeRTOS.h"       /* This pulls in portmacro.h */
#include "task.h"
#include "portmacro.h"
#include "port_systick.h"



/* ---------------------------------------------------- Variables ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

static const char *TAG = "cpu_start"; // [refactor-todo]: might be appropriate to change in the future, but

/**
 * @brief A variable is used to keep track of the critical section nesting.
 * @note This variable has to be stored as part of the task context and must be initialized to a non zero value
 *       to ensure interrupts don't inadvertently become unmasked before the scheduler starts.
 *       As it is stored as part of the task context it will automatically be set to 0 when the first task is started.
 */
static UBaseType_t uxCriticalNesting = 0;
static UBaseType_t uxSavedInterruptState = 0;
BaseType_t uxSchedulerRunning = 0;
UBaseType_t uxInterruptNesting = 0;
BaseType_t xPortSwitchFlag = 0;
__attribute__((aligned(16))) static StackType_t xIsrStack[configISR_STACK_SIZE];
StackType_t *xIsrStackTop = &xIsrStack[0] + (configISR_STACK_SIZE & (~((portPOINTER_SIZE_TYPE)portBYTE_ALIGNMENT_MASK)));



/* ------------------------------------------------ FreeRTOS Portable --------------------------------------------------
 * - Provides implementation for functions required by FreeRTOS
 * - Declared in portable.h
 * ------------------------------------------------------------------------------------------------------------------ */

// ----------------- Scheduler Start/End -------------------

extern void esprv_intc_int_set_threshold(int); // FIXME, this function is in ROM only
BaseType_t xPortStartScheduler(void)
{
    uxInterruptNesting = 0;
    uxCriticalNesting = 0;
    uxSchedulerRunning = 0;

    /* Setup the hardware to generate the tick. */
    vPortSetupTimer();

    esprv_intc_int_set_threshold(1); /* set global INTC masking level */
    riscv_global_interrupts_enable();

    vPortYield();

    /*Should not get here*/
    return pdFALSE;
}

void vPortEndScheduler(void)
{
    /* very unlikely this function will be called, so just trap here */
    abort();
}

// ------------------------ Stack --------------------------

static void prvTaskExitError(void)
{
    /* A function that implements a task must not exit or attempt to return to
    its caller as there is nothing to return to.  If a task wants to exit it
    should instead call vTaskDelete( NULL ).

    Artificially force an assert() to be triggered if configASSERT() is
    defined, then stop here so application writers can catch the error. */
    configASSERT(uxCriticalNesting == ~0UL);
    portDISABLE_INTERRUPTS();
    abort();
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
    extern uint32_t __global_pointer$;
    uint8_t *task_thread_local_start;
    uint8_t *threadptr;
    extern char _thread_local_start, _thread_local_end, _flash_rodata_start;

    /* Byte pointer, so that subsequent calculations don't depend on sizeof(StackType_t). */
    uint8_t *sp = (uint8_t *) pxTopOfStack;

    /* Set up TLS area.
     * The following diagram illustrates the layout of link-time and run-time
     * TLS sections.
     *
     *          +-------------+
     *          |Section:     |      Linker symbols:
     *          |.flash.rodata|      ---------------
     *       0x0+-------------+ <-- _flash_rodata_start
     *        ^ |             |
     *        | | Other data  |
     *        | |     ...     |
     *        | +-------------+ <-- _thread_local_start
     *        | |.tbss        | ^
     *        v |             | |
     *    0xNNNN|int example; | | (thread_local_size)
     *          |.tdata       | v
     *          +-------------+ <-- _thread_local_end
     *          | Other data  |
     *          |     ...     |
     *          |             |
     *          +-------------+
     *
     *                                Local variables of
     *                              pxPortInitialiseStack
     *                             -----------------------
     *          +-------------+ <-- pxTopOfStack
     *          |.tdata (*)   |  ^
     *        ^ |int example; |  |(thread_local_size
     *        | |             |  |
     *        | |.tbss (*)    |  v
     *        | +-------------+ <-- task_thread_local_start
     * 0xNNNN | |             |  ^
     *        | |             |  |
     *        | |             |  |_thread_local_start - _rodata_start
     *        | |             |  |
     *        | |             |  v
     *        v +-------------+ <-- threadptr
     *
     *   (*) The stack grows downward!
     */

    uint32_t thread_local_sz = (uint32_t) (&_thread_local_end - &_thread_local_start);
    thread_local_sz = ALIGNUP(0x10, thread_local_sz);
    sp -= thread_local_sz;
    task_thread_local_start = sp;
    memcpy(task_thread_local_start, &_thread_local_start, thread_local_sz);
    threadptr = task_thread_local_start - (&_thread_local_start - &_flash_rodata_start);

    /* Simulate the stack frame as it would be created by a context switch interrupt. */
    sp -= RV_STK_FRMSZ;
    RvExcFrame *frame = (RvExcFrame *)sp;
    memset(frame, 0, sizeof(*frame));
    frame->ra = (UBaseType_t)prvTaskExitError;
    frame->mepc = (UBaseType_t)pxCode;
    frame->a0 = (UBaseType_t)pvParameters;
    frame->gp = (UBaseType_t)&__global_pointer$;
    frame->tp = (UBaseType_t)threadptr;

    //TODO: IDF-2393
    return (StackType_t *)frame;
}



/* ---------------------------------------------- Port Implementations -------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

BaseType_t xPortInIsrContext(void)
{
    return uxInterruptNesting;
}

BaseType_t IRAM_ATTR xPortInterruptedFromISRContext(void)
{
    /* For single core, this can be the same as xPortInIsrContext() because reading it is atomic */
    return uxInterruptNesting;
}

// ---------------------- Spinlocks ------------------------



// ------------------ Critical Sections --------------------

void vPortEnterCritical(void)
{
    BaseType_t state = portSET_INTERRUPT_MASK_FROM_ISR();
    uxCriticalNesting++;

    if (uxCriticalNesting == 1) {
        uxSavedInterruptState = state;
    }
}

void vPortExitCritical(void)
{
    if (uxCriticalNesting > 0) {
        uxCriticalNesting--;
        if (uxCriticalNesting == 0) {
            portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptState);
        }
    }
}

// ---------------------- Yielding -------------------------

int vPortSetInterruptMask(void)
{
    int ret;
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    ret = REG_READ(INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    REG_WRITE(INTERRUPT_CORE0_CPU_INT_THRESH_REG, RVHAL_EXCM_LEVEL);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
    /**
     * In theory, this function should not return immediately as there is a
     * delay between the moment we mask the interrupt threshold register and
     * the moment a potential lower-priority interrupt is triggered (as said
     * above), it should have a delay of 2 machine cycles/instructions.
     *
     * However, in practice, this function has an epilogue of one instruction,
     * thus the instruction masking the interrupt threshold register is
     * followed by two instructions: `ret` and `csrrs` (RV_SET_CSR).
     * That's why we don't need any additional nop instructions here.
     */
    return ret;
}

void vPortClearInterruptMask(int mask)
{
    REG_WRITE(INTERRUPT_CORE0_CPU_INT_THRESH_REG, mask);
    /**
     * The delay between the moment we unmask the interrupt threshold register
     * and the moment the potential requested interrupt is triggered is not
     * null: up to three machine cycles/instructions can be executed.
     *
     * When compilation size optimization is enabled, this function and its
     * callers returning void will have NO epilogue, thus the instruction
     * following these calls will be executed.
     *
     * If the requested interrupt is a context switch to a higher priority
     * task then the one currently running, we MUST NOT execute any instruction
     * before the interrupt effectively happens.
     * In order to prevent this, force this routine to have a 3-instruction
     * delay before exiting.
     */
    asm volatile ( "nop" );
    asm volatile ( "nop" );
    asm volatile ( "nop" );
}

void vPortYield(void)
{
    if (uxInterruptNesting) {
        vPortYieldFromISR();
    } else {

        esp_crosscore_int_send_yield(0);
        /* There are 3-4 instructions of latency between triggering the software
           interrupt and the CPU interrupt happening. Make sure it happened before
           we return, otherwise vTaskDelay() may return and execute 1-2
           instructions before the delay actually happens.

           (We could use the WFI instruction here, but there is a chance that
           the interrupt will happen while evaluating the other two conditions
           for an instant yield, and if that happens then the WFI would be
           waiting for the next interrupt to occur...)
        */
        while (uxSchedulerRunning && uxCriticalNesting == 0 && REG_READ(SYSTEM_CPU_INTR_FROM_CPU_0_REG) != 0) {}
    }
}

void vPortYieldFromISR( void )
{
    traceISR_EXIT_TO_SCHEDULER();
    uxSchedulerRunning = 1;
    xPortSwitchFlag = 1;
}

void vPortYieldOtherCore(BaseType_t coreid)
{
    esp_crosscore_int_send_yield(coreid);
}

// ------------------- Hook Functions ----------------------

void __attribute__((weak)) vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
#define ERR_STR1 "***ERROR*** A stack overflow in task "
#define ERR_STR2 " has been detected."
    const char *str[] = {ERR_STR1, pcTaskName, ERR_STR2};

    char buf[sizeof(ERR_STR1) + CONFIG_FREERTOS_MAX_TASK_NAME_LEN + sizeof(ERR_STR2) + 1 /* null char */] = {0};

    char *dest = buf;
    for (int i = 0; i < sizeof(str) / sizeof(str[0]); i++) {
        dest = strcat(dest, str[i]);
    }
    esp_system_abort(buf);
}

// ----------------------- System --------------------------

uint32_t xPortGetTickRateHz(void)
{
    return (uint32_t)configTICK_RATE_HZ;
}

#define STACK_WATCH_AREA_SIZE 32
#define STACK_WATCH_POINT_NUMBER (SOC_CPU_WATCHPOINTS_NUM - 1)

void vPortSetStackWatchpoint(void *pxStackStart)
{
    uint32_t addr = (uint32_t)pxStackStart;
    addr = (addr + (STACK_WATCH_AREA_SIZE - 1)) & (~(STACK_WATCH_AREA_SIZE - 1));
    esp_cpu_set_watchpoint(STACK_WATCH_POINT_NUMBER, (char *)addr, STACK_WATCH_AREA_SIZE, ESP_WATCHPOINT_STORE);
}



/* ---------------------------------------------- Misc Implementations -------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- App Start-up ----------------------

/* [refactor-todo]: See if we can include this through a header */
extern void esp_startup_start_app_common(void);

void esp_startup_start_app(void)
{
    esp_startup_start_app_common();

    ESP_LOGI(TAG, "Starting scheduler.");
    vTaskStartScheduler();
}
