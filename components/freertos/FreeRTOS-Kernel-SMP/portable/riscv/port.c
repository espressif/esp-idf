/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <string.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#include "riscv/rvruntime-frames.h"
#include "riscv/rv_utils.h"
#include "riscv/interrupt.h"
#include "esp_private/crosscore_int.h"
#include "esp_private/esp_int_wdt.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/systimer.h"
#include "esp_attr.h"
#include "esp_system.h"
#include "esp_heap_caps_init.h"
#include "esp_task_wdt.h"
#include "esp_task.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "FreeRTOS.h"       /* This pulls in portmacro.h */
#include "task.h"
#include "portmacro.h"
#include "esp_memory_utils.h"
#ifdef CONFIG_FREERTOS_SYSTICK_USES_SYSTIMER
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#endif

#ifdef CONFIG_PM_TRACE
#include "esp_private/pm_trace.h"
#endif //CONFIG_PM_TRACE

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
#include "esp_gdbstub.h"
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

_Static_assert(portBYTE_ALIGNMENT == 16, "portBYTE_ALIGNMENT must be set to 16");

/* ---------------------------------------------------- Variables ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

static const char *TAG = "cpu_start"; // [refactor-todo]: might be appropriate to change in the future, but

BaseType_t uxSchedulerRunning = 0;
volatile UBaseType_t uxInterruptNesting = 0;
portMUX_TYPE port_xTaskLock = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE port_xISRLock = portMUX_INITIALIZER_UNLOCKED;
volatile BaseType_t xPortSwitchFlag = 0;
__attribute__((aligned(16))) static StackType_t xIsrStack[configISR_STACK_SIZE];
StackType_t *xIsrStackTop = &xIsrStack[0] + (configISR_STACK_SIZE & (~((portPOINTER_SIZE_TYPE)portBYTE_ALIGNMENT_MASK)));

// Variables used for IDF style critical sections. These are orthogonal to FreeRTOS critical sections
static UBaseType_t port_uxCriticalNestingIDF = 0;
static UBaseType_t port_uxCriticalOldInterruptStateIDF = 0;

/* ------------------------------------------------ IDF Compatibility --------------------------------------------------
 * - These need to be defined for IDF to compile
 * ------------------------------------------------------------------------------------------------------------------ */

// ------------------ Critical Sections --------------------

void vPortEnterCritical(void)
{
    // Save current interrupt threshold and disable interrupts
    UBaseType_t old_thresh = ulPortSetInterruptMask();
    // Update the IDF critical nesting count
    port_uxCriticalNestingIDF++;
    if (port_uxCriticalNestingIDF == 1) {
        // Save a copy of the old interrupt threshold
        port_uxCriticalOldInterruptStateIDF = (UBaseType_t) old_thresh;
    }
}

void vPortExitCritical(void)
{
    if (port_uxCriticalNestingIDF > 0) {
        port_uxCriticalNestingIDF--;
        if (port_uxCriticalNestingIDF == 0) {
            // Restore the saved interrupt threshold
            vPortClearInterruptMask((int)port_uxCriticalOldInterruptStateIDF);
        }
    }
}

// ----------------------- System --------------------------

#define STACK_WATCH_AREA_SIZE 32
#define STACK_WATCH_POINT_NUMBER (SOC_CPU_WATCHPOINTS_NUM - 1)

void vPortSetStackWatchpoint(void *pxStackStart)
{
    uint32_t addr = (uint32_t)pxStackStart;
    addr = (addr + (STACK_WATCH_AREA_SIZE - 1)) & (~(STACK_WATCH_AREA_SIZE - 1));
    esp_cpu_set_watchpoint(STACK_WATCH_POINT_NUMBER, (char *)addr, STACK_WATCH_AREA_SIZE, ESP_CPU_WATCHPOINT_STORE);
}

// ---------------------- Tick Timer -----------------------

BaseType_t xPortSysTickHandler(void);

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT

#ifdef CONFIG_FREERTOS_CORETIMER_0
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER0_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif
#ifdef CONFIG_FREERTOS_CORETIMER_1
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER1_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif

#elif CONFIG_FREERTOS_SYSTICK_USES_SYSTIMER

_Static_assert(SOC_CPU_CORES_NUM <= SOC_SYSTIMER_ALARM_NUM - 1, "the number of cores must match the number of core alarms in SYSTIMER");

void SysTickIsrHandler(void *arg);

static uint32_t s_handled_systicks[portNUM_PROCESSORS] = { 0 };

#define SYSTICK_INTR_ID (ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE)

