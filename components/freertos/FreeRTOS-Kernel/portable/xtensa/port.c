/*
 * FreeRTOS Kernel V10.5.1
 * Copyright (C) 2015-2019 Cadence Design Systems, Inc.
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-FileCopyrightText: 2015-2019 Cadence Design Systems, Inc
 * SPDX-FileCopyrightText: 2021 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2023-2024 Espressif Systems (Shanghai) CO LTD
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

#include "sdkconfig.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xtensa/config/core.h>
#include <xtensa_context.h>
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_private/crosscore_int.h"
#include "esp_private/esp_int_wdt.h"
#include "esp_system.h"
#include "esp_log.h"
#include "FreeRTOS.h"        /* This pulls in portmacro.h */
#include "task.h"            /* Required for TaskHandle_t, tskNO_AFFINITY, and vTaskStartScheduler */
#include "port_systick.h"
#include "esp_cpu.h"
#include "esp_memory_utils.h"

_Static_assert(portBYTE_ALIGNMENT == 16, "portBYTE_ALIGNMENT must be set to 16");

/* ---------------------------------------------------- Variables ------------------------------------------------------
 * - Various variables used to maintain the FreeRTOS port's state. Used from both port.c and various .S files
 * - Constant offsets are used by assembly to jump to particular TCB members or a stack area (such as the CPSA). We use
 *   C constants instead of preprocessor macros due to assembly lacking "offsetof()".
 * ------------------------------------------------------------------------------------------------------------------ */

#if XCHAL_CP_NUM > 0
/* Offsets used to navigate to a task's CPSA on the stack */
const DRAM_ATTR uint32_t offset_pxEndOfStack = offsetof(StaticTask_t, pxDummy8);
const DRAM_ATTR uint32_t offset_cpsa = XT_CP_SIZE;  /* Offset to start of the CPSA area on the stack. See uxInitialiseStackCPSA(). */
#if configNUM_CORES > 1
/* Offset to TCB_t.xCoreID member. Used to pin unpinned tasks that use the FPU. */
const DRAM_ATTR uint32_t offset_xCoreID = offsetof(StaticTask_t, xDummyCoreID);
#endif /* configNUM_CORES > 1 */
#endif /* XCHAL_CP_NUM > 0 */

volatile unsigned port_xSchedulerRunning[portNUM_PROCESSORS] = {0}; // Indicates whether scheduler is running on a per-core basis
unsigned port_interruptNesting[portNUM_PROCESSORS] = {0};  // Interrupt nesting level. Increased/decreased in portasm.c, _frxt_int_enter/_frxt_int_exit
BaseType_t port_uxCriticalNesting[portNUM_PROCESSORS] = {0};
BaseType_t port_uxOldInterruptState[portNUM_PROCESSORS] = {0};

/*
*******************************************************************************
* Interrupt stack. The size of the interrupt stack is determined by the config
* parameter "configISR_STACK_SIZE" in FreeRTOSConfig.h
*******************************************************************************
*/
volatile StackType_t DRAM_ATTR __attribute__((aligned(16))) port_IntStack[portNUM_PROCESSORS][configISR_STACK_SIZE];
/* One flag for each individual CPU. */
volatile uint32_t port_switch_flag[portNUM_PROCESSORS];

/* ------------------------------------------------ FreeRTOS Portable --------------------------------------------------
 * - Provides implementation for functions required by FreeRTOS
 * - Declared in portable.h
 * ------------------------------------------------------------------------------------------------------------------ */

// ----------------- Scheduler Start/End -------------------

/* Defined in xtensa_context.S */
extern void _xt_coproc_init(void);

BaseType_t xPortStartScheduler( void )
{
    portDISABLE_INTERRUPTS();
    // Interrupts are disabled at this point and stack contains PS with enabled interrupts when task context is restored

#if XCHAL_CP_NUM > 0
    /* Initialize co-processor management for tasks. Leave CPENABLE alone. */
    _xt_coproc_init();
#endif

    /* Setup the hardware to generate the tick. */
    vPortSetupTimer();

    port_xSchedulerRunning[xPortGetCoreID()] = 1;

    // Windows contain references to the startup stack which will be reclaimed by the main task
    // Spill the windows to create a clean environment to ensure we do not carry over any such references
    // to invalid SPs which will cause problems if main_task does a windowoverflow to them
    xthal_window_spill();

    // Cannot be directly called from C; never returns
    __asm__ volatile ("call0    _frxt_dispatch\n");

    /* Should not get here. */
    return pdTRUE;
}

