/*
 * FreeRTOS Kernel V10.5.1 (ESP-IDF SMP modified)
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-FileCopyrightText: 2021 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2023-2025 Espressif Systems (Shanghai) CO LTD
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

/*-----------------------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V port.
 *----------------------------------------------------------------------*/

#include "sdkconfig.h"
#include <string.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#include "soc/interrupt_reg.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#include "riscv/rvruntime-frames.h"
#include "riscv/rv_utils.h"
#include "riscv/interrupt.h"
#include "esp_private/crosscore_int.h"
#include "hal/crosscore_int_ll.h"
#include "esp_attr.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "FreeRTOS.h"       /* This pulls in portmacro.h */
#include "task.h"
#include "portmacro.h"
#include "port_systick.h"
#include "esp_memory_utils.h"

#if SOC_CPU_HAS_HWLOOP
#include "riscv/csr.h"
#include "riscv/csr_hwlp.h"
#endif

#if ( SOC_CPU_COPROC_NUM > 0 )

#include "esp_private/panic_internal.h"

/* Since `portFORCE_INLINE` is not defined in `portmacro.h`, we must define it here since it is
 * used by `atomic.h`. */
#define portFORCE_INLINE    inline
#include "freertos/atomic.h"

#endif // ( SOC_CPU_COPROC_NUM > 0 )


_Static_assert(portBYTE_ALIGNMENT == 16, "portBYTE_ALIGNMENT must be set to 16");
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
/**
 * offsetof() can not be used in asm code. Then we need make sure that
 * PORT_OFFSET_PX_STACK and PORT_OFFSET_PX_END_OF_STACK have expected values.
 * Macro used in the portasm.S instead of variables to save at least 4 instruction calls
 * which accessing DRAM memory. This optimization saves CPU time in the interrupt handling.
 */

_Static_assert(offsetof( StaticTask_t, pxDummy6 ) == PORT_OFFSET_PX_STACK);
_Static_assert(offsetof( StaticTask_t, pxDummy8 ) == PORT_OFFSET_PX_END_OF_STACK);
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD

/* ---------------------------------------------------- Variables ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

volatile UBaseType_t port_xSchedulerRunning[portNUM_PROCESSORS] = {0}; // Indicates whether scheduler is running on a per-core basis
volatile UBaseType_t port_uxInterruptNesting[portNUM_PROCESSORS] = {0};  // Interrupt nesting level. Increased/decreased in portasm.c
volatile UBaseType_t port_uxCriticalNesting[portNUM_PROCESSORS] = {0};
volatile UBaseType_t port_uxOldInterruptState[portNUM_PROCESSORS] = {0};
volatile UBaseType_t xPortSwitchFlag[portNUM_PROCESSORS] = {0};

#if ( SOC_CPU_COPROC_NUM > 0 )

/* Current owner of the coprocessors for each core */
StaticTask_t* port_uxCoprocOwner[portNUM_PROCESSORS][SOC_CPU_COPROC_NUM];

#endif /* SOC_CPU_COPROC_NUM > 0 */

/*
*******************************************************************************
* Interrupt stack. The size of the interrupt stack is determined by the config
* parameter "configISR_STACK_SIZE" in FreeRTOSConfig.h
*******************************************************************************
*/
__attribute__((aligned(16))) StackType_t xIsrStack[portNUM_PROCESSORS][configISR_STACK_SIZE];
StackType_t *xIsrStackTop[portNUM_PROCESSORS] = {0};

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
StackType_t *xIsrStackBottom[portNUM_PROCESSORS] = {0};
#endif

/* ------------------------------------------------ FreeRTOS Portable --------------------------------------------------
 * - Provides implementation for functions required by FreeRTOS
 * - Declared in portable.h
 * ------------------------------------------------------------------------------------------------------------------ */

// ----------------- Scheduler Start/End -------------------