/**
 * @brief Set up the systimer peripheral to generate the tick interrupt
 *
 * Both timer alarms are configured in periodic mode.
 * It is done at the same time so SysTicks for both CPUs occur at the same time or very close.
 * Shifts a time of triggering interrupts for core 0 and core 1.
 */
void vPortSetupTimer(void)
{
    unsigned cpuid = xPortGetCoreID();
#ifdef CONFIG_FREERTOS_CORETIMER_SYSTIMER_LVL3
    const unsigned level = ESP_INTR_FLAG_LEVEL3;
#else
    const unsigned level = ESP_INTR_FLAG_LEVEL1;
#endif
    /* Systimer HAL layer object */
    static systimer_hal_context_t systimer_hal;
    /* set system timer interrupt vector */
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE + cpuid, ESP_INTR_FLAG_IRAM | level, SysTickIsrHandler, &systimer_hal, NULL));

    if (cpuid == 0) {
        periph_module_enable(PERIPH_SYSTIMER_MODULE);
        systimer_hal_init(&systimer_hal);
        systimer_hal_tick_rate_ops_t ops = {
            .ticks_to_us = systimer_ticks_to_us,
            .us_to_ticks = systimer_us_to_ticks,
        };
        systimer_hal_set_tick_rate_ops(&systimer_hal, &ops);
        systimer_ll_set_counter_value(systimer_hal.dev, SYSTIMER_LL_COUNTER_OS_TICK, 0);
        systimer_ll_apply_counter_value(systimer_hal.dev, SYSTIMER_LL_COUNTER_OS_TICK);

        for (cpuid = 0; cpuid < SOC_CPU_CORES_NUM; cpuid++) {
            systimer_hal_counter_can_stall_by_cpu(&systimer_hal, SYSTIMER_LL_COUNTER_OS_TICK, cpuid, false);
        }

        for (cpuid = 0; cpuid < portNUM_PROCESSORS; ++cpuid) {
            uint32_t alarm_id = SYSTIMER_LL_ALARM_OS_TICK_CORE0 + cpuid;

            /* configure the timer */
            systimer_hal_connect_alarm_counter(&systimer_hal, alarm_id, SYSTIMER_LL_COUNTER_OS_TICK);
            systimer_hal_set_alarm_period(&systimer_hal, alarm_id, 1000000UL / CONFIG_FREERTOS_HZ);
            systimer_hal_select_alarm_mode(&systimer_hal, alarm_id, SYSTIMER_ALARM_MODE_PERIOD);
            systimer_hal_counter_can_stall_by_cpu(&systimer_hal, SYSTIMER_LL_COUNTER_OS_TICK, cpuid, true);
            if (cpuid == 0) {
                systimer_hal_enable_alarm_int(&systimer_hal, alarm_id);
                systimer_hal_enable_counter(&systimer_hal, SYSTIMER_LL_COUNTER_OS_TICK);
#ifndef CONFIG_FREERTOS_UNICORE
                // SysTick of core 0 and core 1 are shifted by half of period
                systimer_hal_counter_value_advance(&systimer_hal, SYSTIMER_LL_COUNTER_OS_TICK, 1000000UL / CONFIG_FREERTOS_HZ / 2);
#endif
            }
        }
    } else {
        uint32_t alarm_id = SYSTIMER_LL_ALARM_OS_TICK_CORE0 + cpuid;
        systimer_hal_enable_alarm_int(&systimer_hal, alarm_id);
    }
}

/**
 * @brief Systimer interrupt handler.
 *
 * The Systimer interrupt for SysTick works in periodic mode no need to calc the next alarm.
 * If a timer interrupt is ever serviced more than one tick late, it is necessary to process multiple ticks.
 */
IRAM_ATTR void SysTickIsrHandler(void *arg)
{
    uint32_t cpuid = xPortGetCoreID();
    systimer_hal_context_t *systimer_hal = (systimer_hal_context_t *)arg;
#ifdef CONFIG_PM_TRACE
    ESP_PM_TRACE_ENTER(TICK, cpuid);
#endif

    uint32_t alarm_id = SYSTIMER_LL_ALARM_OS_TICK_CORE0 + cpuid;
    do {
        systimer_ll_clear_alarm_int(systimer_hal->dev, alarm_id);

        uint32_t diff = systimer_hal_get_counter_value(systimer_hal, SYSTIMER_LL_COUNTER_OS_TICK) / systimer_ll_get_alarm_period(systimer_hal->dev, alarm_id) - s_handled_systicks[cpuid];
        if (diff > 0) {
            if (s_handled_systicks[cpuid] == 0) {
                s_handled_systicks[cpuid] = diff;
                diff = 1;
            } else {
                s_handled_systicks[cpuid] += diff;
            }

            do {
                xPortSysTickHandler();
            } while (--diff);
        }
    } while (systimer_ll_is_alarm_int_fired(systimer_hal->dev, alarm_id));

#ifdef CONFIG_PM_TRACE
    ESP_PM_TRACE_EXIT(TICK, cpuid);
#endif
}