void vPortEndScheduler( void )
{
    /* It is unlikely that the Xtensa port will get stopped.  If required simply
    disable the tick interrupt here. */
    abort();
}

// ------------------------ Stack --------------------------

// User exception dispatcher when exiting
void _xt_user_exit(void);

#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
// Wrapper to allow task functions to return (increases stack overhead by 16 bytes)
static void vPortTaskWrapper(TaskFunction_t pxCode, void *pvParameters)
{
    pxCode(pvParameters);
    //FreeRTOS tasks should not return. Log the task name and abort.
    char *pcTaskName = pcTaskGetName(NULL);
    ESP_LOGE("FreeRTOS", "FreeRTOS Task \"%s\" should not return, Aborting now!", pcTaskName);
    abort();
}
#endif

/**
 * @brief Align stack pointer in a downward growing stack
 *
 * This macro is used to round a stack pointer downwards to the nearest n-byte boundary, where n is a power of 2.
 * This macro is generally used when allocating aligned areas on a downward growing stack.
 */
#define STACKPTR_ALIGN_DOWN(n, ptr)     ((ptr) & (~((n)-1)))

#if XCHAL_CP_NUM > 0
/**
 * @brief Allocate and initialize coprocessor save area on the stack
 *
 * This function allocates the coprocessor save area on the stack (sized XT_CP_SIZE) which includes...
 *  - Individual save areas for each coprocessor (size XT_CPx_SA, inclusive of each area's alignment)
 *  - Coprocessor context switching flags (e.g., XT_CPENABLE, XT_CPSTORED, XT_CP_CS_ST, XT_CP_ASA).
 *
 * The coprocessor save area is aligned to a 16-byte boundary.
 * The coprocessor context switching flags are then initialized
 *
 * @param[in] uxStackPointer Current stack pointer address
 * @return Stack pointer that points to allocated and initialized the coprocessor save area
 */