BaseType_t xPortStartScheduler(void)
{
#if ( SOC_CPU_COPROC_NUM > 0 )

#if SOC_CPU_HAS_FPU
    /* Disable FPU so that the first task to use it will trigger an exception */
    rv_utils_disable_fpu();
#endif /* SOC_CPU_HAS_FPU */

#if SOC_CPU_HAS_PIE
    /* Similarly, disable PIE */
    rv_utils_disable_pie();
#endif /* SOC_CPU_HAS_FPU */

#if SOC_CPU_HAS_HWLOOP
    /* Initialize the Hardware loop feature */
    RV_WRITE_CSR(CSR_HWLP_STATE_REG, HWLP_INITIAL_STATE);
#endif /* SOC_CPU_HAS_HWLOOP */
#endif /* ( SOC_CPU_COPROC_NUM > 0 ) */

    /* Initialize all kernel state tracking variables */
    BaseType_t coreID = xPortGetCoreID();
    port_uxInterruptNesting[coreID] = 0;
    port_uxCriticalNesting[coreID] = 0;
    port_xSchedulerRunning[coreID] = 0;

    /* Initialize ISR Stack(s) */
    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        xIsrStackTop[i] = &xIsrStack[i][0] + (configISR_STACK_SIZE & (~((portPOINTER_SIZE_TYPE)portBYTE_ALIGNMENT_MASK)));
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
        xIsrStackBottom[i] = &xIsrStack[i][0];
#endif
    }

    /* Setup the hardware to generate the tick. */
    vPortSetupTimer();

    esprv_int_set_threshold(RVHAL_INTR_ENABLE_THRESH); /* set global interrupt masking level */
    rv_utils_intr_global_enable();

    vPortYield();

    /* Should not get here */
    return pdTRUE;
}

void vPortEndScheduler(void)
{
    /* very unlikely this function will be called, so just trap here */
    abort();
}

// ------------------------ Stack --------------------------

/**
 * @brief Align stack pointer in a downward growing stack
 *
 * This macro is used to round a stack pointer downwards to the nearest n-byte boundary, where n is a power of 2.
 * This macro is generally used when allocating aligned areas on a downward growing stack.
 */
#define STACKPTR_ALIGN_DOWN(n, ptr)     ((ptr) & (~((n)-1)))

/**
 * @brief Allocate and initialize GCC TLS area
 *
 * This function allocates and initializes the area on the stack used to store GCC TLS (Thread Local Storage) variables.
 * - The area's size is derived from the TLS section's linker variables, and rounded up to a multiple of 16 bytes
 * - The allocated area is aligned to a 16-byte aligned address
 * - The TLS variables in the area are then initialized
 *
 * Each task access the TLS variables using the THREADPTR register plus an offset to obtain the address of the variable.
 * The value for the THREADPTR register is also calculated by this function, and that value should be use to initialize
 * the THREADPTR register.
 *
 * @param[in] uxStackPointer Current stack pointer address
 * @param[out] ret_threadptr_reg_init Calculated THREADPTR register initialization value
 * @return Stack pointer that points to the TLS area
 */
FORCE_INLINE_ATTR UBaseType_t uxInitialiseStackTLS(UBaseType_t uxStackPointer, uint32_t *ret_threadptr_reg_init)
{
    /*
    LOW ADDRESS
            |---------------------------|   Linker Symbols
            | Section                   |   --------------
            | .flash.tdata              |
         0x0|---------------------------| <- _thread_local_data_start  ^
            | .flash.tdata              |                              |
            | int var_1 = 1;            |                              |
            |                           | <- _thread_local_data_end    |
            |                           | <- _thread_local_bss_start   | tls_area_size
            |                           |                              |
            | .flash.tbss (NOLOAD)      |                              |
            | int var_2;                |                              |
            |---------------------------| <- _thread_local_bss_end     V
            | Other data                |
            | ...                       |
            |---------------------------|
    HIGH ADDRESS
    */
    // Calculate TLS area size and round up to multiple of 16 bytes.
    extern char _thread_local_data_start, _thread_local_data_end;
    extern char _thread_local_bss_start, _thread_local_bss_end;
    const uint32_t tls_data_size = (uint32_t)&_thread_local_data_end - (uint32_t)&_thread_local_data_start;
    const uint32_t tls_bss_size = (uint32_t)&_thread_local_bss_end - (uint32_t)&_thread_local_bss_start;
    const uint32_t tls_area_size = ALIGNUP(16, tls_data_size + tls_bss_size);
    // TODO: check that TLS area fits the stack

    // Allocate space for the TLS area on the stack. The area must be aligned to 16-bytes
    uxStackPointer = STACKPTR_ALIGN_DOWN(16, uxStackPointer - (UBaseType_t)tls_area_size);
    // Initialize the TLS data with the initialization values of each TLS variable
    memcpy((void *)uxStackPointer, &_thread_local_data_start, tls_data_size);
    // Initialize the TLS bss with zeroes
    memset((void *)(uxStackPointer + tls_data_size), 0, tls_bss_size);

    // Save tls start address
    *ret_threadptr_reg_init = (uint32_t)uxStackPointer;
    return uxStackPointer;
}