#endif // CONFIG_FREERTOS_SYSTICK_USES_SYSTIMER

// --------------------- App Start-up ----------------------

extern void app_main(void);

static void main_task(void *args)
{
#if !CONFIG_FREERTOS_UNICORE
    // Wait for FreeRTOS initialization to finish on APP CPU, before replacing its startup stack
    while (uxSchedulerRunning == 0) {
        ;
    }
#endif

    // [refactor-todo] check if there is a way to move the following block to esp_system startup
    heap_caps_enable_nonos_stack_heaps();

    // Now we have startup stack RAM available for heap, enable any DMA pool memory
#if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
    if (g_spiram_ok) {
        esp_err_t r = esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Could not reserve internal/DMA pool (error 0x%x)", r);
            abort();
        }
    }
#endif

    //Initialize task wdt if configured to do so
#if CONFIG_ESP_TASK_WDT_INIT
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000,
        .idle_core_mask = 0,
#if CONFIG_ESP_TASK_WDT_PANIC
        .trigger_panic = true,
#endif
    };
#if CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
    twdt_config.idle_core_mask |= (1 << 0);
#endif
#if CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
    twdt_config.idle_core_mask |= (1 << 1);
#endif
    ESP_ERROR_CHECK(esp_task_wdt_init(&twdt_config));
#endif // CONFIG_ESP_TASK_WDT_INIT

    app_main();
    vTaskDelete(NULL);
}

void esp_startup_start_app_common(void)
{
#if CONFIG_ESP_INT_WDT_INIT
    esp_int_wdt_init();
    //Initialize the interrupt watch dog for CPU0.
    esp_int_wdt_cpu_init();
#endif

    esp_crosscore_int_init();

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    esp_gdbstub_init();
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

    portBASE_TYPE res = xTaskCreatePinnedToCore(main_task, "main",
                                                ESP_TASK_MAIN_STACK, NULL,
                                                ESP_TASK_MAIN_PRIO, NULL, ESP_TASK_MAIN_CORE);
    assert(res == pdTRUE);
    (void)res;
}

void esp_startup_start_app(void)
{
    esp_startup_start_app_common();

    ESP_LOGI(TAG, "Starting scheduler.");
    vTaskStartScheduler();
}

/* ---------------------------------------------- Port Implementations -------------------------------------------------
 * Implementations of Porting Interface functions
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

UBaseType_t ulPortSetInterruptMask(void)
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

void vPortClearInterruptMask(UBaseType_t mask)
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

BaseType_t xPortCheckIfInISR(void)
{
    return uxInterruptNesting;
}

// ------------------ Critical Sections --------------------

void IRAM_ATTR vPortTakeLock( portMUX_TYPE *lock )
{
    spinlock_acquire( lock, portMUX_NO_TIMEOUT);
}

void IRAM_ATTR vPortReleaseLock( portMUX_TYPE *lock )
{
    spinlock_release( lock );
}

// ---------------------- Yielding -------------------------

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
        while (uxSchedulerRunning && REG_READ(SYSTEM_CPU_INTR_FROM_CPU_0_REG) != 0) {}
    }
}

void vPortYieldFromISR( void )
{
    //traceISR_EXIT_TO_SCHEDULER();
    uxSchedulerRunning = 1;
    xPortSwitchFlag = 1;
}

/* ------------------------------------------------ FreeRTOS Portable --------------------------------------------------
 * - Provides implementation for functions required by FreeRTOS
 * - Declared in portable.h
 * ------------------------------------------------------------------------------------------------------------------ */

// ----------------- Scheduler Start/End -------------------

BaseType_t xPortStartScheduler(void)
{
    uxInterruptNesting = 0;
    port_uxCriticalNestingIDF = 0;
    uxSchedulerRunning = 0;

    /* Setup the hardware to generate the tick. */
    vPortSetupTimer();

    esprv_intc_int_set_threshold(1); /* set global INTC masking level */
    rv_utils_intr_global_enable();

    vPortYield();

    /*Should not get here*/
    return pdFALSE;
}

void vPortEndScheduler(void)
{
    /* very unlikely this function will be called, so just trap here */
    abort();
}

// ----------------------- Memory --------------------------