FORCE_INLINE_ATTR UBaseType_t uxInitialiseStackCPSA(UBaseType_t uxStackPointer)
{
    /*
    HIGH ADDRESS
    |-------------------|      XT_CP_SIZE
    | CPn SA            |           ^
    | ...               |           |
    | CP0 SA            |           |
    | ----------------- |           |       ---- XCHAL_TOTAL_SA_ALIGN aligned
    |-------------------|           |   12 bytes
    | XT_CP_ASA         |           |       ^
    | XT_CP_CS_ST       |           |       |
    | XT_CPSTORED       |           |       |
    | XT_CPENABLE       |           |       |
    |-------------------| ---------------------- 16 byte aligned
    LOW ADDRESS
    */

    // Allocate overall coprocessor save area, aligned down to 16 byte boundary
    uxStackPointer = STACKPTR_ALIGN_DOWN(16, uxStackPointer - XT_CP_SIZE);
    // Initialize the coprocessor context switching flags.
    uint32_t *p = (uint32_t *)uxStackPointer;
    p[0] = 0;   // Clear XT_CPENABLE and XT_CPSTORED
    p[1] = 0;   // Clear XT_CP_CS_ST
    // XT_CP_ASA points to the aligned start of the individual CP save areas (i.e., start of CP0 SA)
    p[2] = (uint32_t)ALIGNUP(XCHAL_TOTAL_SA_ALIGN, (uint32_t)uxStackPointer + 12);
    return uxStackPointer;
}
#endif /* XCHAL_CP_NUM > 0 */

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
    TLS layout at link-time, where 0xNNN is the offset that the linker calculates to a particular TLS variable.

    LOW ADDRESS
            |---------------------------|   Linker Symbols
            | Section                   |   --------------
            | .flash.rodata             |
         0x0|---------------------------| <- _flash_rodata_start
          ^ | Other Data                |
          | |---------------------------| <- _thread_local_start
          | | .tbss                     | ^
          V |                           | |
      0xNNN | int example;              | | tls_area_size
            |                           | |
            | .tdata                    | V
            |---------------------------| <- _thread_local_end
            | Other data                |
            | ...                       |
            |---------------------------|
    HIGH ADDRESS
    */
    // Calculate the TLS area's size (rounded up to multiple of 16 bytes).
    extern int _thread_local_start, _thread_local_end, _flash_rodata_start, _flash_rodata_align;
    const uint32_t tls_area_size = ALIGNUP(16, (uint32_t)&_thread_local_end - (uint32_t)&_thread_local_start);
    // TODO: check that TLS area fits the stack

    // Allocate space for the TLS area on the stack. The area must be allocated at a 16-byte aligned address
    uxStackPointer = STACKPTR_ALIGN_DOWN(16, uxStackPointer - (UBaseType_t)tls_area_size);
    // Initialize the TLS area with the initialization values of each TLS variable
    memcpy((void *)uxStackPointer, &_thread_local_start, tls_area_size);

    /*
    Calculate the THREADPTR register's initialization value based on the link-time offset and the TLS area allocated on
    the stack.

    HIGH ADDRESS
            |---------------------------|
            | .tdata (*)                |
          ^ | int example;              |
          | |                           |
          | | .tbss (*)                 |
          | |---------------------------| <- uxStackPointer (start of TLS area)
    0xNNN | |                           | ^
          | |                           | |
          |             ...               | (_thread_local_start - _flash_rodata_start) + align_up(TCB_SIZE, tls_section_alignment)
          | |                           | |
          | |                           | V
          V |                           | <- threadptr register's value

    LOW ADDRESS

    Note: Xtensa is slightly different compared to the RISC-V port as there is an implicit aligned TCB_SIZE added to
    the offset. (search for 'tpoff' in elf32-xtensa.c in BFD):
        - "offset = address - tls_section_vma + align_up(TCB_SIZE, tls_section_alignment)"
        - TCB_SIZE is hardcoded to 8
    */
    const uint32_t tls_section_align = (uint32_t)&_flash_rodata_align;  // ALIGN value of .flash.rodata section
    #define TCB_SIZE 8
    const uint32_t base = ALIGNUP(tls_section_align, TCB_SIZE);
    *ret_threadptr_reg_init = (uint32_t)uxStackPointer - ((uint32_t)&_thread_local_start - (uint32_t)&_flash_rodata_start) - base;

    return uxStackPointer;
}