#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
static void vPortTaskWrapper(TaskFunction_t pxCode, void *pvParameters)
{
    __asm__ volatile(".cfi_undefined ra");  // tell to debugger that it's outermost (initial) frame
    extern void __attribute__((noreturn)) panic_abort(const char *details);
    static char DRAM_ATTR msg[80] = "FreeRTOS: FreeRTOS Task \"\0";
    pxCode(pvParameters);
    /* FreeRTOS tasks should not return. Log the task name and abort. */
    /* We cannot use s(n)printf because it is in flash */
    strcat(msg, pcTaskGetName(NULL));
    strcat(msg, "\" should not return, Aborting now!");
    panic_abort(msg);
}
#endif // CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER


#if ( SOC_CPU_COPROC_NUM > 0 )

/**
 * @brief Retrieve or allocate coprocessors save area from the given pxTopOfStack address.
 *
 * @param pxTopOfStack End of the stack address. This represents the highest address of a Task's stack.
 */
FORCE_INLINE_ATTR RvCoprocSaveArea* pxRetrieveCoprocSaveAreaFromStackPointer(UBaseType_t pxTopOfStack)
{
    return (RvCoprocSaveArea*) STACKPTR_ALIGN_DOWN(16, pxTopOfStack - sizeof(RvCoprocSaveArea));
}

/**
 * @brief Allocate and initialize the coprocessors save area on the stack
 *
 * @param[in] uxStackPointer Current stack pointer address
 *
 * @return Stack pointer that points to allocated and initialized the coprocessor save area
 */
FORCE_INLINE_ATTR UBaseType_t uxInitialiseCoprocSaveArea(UBaseType_t uxStackPointer)
{
    RvCoprocSaveArea* sa = pxRetrieveCoprocSaveAreaFromStackPointer(uxStackPointer);
    memset(sa, 0, sizeof(RvCoprocSaveArea));
    return (UBaseType_t) sa;
}


static void vPortCleanUpCoprocArea(void *pvTCB)
{
    StaticTask_t* task = (StaticTask_t*) pvTCB;

    /* Get a pointer to the task's coprocessor save area */
    const UBaseType_t bottomstack = (UBaseType_t) task->pxDummy8;
    RvCoprocSaveArea* sa = pxRetrieveCoprocSaveAreaFromStackPointer(bottomstack);

    /* If the Task ever saved the original stack pointer, restore it before returning */
    if (sa->sa_allocator != 0) {
        /* Restore the original lowest address of the stack in the TCB */
        task->pxDummy6 = sa->sa_tcbstack;

        /* Get the core the task is pinned on */
        #if ( configNUM_CORES > 1 )
            const BaseType_t coreID = task->xDummyCoreID;
            /* If the task is not pinned on any core, it didn't use any coprocessor than need to be freed (FPU or PIE).
             * If it used the HWLP coprocessor, it has nothing to clear since there is no "owner" for it. */
            if (coreID == tskNO_AFFINITY) {
                return;
            }
        #else /* configNUM_CORES > 1 */
            const BaseType_t coreID = 0;
        #endif /* configNUM_CORES > 1 */

        for (int i = 0; i < SOC_CPU_COPROC_NUM; i++) {
            StaticTask_t** owner = &port_uxCoprocOwner[coreID][i];
            /* If the owner is `task`, replace it with NULL atomically */
            Atomic_CompareAndSwapPointers_p32((void**) owner, NULL, task);
        }
    }
}
#endif /* SOC_CPU_COPROC_NUM > 0 */