#define FREERTOS_SMP_MALLOC_CAPS    (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)

void *pvPortMalloc( size_t xSize )
{
    return heap_caps_malloc(xSize, FREERTOS_SMP_MALLOC_CAPS);
}

void vPortFree( void *pv )
{
    heap_caps_free(pv);
}

void vPortInitialiseBlocks( void )
{
    ;   //Does nothing, heap is initialized separately in ESP-IDF
}

size_t xPortGetFreeHeapSize( void )
{
    return esp_get_free_heap_size();
}

#if( configSTACK_ALLOCATION_FROM_SEPARATE_HEAP == 1 )
void *pvPortMallocStack( size_t xSize )
{
    return NULL;
}

void vPortFreeStack( void *pv )
{

}
#endif

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize )
{
    StackType_t *pxStackBufferTemp;
    StaticTask_t *pxTCBBufferTemp;

    /* If the stack grows down then allocate the stack then the TCB so the stack
     * does not grow into the TCB.  Likewise if the stack grows up then allocate
     * the TCB then the stack. */
    #if (portSTACK_GROWTH > 0)
    {
        //Allocate TCB and stack buffer in internal memory
        pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
        pxStackBufferTemp = pvPortMalloc(configMINIMAL_STACK_SIZE);
    }
    #else /* portSTACK_GROWTH */
    {
        //Allocate TCB and stack buffer in internal memory
        pxStackBufferTemp = pvPortMalloc(configMINIMAL_STACK_SIZE);
        pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
    }
    #endif /* portSTACK_GROWTH */

    assert(pxStackBufferTemp != NULL);
    assert(pxTCBBufferTemp != NULL);
    // Write back pointers
    *ppxIdleTaskStackBuffer = pxStackBufferTemp;
    *ppxIdleTaskTCBBuffer = pxTCBBufferTemp;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize )
{
    StaticTask_t *pxTCBBufferTemp;
    StackType_t *pxStackBufferTemp;

    /* If the stack grows down then allocate the stack then the TCB so the stack
     * does not grow into the TCB.  Likewise if the stack grows up then allocate
     * the TCB then the stack. */
    #if (portSTACK_GROWTH > 0)
    {
        //Allocate TCB and stack buffer in internal memory
        pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
        pxStackBufferTemp = pvPortMalloc(configTIMER_TASK_STACK_DEPTH);
    }
    #else /* portSTACK_GROWTH */
    {
        //Allocate TCB and stack buffer in internal memory
        pxStackBufferTemp = pvPortMalloc(configTIMER_TASK_STACK_DEPTH);
        pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
    }
    #endif /* portSTACK_GROWTH */

    assert(pxTCBBufferTemp != NULL);
    assert(pxStackBufferTemp != NULL);
    //Write back pointers
    *ppxTimerTaskTCBBuffer = pxTCBBufferTemp;
    *ppxTimerTaskStackBuffer = pxStackBufferTemp;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif //( configSUPPORT_STATIC_ALLOCATION == 1 )

// ------------------------ Stack --------------------------

__attribute__((noreturn)) static void _prvTaskExitError(void)
{
    /* A function that implements a task must not exit or attempt to return to
    its caller as there is nothing to return to.  If a task wants to exit it
    should instead call vTaskDelete( NULL ).

    Artificially force an assert() to be triggered if configASSERT() is
    defined, then stop here so application writers can catch the error. */
    portDISABLE_INTERRUPTS();
    abort();
}

__attribute__((naked)) static void prvTaskExitError(void)
{
    asm volatile(".option push\n" \
                 ".option norvc\n" \
                 "nop\n" \
                 ".option pop");
    /* Task entry's RA will point here. Shifting RA into prvTaskExitError is necessary
       to make GDB backtrace ending inside that function.
       Otherwise backtrace will end in the function laying just before prvTaskExitError in address space. */
    _prvTaskExitError();
}

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
    // Calculate TLS area size and round up to multiple of 16 bytes.
    extern char _thread_local_start, _thread_local_end, _flash_rodata_start;
    const uint32_t tls_area_size = ALIGNUP(16, (uint32_t)&_thread_local_end - (uint32_t)&_thread_local_start);
    // TODO: check that TLS area fits the stack

    // Allocate space for the TLS area on the stack. The area must be aligned to 16-bytes
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
          |             ...               | _thread_local_start - _rodata_start
          | |                           | |
          | |                           | V
          V |                           | <- threadptr register's value

    LOW ADDRESS
    */
    *ret_threadptr_reg_init = (uint32_t)uxStackPointer - ((uint32_t)&_thread_local_start - (uint32_t)&_flash_rodata_start);
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
    - We use RV_STK_FRMSZ (instead of sizeof(RvExcFrame)) as it rounds up the total size to a multiple of 16.
    */
    uxStackPointer = STACKPTR_ALIGN_DOWN(16, uxStackPointer - RV_STK_FRMSZ);

    // Clear the entire interrupt stack frame
    RvExcFrame *frame = (RvExcFrame *)uxStackPointer;
    memset(frame, 0, sizeof(RvExcFrame));

    /*
    Initialize the stack frame.

    Note: Shifting RA into prvTaskExitError is necessary to make the GDB backtrace terminate inside that function.
    Otherwise, the backtrace will end in the function located just before prvTaskExitError in the address space.
    */
    extern uint32_t __global_pointer$;
    frame->ra = (UBaseType_t)prvTaskExitError + 4; // size of the nop instruction at the beginning of prvTaskExitError
    frame->mepc = (UBaseType_t)pxCode;
    frame->a0 = (UBaseType_t)pvParameters;
    frame->gp = (UBaseType_t)&__global_pointer$;
    frame->tp = (UBaseType_t)threadptr_reg_init;

    return uxStackPointer;
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
    /*
    HIGH ADDRESS
    |---------------------------| <- pxTopOfStack on entry
    | TLS Variables             |
    | ------------------------- | <- Start of useable stack
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
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);

    // Initialize GCC TLS area
    uint32_t threadptr_reg_init;
    uxStackPointer = uxInitialiseStackTLS(uxStackPointer, &threadptr_reg_init);
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);

    // Initialize the starting interrupt stack frame
    uxStackPointer = uxInitialiseStackFrame(uxStackPointer, pxCode, pvParameters, threadptr_reg_init);
    configASSERT((uxStackPointer & portBYTE_ALIGNMENT_MASK) == 0);

    // Return the task's current stack pointer address which should point to the starting interrupt stack frame
    return (StackType_t *)uxStackPointer;
    //TODO: IDF-2393
}

// ------- Thread Local Storage Pointers Deletion Callbacks -------

#if ( CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS )
void vPortTLSPointersDelCb( void *pxTCB )
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
#endif // CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS

// -------------------- Tick Handler -----------------------

extern void esp_vApplicationIdleHook(void);
extern void esp_vApplicationTickHook(void);

BaseType_t xPortSysTickHandler(void)
{
#if configBENCHMARK
    portbenchmarkIntLatency();
#endif //configBENCHMARK
    traceISR_ENTER(SYSTICK_INTR_ID);
    BaseType_t ret = xTaskIncrementTick();
    //Manually call the IDF tick hooks
    esp_vApplicationTickHook();
    if (ret != pdFALSE) {
        portYIELD_FROM_ISR();
    } else {
        traceISR_EXIT();
    }
    return ret;
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

#if  (  configUSE_TICK_HOOK > 0 )
void vApplicationTickHook( void )
{
    esp_vApplicationTickHook();
}
#endif

#if CONFIG_FREERTOS_USE_MINIMAL_IDLE_HOOK
/*
By default, the port uses vApplicationMinimalIdleHook() to run IDF style idle
hooks. However, users may also want to provide their own vApplicationMinimalIdleHook().
In this case, we use to -Wl,--wrap option to wrap the user provided vApplicationMinimalIdleHook()
*/
extern void __real_vApplicationMinimalIdleHook( void );
void __wrap_vApplicationMinimalIdleHook( void )
{
    esp_vApplicationIdleHook(); //Run IDF style hooks
    __real_vApplicationMinimalIdleHook(); //Call the user provided vApplicationMinimalIdleHook()
}
#else // CONFIG_FREERTOS_USE_MINIMAL_IDLE_HOOK
void vApplicationMinimalIdleHook( void )
{
    esp_vApplicationIdleHook(); //Run IDF style hooks
}
#endif // CONFIG_FREERTOS_USE_MINIMAL_IDLE_HOOK

/*
 * Hook function called during prvDeleteTCB() to cleanup any
 * user defined static memory areas in the TCB.
 */
#if CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP
void __real_vPortCleanUpTCB( void *pxTCB );

void __wrap_vPortCleanUpTCB( void *pxTCB )
#else
void vPortCleanUpTCB ( void *pxTCB )
#endif /* CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP */
{
#if ( CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP )
    /* Call user defined vPortCleanUpTCB */
    __real_vPortCleanUpTCB( pxTCB );
#endif /* CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP */

#if ( CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS )
    /* Call TLS pointers deletion callbacks */
    vPortTLSPointersDelCb( pxTCB );
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */
}