/**
 * @brief Initialize the task's starting interrupt stack frame
 *
 * This function initializes the task's starting interrupt stack frame. The dispatcher will use this stack frame in a
 * context restore routine. Therefore, the starting stack frame must be initialized as if the task was interrupted right
 * before its first instruction is called.
 *
 * - The stack frame is allocated to a 16-byte aligned address
 * - The THREADPTR register is saved in the extra storage area of the stack frame. This is also initialized
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
    HIGH ADDRESS
    |---------------------------|       ^ XT_STK_FRMSZ
    |                           |       |
    | Stack Frame Extra Storage |       |
    |                           |       |
    | ------------------------- |       |   ^ XT_STK_EXTRA
    |                           |       |   |
    | Intr/Exc Stack Frame      |       |   |
    |                           |       V   V
    | ------------------------- | ---------------------- 16 byte aligned
    LOW ADDRESS
    */

    /*
    Allocate space for the task's starting interrupt stack frame.
    - The stack frame must be allocated to a 16-byte aligned address.
    - We use XT_STK_FRMSZ (instead of sizeof(XtExcFrame)) as it...
        - includes the size of the extra storage area
        - includes the size for a base save area before the stack frame
        - rounds up the total size to a multiple of 16
    */
    UBaseType_t uxStackPointerPrevious = uxStackPointer;
    uxStackPointer = STACKPTR_ALIGN_DOWN(16, uxStackPointer - XT_STK_FRMSZ);

    // Clear the entire interrupt stack frame
    memset((void *)uxStackPointer, 0, (size_t)(uxStackPointerPrevious - uxStackPointer));

    XtExcFrame *frame = (XtExcFrame *)uxStackPointer;

    /*
    Initialize common registers
    */
    frame->a0 = 0;                                          // Set the return address to 0 terminate GDB backtrace
    frame->a1 = uxStackPointer + XT_STK_FRMSZ;              // Saved stack pointer should point to physical top of stack frame
    frame->exit = (UBaseType_t) _xt_user_exit;              // User exception exit dispatcher

    /*
    Initialize the task's entry point. This will differ depending on
    - Whether the task's entry point is the wrapper function or pxCode
    - Whether Windowed ABI is used (for windowed, we mimic the task entry point being call4'd )
    */
    #if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
        frame->pc = (UBaseType_t) vPortTaskWrapper;         // Task entry point is the wrapper function
        #ifdef __XTENSA_CALL0_ABI__
            frame->a2 = (UBaseType_t) pxCode;               // Wrapper function's argument 0 (which is the task function)
            frame->a3 = (UBaseType_t) pvParameters;         // Wrapper function's argument 1 (which is the task function's argument)
        #else // __XTENSA_CALL0_ABI__
            frame->a6 = (UBaseType_t) pxCode;               // Wrapper function's argument 0 (which is the task function), passed as if we call4'd
            frame->a7 = (UBaseType_t) pvParameters;         // Wrapper function's argument 1 (which is the task function's argument), passed as if we call4'd
        #endif // __XTENSA_CALL0_ABI__
    #else
        frame->pc = (UBaseType_t) pxCode;                   // Task entry point is the provided task function
        #ifdef __XTENSA_CALL0_ABI__
            frame->a2 = (UBaseType_t) pvParameters;         // Task function's argument
        #else // __XTENSA_CALL0_ABI__
            frame->a6 = (UBaseType_t) pvParameters;         // Task function's argument, passed as if we call4'd
        #endif // __XTENSA_CALL0_ABI__
    #endif

    /*
    Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode.
    For windowed ABI also set WOE and CALLINC (pretend task was 'call4'd)
    */
    #ifdef __XTENSA_CALL0_ABI__
        frame->ps = PS_UM | PS_EXCM;
    #else // __XTENSA_CALL0_ABI__
        frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
    #endif // __XTENSA_CALL0_ABI__

    #ifdef XT_USE_SWPRI
        // Set the initial virtual priority mask value to all 1's.
        frame->vpri = 0xFFFFFFFF;
    #endif

    // Initialize the threadptr register in the extra save area of the stack frame
    uint32_t *threadptr_reg = (uint32_t *)(uxStackPointer + XT_STK_EXTRA);
    *threadptr_reg = threadptr_reg_init;

    return uxStackPointer;
}

#if ( portHAS_STACK_OVERFLOW_CHECKING == 1 )
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     StackType_t * pxEndOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
#else
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
#endif
{
#ifdef __clang_analyzer__
    // Teach clang-tidy that pxTopOfStack cannot be a pointer to const
    volatile StackType_t * pxTemp = pxTopOfStack;
    pxTopOfStack = pxTemp;
#endif /*__clang_analyzer__ */
    /*
    HIGH ADDRESS
    |---------------------------| <- pxTopOfStack on entry
    | Coproc Save Area          | (CPSA MUST BE FIRST)
    | ------------------------- |
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
    */
    UBaseType_t uxStackPointer = (UBaseType_t)pxTopOfStack;
    // Make sure the incoming stack pointer is aligned on 16
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);
#if XCHAL_CP_NUM > 0
    // Initialize the coprocessor save area. THIS MUST BE THE FIRST AREA due to access from _frxt_task_coproc_state()
    uxStackPointer = uxInitialiseStackCPSA(uxStackPointer);
    // Each allocated section on the stack must have a size aligned on 16
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);
#endif /* XCHAL_CP_NUM > 0 */

    // Initialize the GCC TLS area
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
    unsigned int irqStatus;
    BaseType_t ret;
    irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    ret = (port_interruptNesting[xPortGetCoreID()] != 0);
    portCLEAR_INTERRUPT_MASK_FROM_ISR(irqStatus);
    return ret;
}

void vPortAssertIfInISR(void)
{
    /* Assert if the interrupt nesting count is > 0 */
    configASSERT(xPortInIsrContext() == 0);
}