/**
 * @brief Initialize the task's starting interrupt stack frame
 *
 * This function initializes the task's starting interrupt stack frame. The dispatcher will use this stack frame in a
 * context restore routine. Therefore, the starting stack frame must be initialized as if the task was interrupted right
 * before its first instruction is called.
 *
 * - The stack frame is allocated to a 16-byte aligned address
 *
 * @param[in] uxStackPointer Current stack pointer address
 * @param[in] pxCode Task function
 * @param[in] pvParameters Task function's parameter
 * @param[in] threadptr_reg_init THREADPTR register initialization value
 * @return Stack pointer that points to the stack frame
 */
FORCE_INLINE_ATTR UBaseType_t uxInitialiseStackFrame(UBaseType_t uxStackPointer, TaskFunction_t pxCode, void *pvParameters, uint32_t threadptr_reg_init)
{
    /*
    Allocate space for the task's starting interrupt stack frame.
    - The stack frame must be allocated to a 16-byte aligned address.
    - We use RV_STK_FRMSZ as it rounds up the total size to a multiple of 16.
    */
    uxStackPointer = STACKPTR_ALIGN_DOWN(16, uxStackPointer - RV_STK_FRMSZ);

    // Clear the entire interrupt stack frame
    RvExcFrame *frame = (RvExcFrame *)uxStackPointer;
    memset(frame, 0, sizeof(RvExcFrame));

    /* Initialize the stack frame. */
    extern uint32_t __global_pointer$;
    #if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
        frame->mepc = (UBaseType_t)vPortTaskWrapper;
        frame->a0 = (UBaseType_t)pxCode;
        frame->a1 = (UBaseType_t)pvParameters;
    #else
        frame->mepc = (UBaseType_t)pxCode;
        frame->a0 = (UBaseType_t)pvParameters;
    #endif // CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
    frame->gp = (UBaseType_t)&__global_pointer$;
    frame->tp = (UBaseType_t)threadptr_reg_init;

    return uxStackPointer;
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
#ifdef __clang_analyzer__
    // Teach clang-tidy that pxTopOfStack cannot be a pointer to const
    volatile StackType_t * pxTemp = pxTopOfStack;
    pxTopOfStack = pxTemp;
#endif /*__clang_analyzer__ */
    /*
    HIGH ADDRESS
    |---------------------------| <- pxTopOfStack on entry
    | TLS Variables             |
    | ------------------------- | <- Start of usable stack
    | Starting stack frame      |
    | ------------------------- | <- pxTopOfStack on return (which is the tasks current SP)
    |             |             |
    |             |             |
    |             V             |
    ----------------------------- <- Bottom of stack
    LOW ADDRESS

    - All stack areas are aligned to 16 byte boundary
    - We use UBaseType_t for all of stack area initialization functions for more convenient pointer arithmetic

    In the case of targets that have coprocessors, the stack is presented as follows:
    HIGH ADDRESS
    |---------------------------| <- pxTopOfStack on entry
    | Coproc. Save Area         | <- RvCoprocSaveArea
    | ------------------------- |
    | TLS Variables             |
    | ------------------------- | <- Start of usable stack
    | Starting stack frame      |
    | ------------------------- | <- pxTopOfStack on return (which is the tasks current SP)
    |             |             |
    |             |             |
    |             V             |
    |---------------------------|
    | Coproc. m Saved Context   | <- Coprocessor context save area after allocation
    |---------------------------|
    | Coproc. n Saved Context   | <- Another coprocessor context save area after allocation
    ----------------------------- <- Bottom of stack
    LOW ADDRESS

    Where m != n, n < SOC_CPU_COPROC_NUM, m < SOC_CPU_COPROC_NUM

    */

    UBaseType_t uxStackPointer = (UBaseType_t)pxTopOfStack;
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);

#if ( SOC_CPU_COPROC_NUM > 0 )
    // Initialize the coprocessors save area
    uxStackPointer = uxInitialiseCoprocSaveArea(uxStackPointer);
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);
#endif // SOC_CPU_COPROC_NUM > 0

    // Initialize GCC TLS area
    uint32_t threadptr_reg_init;
    uxStackPointer = uxInitialiseStackTLS(uxStackPointer, &threadptr_reg_init);
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);

    // Initialize the starting interrupt stack frame
    uxStackPointer = uxInitialiseStackFrame(uxStackPointer, pxCode, pvParameters, threadptr_reg_init);
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);

    // Return the task's current stack pointer address which should point to the starting interrupt stack frame
    return (StackType_t *)uxStackPointer;
}



/* ---------------------------------------------- Port Implementations -------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

BaseType_t xPortInIsrContext(void)
{
#if (configNUM_CORES > 1)
    unsigned int irqStatus;
    BaseType_t ret;

    /* Disable interrupts to fetch the coreID atomically */
    irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();

    /* Return the interrupt nesting counter for this core */
    ret = port_uxInterruptNesting[xPortGetCoreID()];

    /* Restore interrupts */
    portCLEAR_INTERRUPT_MASK_FROM_ISR(irqStatus);

    return ret;
#else
    /* Optimize the call for single-core targets */
    return port_uxInterruptNesting[0];
#endif /* (configNUM_CORES > 1) */
}

void vPortAssertIfInISR(void)
{
    /* Assert if the interrupt nesting count is > 0 */
    configASSERT(xPortInIsrContext() == 0);
}

BaseType_t IRAM_ATTR xPortInterruptedFromISRContext(void)
{
    /* Return the interrupt nesting counter for this core */
    return port_uxInterruptNesting[xPortGetCoreID()];
}

UBaseType_t xPortSetInterruptMaskFromISR(void)
{
    UBaseType_t prev_int_level = 0, int_level = 0;
#if !SOC_INT_CLIC_SUPPORTED
    int_level = RVHAL_EXCM_LEVEL;
#else
    int_level = RVHAL_EXCM_LEVEL_CLIC;
#endif

    prev_int_level = rv_utils_set_intlevel_regval(int_level);
    return prev_int_level;
}