BaseType_t IRAM_ATTR xPortInterruptedFromISRContext(void)
{
    return (port_interruptNesting[xPortGetCoreID()] != 0);
}

// ------------------ Critical Sections --------------------

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

void __attribute__((optimize("-O3"))) vPortExitCritical(portMUX_TYPE *mux)
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
        vPortExitCritical(mux);
    } else {
        esp_rom_printf("port*_CRITICAL called from ISR context. Aborting!\n");
        abort();
    }
}

// ---------------------- Yielding -------------------------

void vPortYieldOtherCore( BaseType_t coreid )
{
    esp_crosscore_int_send_yield( coreid );
}

// ------------------- Hook Functions ----------------------

void  __attribute__((weak)) vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
#define ERR_STR1 "***ERROR*** A stack overflow in task "
#define ERR_STR2 " has been detected."
    const char *str[] = {ERR_STR1, pcTaskName, ERR_STR2};

    char buf[sizeof(ERR_STR1) + CONFIG_FREERTOS_MAX_TASK_NAME_LEN + sizeof(ERR_STR2) + 1 /* null char */] = { 0 };

    char *dest = buf;
    for (size_t i = 0 ; i < sizeof(str) / sizeof(str[0]); i++) {
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

void vPortSetStackWatchpoint( void *pxStackStart )
{
    //Set watchpoint 1 to watch the last 32 bytes of the stack.
    //Unfortunately, the Xtensa watchpoints can't set a watchpoint on a random [base - base+n] region because
    //the size works by masking off the lowest address bits. For that reason, we futz a bit and watch the lowest 32
    //bytes of the stack we can actually watch. In general, this can cause the watchpoint to be triggered at most
    //28 bytes early. The value 32 is chosen because it's larger than the stack canary, which in FreeRTOS is 20 bytes.
    //This way, we make sure we trigger before/when the stack canary is corrupted, not after.
    int addr = (int)pxStackStart;
    addr = (addr + 31) & (~31);
    esp_cpu_set_watchpoint(STACK_WATCH_POINT_NUMBER, (char *)addr, 32, ESP_CPU_WATCHPOINT_STORE);
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
                // We call EARLY log here as currently portCLEAN_UP_TCB() is called in a critical section
                ESP_EARLY_LOGE("FreeRTOS", "Fatal error: TLSP deletion callback at index %d overwritten with non-excutable pointer %p", x, pvThreadLocalStoragePointersDelCallback[ x ]);
                abort();
            }

            pvThreadLocalStoragePointersDelCallback[ x ]( x, tcb->pvDummy15[ x ] );   //Call del cb
        }
    }
}
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

#if ( XCHAL_CP_NUM > 0 )
static void vPortCleanUpCoprocArea(void *pvTCB)
{
    UBaseType_t uxCoprocArea;
    BaseType_t xTargetCoreID;

    /* Get a pointer to the task's coprocessor save area */
    uxCoprocArea = ( UBaseType_t ) ( ( ( StaticTask_t * ) pvTCB )->pxDummy8 );  /* Get TCB_t.pxEndOfStack */
    uxCoprocArea = STACKPTR_ALIGN_DOWN(16, uxCoprocArea - XT_CP_SIZE);

    #if ( configNUMBER_OF_CORES > 1 )
        /* Get xTargetCoreID from the TCB.xCoreID */
        xTargetCoreID = ( ( StaticTask_t * ) pvTCB )->xDummyCoreID;
    #else /* configNUMBER_OF_CORES > 1 */
        xTargetCoreID = 0;
    #endif /* configNUMBER_OF_CORES > 1 */

    /* If task has live floating point registers somewhere, release them */
    void _xt_coproc_release(volatile void *coproc_sa_base, BaseType_t xTargetCoreID);
    _xt_coproc_release( (void *)uxCoprocArea, xTargetCoreID );
}
#endif /* XCHAL_CP_NUM > 0 */

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

    #if ( XCHAL_CP_NUM > 0 )
        /* Cleanup coproc save area */
        vPortCleanUpCoprocArea( pxTCB );
    #endif /* XCHAL_CP_NUM > 0 */
}