void vPortClearInterruptMaskFromISR(UBaseType_t prev_int_level)
{
    rv_utils_restore_intlevel_regval(prev_int_level);
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

// ------------------ Critical Sections --------------------

#if (configNUM_CORES > 1)
BaseType_t __attribute__((optimize("-O3"))) xPortEnterCriticalTimeout(portMUX_TYPE *mux, BaseType_t timeout)
{
    /* Interrupts may already be disabled (if this function is called in nested
     * manner). However, there's no atomic operation that will allow us to check,
     * thus we have to disable interrupts again anyways.
     *
     * However, if this is call is NOT nested (i.e., the first call to enter a
     * critical section), we will save the previous interrupt level so that the
     * saved level can be restored on the last call to exit the critical.
     */
    BaseType_t xOldInterruptLevel = portSET_INTERRUPT_MASK_FROM_ISR();
    if (!spinlock_acquire(mux, timeout)) {
        //Timed out attempting to get spinlock. Restore previous interrupt level and return
        portCLEAR_INTERRUPT_MASK_FROM_ISR(xOldInterruptLevel);
        return pdFAIL;
    }
    //Spinlock acquired. Increment the critical nesting count.
    BaseType_t coreID = xPortGetCoreID();
    BaseType_t newNesting = port_uxCriticalNesting[coreID] + 1;
    port_uxCriticalNesting[coreID] = newNesting;
    //If this is the first entry to a critical section. Save the old interrupt level.
    if ( newNesting == 1 ) {
        port_uxOldInterruptState[coreID] = xOldInterruptLevel;
    }
    return pdPASS;
}

void __attribute__((optimize("-O3"))) vPortExitCriticalMultiCore(portMUX_TYPE *mux)
{
    /* This function may be called in a nested manner. Therefore, we only need
     * to re-enable interrupts if this is the last call to exit the critical. We
     * can use the nesting count to determine whether this is the last exit call.
     */
    spinlock_release(mux);
    BaseType_t coreID = xPortGetCoreID();
    BaseType_t nesting = port_uxCriticalNesting[coreID];

    /* Critical section nesting count must never be negative */
    configASSERT( nesting > 0 );

    if (nesting > 0) {
        nesting--;
        port_uxCriticalNesting[coreID] = nesting;

        //This is the last exit call, restore the saved interrupt level
        if ( nesting == 0 ) {
            portCLEAR_INTERRUPT_MASK_FROM_ISR(port_uxOldInterruptState[coreID]);
        }
    }
}


BaseType_t xPortEnterCriticalTimeoutCompliance(portMUX_TYPE *mux, BaseType_t timeout)
{
    BaseType_t ret;
    if (!xPortInIsrContext()) {
        ret = xPortEnterCriticalTimeout(mux, timeout);
    } else {
        esp_rom_printf("port*_CRITICAL called from ISR context. Aborting!\n");
        abort();
        ret = pdFAIL;
    }
    return ret;
}

void vPortExitCriticalCompliance(portMUX_TYPE *mux)
{
    if (!xPortInIsrContext()) {
        vPortExitCriticalMultiCore(mux);
    } else {
        esp_rom_printf("port*_CRITICAL called from ISR context. Aborting!\n");
        abort();
    }
}
#endif /* (configNUM_CORES > 1) */

void vPortEnterCritical(void)
{
#if (configNUM_CORES > 1)
        esp_rom_printf("vPortEnterCritical(void) is not supported on single-core targets. Please use vPortEnterCriticalMultiCore(portMUX_TYPE *mux) instead.\n");
        abort();
#endif /* (configNUM_CORES > 1) */
    BaseType_t state = portSET_INTERRUPT_MASK_FROM_ISR();
    port_uxCriticalNesting[0]++;

    if (port_uxCriticalNesting[0] == 1) {
        port_uxOldInterruptState[0] = state;
    }
}

void vPortExitCritical(void)
{
#if (configNUM_CORES > 1)
        esp_rom_printf("vPortExitCritical(void) is not supported on single-core targets. Please use vPortExitCriticalMultiCore(portMUX_TYPE *mux) instead.\n");
        abort();
#endif /* (configNUM_CORES > 1) */

    /* Critical section nesting count must never be negative */
    configASSERT( port_uxCriticalNesting[0] > 0 );

    if (port_uxCriticalNesting[0] > 0) {
        port_uxCriticalNesting[0]--;

        if (port_uxCriticalNesting[0] == 0) {
            portCLEAR_INTERRUPT_MASK_FROM_ISR(port_uxOldInterruptState[0]);
        }
    }
}

// ---------------------- Yielding -------------------------

void vPortYield(void)
{
    BaseType_t coreID = xPortGetCoreID();

    if (port_uxInterruptNesting[coreID]) {
        vPortYieldFromISR();
    } else {
        esp_crosscore_int_send_yield(coreID);
        /* There are 3-4 instructions of latency between triggering the software
           interrupt and the CPU interrupt happening. Make sure it happened before
           we return, otherwise vTaskDelay() may return and execute 1-2
           instructions before the delay actually happens.

           (We could use the WFI instruction here, but there is a chance that
           the interrupt will happen while evaluating the other two conditions
           for an instant yield, and if that happens then the WFI would be
           waiting for the next interrupt to occur...)
        */
        while (port_xSchedulerRunning[coreID] && port_uxCriticalNesting[coreID] == 0 && crosscore_int_ll_get_state(coreID) != 0) {}
    }
}

void vPortYieldFromISR( void )
{
    traceISR_EXIT_TO_SCHEDULER();
    BaseType_t coreID = xPortGetCoreID();
    port_xSchedulerRunning[coreID] = 1;
    xPortSwitchFlag[coreID] = 1;
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
    esp_cpu_set_watchpoint(STACK_WATCH_POINT_NUMBER, (char *)addr, STACK_WATCH_AREA_SIZE, ESP_CPU_WATCHPOINT_STORE);
}

// --------------------- TCB Cleanup -----------------------

#if ( CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS )
static void vPortTLSPointersDelCb( void *pxTCB )
{
    /* Typecast pxTCB to StaticTask_t type to access TCB struct members.
     * pvDummy15 corresponds to pvThreadLocalStoragePointers member of the TCB.
     */
    StaticTask_t *tcb = ( StaticTask_t * )pxTCB;

    /* The TLSP deletion callbacks are stored at an offset of (configNUM_THREAD_LOCAL_STORAGE_POINTERS/2) */
    TlsDeleteCallbackFunction_t *pvThreadLocalStoragePointersDelCallback = ( TlsDeleteCallbackFunction_t * )( &( tcb->pvDummy15[ ( configNUM_THREAD_LOCAL_STORAGE_POINTERS / 2 ) ] ) );

    /* We need to iterate over half the depth of the pvThreadLocalStoragePointers area
     * to access all TLS pointers and their respective TLS deletion callbacks.
     */
    for ( int x = 0; x < ( configNUM_THREAD_LOCAL_STORAGE_POINTERS / 2 ); x++ ) {
        if ( pvThreadLocalStoragePointersDelCallback[ x ] != NULL ) {  //If del cb is set
            /* In case the TLSP deletion callback has been overwritten by a TLS pointer, gracefully abort. */
            if ( !esp_ptr_executable( pvThreadLocalStoragePointersDelCallback[ x ] ) ) {
                ESP_LOGE("FreeRTOS", "Fatal error: TLSP deletion callback at index %d overwritten with non-excutable pointer %p", x, pvThreadLocalStoragePointersDelCallback[ x ]);
                abort();
            }

            pvThreadLocalStoragePointersDelCallback[ x ]( x, tcb->pvDummy15[ x ] );   //Call del cb
        }
    }
}
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

void vPortTCBPreDeleteHook( void *pxTCB )
{
    #if ( CONFIG_FREERTOS_TASK_PRE_DELETION_HOOK )
        /* Call the user defined task pre-deletion hook */
        extern void vTaskPreDeletionHook( void * pxTCB );
        vTaskPreDeletionHook( pxTCB );
    #endif /* CONFIG_FREERTOS_TASK_PRE_DELETION_HOOK */

    #if ( CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP )
        /*
         * If the user is using the legacy task pre-deletion hook, call it.
         * Todo: Will be removed in IDF-8097
         */
        #warning "CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP is deprecated. Use CONFIG_FREERTOS_TASK_PRE_DELETION_HOOK instead."
        extern void vPortCleanUpTCB( void * pxTCB );
        vPortCleanUpTCB( pxTCB );
    #endif /* CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP */

    #if ( CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS )
        /* Call TLS pointers deletion callbacks */
        vPortTLSPointersDelCb( pxTCB );
    #endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

    #if ( SOC_CPU_COPROC_NUM > 0 )
        /* Cleanup coproc save area */
        vPortCleanUpCoprocArea( pxTCB );
    #endif /* SOC_CPU_COPROC_NUM > 0 */
}


#if ( SOC_CPU_COPROC_NUM > 0 )

// ----------------------- Coprocessors --------------------------

/**
 * @brief Pin the given task to the given core
 *
 * This function is called when a task uses a coprocessor. Since the coprocessors registers
 * are saved lazily, as soon as a task starts using one, it must always be scheduled on the core
 * it is currently executing on.
 */
#if ( configNUM_CORES > 1 )
void vPortTaskPinToCore(StaticTask_t* task, int coreid)
{
    task->xDummyCoreID = coreid;
}
#endif /* configNUM_CORES > 1 */


/**
 * @brief Function to call to simulate an `abort()` occurring in a different context than the one it's called from.
 */
extern void xt_unhandled_exception(void *frame);

/**
 * @brief Get coprocessor save area out of the given task. If the coprocessor area is not created,
 *        it shall be allocated.
 *
 * @param task Task to get the coprocessor save area of
 * @param allocate When true, memory will be allocated for the coprocessor if it hasn't been allocated yet.
 *                 When false, the coprocessor memory will be left as NULL if not allocated.
 * @param coproc Coprocessor number to allocate memory for
 */
RvCoprocSaveArea* pxPortGetCoprocArea(StaticTask_t* task, bool allocate, int coproc)
{
    assert(coproc < SOC_CPU_COPROC_NUM);

    const UBaseType_t bottomstack = (UBaseType_t) task->pxDummy8;
    RvCoprocSaveArea* sa = pxRetrieveCoprocSaveAreaFromStackPointer(bottomstack);
    /* Check if the allocator is NULL. Since we don't have a way to get the end of the stack
     * during its initialization, we have to do this here */
    if (sa->sa_allocator == 0) {
        /* Since the lowest stack address shall not be used as `sp` anymore, we will modify it */
        sa->sa_tcbstack = task->pxDummy6;
        sa->sa_allocator = (UBaseType_t) task->pxDummy6;
    }

    /* Check if coprocessor area is allocated */
    if (allocate && sa->sa_coprocs[coproc] == NULL) {
        const uint32_t coproc_sa_sizes[] = {
            RV_COPROC0_SIZE, RV_COPROC1_SIZE, RV_COPROC2_SIZE
        };
        const uint32_t coproc_sa_align[] = {
            RV_COPROC0_ALIGN, RV_COPROC1_ALIGN, RV_COPROC2_ALIGN
        };
        /* The allocator points to a usable part of the stack, use it for the coprocessor.
         * Align it up to the coprocessor save area requirement */
        sa->sa_allocator = (sa->sa_allocator + coproc_sa_align[coproc] - 1) & ~(coproc_sa_align[coproc] - 1);
        sa->sa_coprocs[coproc] = (void*) (sa->sa_allocator);
        sa->sa_allocator += coproc_sa_sizes[coproc];
        /* Update the lowest address of the stack to prevent FreeRTOS performing overflow/watermark checks on the coprocessors contexts */
        task->pxDummy6 = (void*) (sa->sa_allocator);
        /* Make sure the Task stack pointer is not pointing to the coprocessor context area, in other words, make
         * sure we don't have a stack overflow */
        void* task_sp = task->pxDummy1;
        if (task_sp <= task->pxDummy6) {
            /* In theory we need to call vApplicationStackOverflowHook to trigger the stack overflow callback,
             * but in practice, since we are already in an exception handler, this won't work, so let's manually
             * trigger an exception with the previous coprocessor owner's TCB */
            g_panic_abort = true;
            g_panic_abort_details = (char *) "ERROR: Stack overflow while saving coprocessor context!\n";
            xt_unhandled_exception(task_sp);
        }
    }
    return sa;
}


/**
 * @brief Update given coprocessor owner and get the address of former owner's save area.
 *
 * This function is called when the current running task has poked a coprocessor's register which
 * was used by a previous task. We have to save the coprocessor context (registers) inside the
 * current owner's save area and change the ownership. The coprocessor will be marked as used in
 * the new owner's coprocessor save area.
 *
 * @param coreid    Current core
 * @param coproc    Coprocessor to save context of
 *
 * @returns Coprocessor former owner's save area, can be NULL if there was no owner yet, can be -1 if
 *          the former owner is the same as the new owner.
 */
RvCoprocSaveArea* pxPortUpdateCoprocOwner(int coreid, int coproc, StaticTask_t* owner)
{
    RvCoprocSaveArea* sa = NULL;
    /* Address of coprocessor owner */
    StaticTask_t** owner_addr = &port_uxCoprocOwner[ coreid ][ coproc ];
    /* Atomically exchange former owner with the new one */
    StaticTask_t* former = Atomic_SwapPointers_p32((void**) owner_addr, owner);
    /* Get the save area of former owner. small optimization here, if the former owner is the new owner,
     * return -1. This will simplify the assembly code while making it faster. */
    if (former == owner) {
        sa = (void*) -1;
    } else if (former != NULL) {
        /* Allocate coprocessor memory if not available yet */
        sa = pxPortGetCoprocArea(former, true, coproc);
    }
    return sa;
}


/**
 * @brief Aborts execution when a coprocessor was used in an ISR context
 */
void vPortCoprocUsedInISR(void* frame)
{
    /* Since this function is called from an exception handler, the interrupts are disabled,
     * as such, it is not possible to trigger another exception as would `abort` do.
     * Simulate an abort without actually triggering an exception. */
    g_panic_abort = true;
    g_panic_abort_details = (char *) "ERROR: Coprocessors must not be used in ISRs!\n";
    xt_unhandled_exception(frame);
}

#endif /* SOC_CPU_COPROC_NUM > 0 */

/* ---------------------------------------------- Misc Implementations -------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */
