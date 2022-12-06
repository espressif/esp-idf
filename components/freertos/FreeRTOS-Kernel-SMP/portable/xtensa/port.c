/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"   //For vApplicationStackOverflowHook
#include "portmacro.h"
#include "spinlock.h"
#include "xt_instr_macros.h"
#include "xtensa/xtensa_context.h"
#include "xtensa/corebits.h"
#include "xtensa/config/core.h"
#include "xtensa/config/core-isa.h"
#include "xtensa/xtruntime.h"
#include "esp_private/esp_int_wdt.h"
#include "esp_private/systimer.h"
#include "esp_private/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "esp_task_wdt.h"
#include "esp_heap_caps_init.h"
#include "esp_freertos_hooks.h"
#include "esp_intr_alloc.h"
#include "esp_memory_utils.h"
#include "esp_chip_info.h"
#if CONFIG_SPIRAM
/* Required by esp_psram_extram_reserve_dma_pool() */
#include "esp_psram.h"
#include "esp_private/esp_psram_extram.h"
#endif
#ifdef CONFIG_APPTRACE_ENABLE
#include "esp_app_trace.h"
#endif
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
#include "esp_gdbstub.h"                    /* Required by esp_gdbstub_init() */
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
#ifdef CONFIG_FREERTOS_SYSTICK_USES_SYSTIMER
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#endif // CONFIG_FREERTOS_SYSTICK_USES_SYSTIMER

_Static_assert(portBYTE_ALIGNMENT == 16, "portBYTE_ALIGNMENT must be set to 16");

/*
OS state variables
*/
volatile unsigned port_xSchedulerRunning[portNUM_PROCESSORS] = {0};
unsigned int port_interruptNesting[portNUM_PROCESSORS] = {0};  // Interrupt nesting level. Increased/decreased in portasm.c, _frxt_int_enter/_frxt_int_exit
//FreeRTOS SMP Locks
portMUX_TYPE port_xTaskLock = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE port_xISRLock = portMUX_INITIALIZER_UNLOCKED;

/* ------------------------------------------------ IDF Compatibility --------------------------------------------------
 * - These need to be defined for IDF to compile
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

BaseType_t IRAM_ATTR xPortInterruptedFromISRContext(void)
{
    return (port_interruptNesting[xPortGetCoreID()] != 0);
}

// ------------------ Critical Sections --------------------

/*
Variables used by IDF critical sections only (SMP tracks critical nesting inside TCB now)
[refactor-todo] Figure out how IDF critical sections will be merged with SMP FreeRTOS critical sections
*/
BaseType_t port_uxCriticalNestingIDF[portNUM_PROCESSORS] = {0};
BaseType_t port_uxCriticalOldInterruptStateIDF[portNUM_PROCESSORS] = {0};

/*
*******************************************************************************
* Interrupt stack. The size of the interrupt stack is determined by the config
* parameter "configISR_STACK_SIZE" in FreeRTOSConfig.h
*******************************************************************************
*/
volatile StackType_t DRAM_ATTR __attribute__((aligned(16))) port_IntStack[portNUM_PROCESSORS][configISR_STACK_SIZE];
/* One flag for each individual CPU. */
volatile uint32_t port_switch_flag[portNUM_PROCESSORS];

BaseType_t xPortEnterCriticalTimeout(portMUX_TYPE *lock, BaseType_t timeout)
{
    /* Interrupts may already be disabled (if this function is called in nested
     * manner). However, there's no atomic operation that will allow us to check,
     * thus we have to disable interrupts again anyways.
     *
     * However, if this is call is NOT nested (i.e., the first call to enter a
     * critical section), we will save the previous interrupt level so that the
     * saved level can be restored on the last call to exit the critical.
     */
    BaseType_t xOldInterruptLevel = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);
    if (!spinlock_acquire(lock, timeout)) {
        //Timed out attempting to get spinlock. Restore previous interrupt level and return
        XTOS_RESTORE_JUST_INTLEVEL((int) xOldInterruptLevel);
        return pdFAIL;
    }
    //Spinlock acquired. Increment the IDF critical nesting count.
    BaseType_t coreID = xPortGetCoreID();
    BaseType_t newNesting = port_uxCriticalNestingIDF[coreID] + 1;
    port_uxCriticalNestingIDF[coreID] = newNesting;
    //If this is the first entry to a critical section. Save the old interrupt level.
    if ( newNesting == 1 ) {
        port_uxCriticalOldInterruptStateIDF[coreID] = xOldInterruptLevel;
    }
    return pdPASS;

}

void vPortExitCriticalIDF(portMUX_TYPE *lock)
{
    /* This function may be called in a nested manner. Therefore, we only need
     * to reenable interrupts if this is the last call to exit the critical. We
     * can use the nesting count to determine whether this is the last exit call.
     */
    spinlock_release(lock);
    BaseType_t coreID = xPortGetCoreID();
    BaseType_t nesting = port_uxCriticalNestingIDF[coreID];
    if (nesting > 0) {
        nesting--;
        port_uxCriticalNestingIDF[coreID] = nesting;
        //This is the last exit call, restore the saved interrupt level
        if ( nesting == 0 ) {
            XTOS_RESTORE_JUST_INTLEVEL((int) port_uxCriticalOldInterruptStateIDF[coreID]);
        }
    }
}

/*
In case any IDF libs called the port critical functions directly instead of through the macros.
Just inline call the IDF versions
*/
void vPortEnterCritical(portMUX_TYPE *lock)
{
    vPortEnterCriticalIDF(lock);
}

void vPortExitCritical(portMUX_TYPE *lock)
{
    vPortExitCriticalIDF(lock);
}

// ----------------------- System --------------------------

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

// ---------------------- Tick Timer -----------------------

BaseType_t xPortSysTickHandler(void);

#ifdef CONFIG_FREERTOS_SYSTICK_USES_CCOUNT
extern void _frxt_tick_timer_init(void);
extern void _xt_tick_divisor_init(void);

#ifdef CONFIG_FREERTOS_CORETIMER_0
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER0_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif
#ifdef CONFIG_FREERTOS_CORETIMER_1
    #define SYSTICK_INTR_ID (ETS_INTERNAL_TIMER1_INTR_SOURCE+ETS_INTERNAL_INTR_SOURCE_OFF)
#endif

/**
 * @brief Initialize CCONT timer to generate the tick interrupt
 *
 */
void vPortSetupTimer(void)
{
    /* Init the tick divisor value */
    _xt_tick_divisor_init();

    _frxt_tick_timer_init();
}

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

#endif // CONFIG_FREERTOS_SYSTICK_USES_CCOUNT

// --------------------- App Start-up ----------------------

static const char *TAG = "cpu_start";

extern void app_main(void);

static void main_task(void *args)
{
#if !CONFIG_FREERTOS_UNICORE
    // Wait for FreeRTOS initialization to finish on APP CPU, before replacing its startup stack
    while (port_xSchedulerRunning[1] == 0) {
        ;
    }
#endif

    // [refactor-todo] check if there is a way to move the following block to esp_system startup
    heap_caps_enable_nonos_stack_heaps();

    // Now we have startup stack RAM available for heap, enable any DMA pool memory
#if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
    if (esp_psram_is_initialized()) {
        esp_err_t r = esp_psram_extram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Could not reserve internal/DMA pool (error 0x%x)", r);
            abort();
        }
    }
#endif

    //Initialize TWDT if configured to do so
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
#endif // CONFIG_ESP_TASK_WDT

    app_main();
    vTaskDelete(NULL);
}

void esp_startup_start_app_common(void)
{
#if CONFIG_ESP_INT_WDT
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

void esp_startup_start_app_other_cores(void)
{
    // For now, we only support up to two core: 0 and 1.
    if (xPortGetCoreID() >= 2) {
        abort();
    }

    // Wait for FreeRTOS initialization to finish on PRO CPU
    while (port_xSchedulerRunning[0] == 0) {
        ;
    }

#if CONFIG_APPTRACE_ENABLE
    // [refactor-todo] move to esp_system initialization
    esp_err_t err = esp_apptrace_init();
    assert(err == ESP_OK && "Failed to init apptrace module on APP CPU!");
#endif

#if CONFIG_ESP_INT_WDT
    //Initialize the interrupt watch dog for CPU1.
    esp_int_wdt_cpu_init();
#endif

    esp_crosscore_int_init();

    ESP_EARLY_LOGI(TAG, "Starting scheduler on APP CPU.");
    xPortStartScheduler();
    abort(); /* Only get to here if FreeRTOS somehow very broken */
}

void esp_startup_start_app(void)
{
#if !CONFIG_ESP_INT_WDT
#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
    assert(!soc_has_cache_lock_bug() && "ESP32 Rev 3 + Dual Core + PSRAM requires INT WDT enabled in project config!");
#endif
#endif

    esp_startup_start_app_common();

    ESP_EARLY_LOGI(TAG, "Starting scheduler on PRO CPU.");
    vTaskStartScheduler();
}


/* ---------------------------------------------- Port Implementations -------------------------------------------------
 * Implementations of Porting Interface functions
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

BaseType_t xPortCheckIfInISR(void)
{
    //Disable interrupts so that reading port_interruptNesting is atomic
    BaseType_t ret;
    unsigned int prev_int_level = portDISABLE_INTERRUPTS();
    ret = (port_interruptNesting[xPortGetCoreID()] != 0) ? pdTRUE : pdFALSE;
    portRESTORE_INTERRUPTS(prev_int_level);
    return ret;
}

// ------------------ Critical Sections --------------------

void vPortTakeLock( portMUX_TYPE *lock )
{
    spinlock_acquire( lock, portMUX_NO_TIMEOUT);
}

void vPortReleaseLock( portMUX_TYPE *lock )
{
    spinlock_release( lock );
}

// ---------------------- Yielding -------------------------

// ----------------------- System --------------------------

/* ------------------------------------------------ FreeRTOS Portable --------------------------------------------------
 * - Provides implementation for functions required by FreeRTOS
 * - Declared in portable.h
 * ------------------------------------------------------------------------------------------------------------------ */

// ----------------- Scheduler Start/End -------------------

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

#if configNUM_CORES > 1
    // Workaround for non-thread safe multi-core OS startup (see IDF-4524)
    if (xPortGetCoreID() != 0) {
        vTaskStartSchedulerOtherCores();
    }
#endif // configNUM_CORES > 1

    // Cannot be directly called from C; never returns
    __asm__ volatile ("call0    _frxt_dispatch\n");

    /* Should not get here. */
    return pdTRUE;
}

void vPortEndScheduler( void )
{
    ;
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
    /* Stack always grows downwards (from high address to low address) on all
     * ESP Xtensa targets. Given that the heap allocator likely allocates memory
     * from low to high address, we allocate the stack first and then the TCB so
     * that the stack does not grow downwards into the TCB.
     *
     * Allocate TCB and stack buffer in internal memory. */
    pxStackBufferTemp = pvPortMalloc(CONFIG_FREERTOS_IDLE_TASK_STACKSIZE);
    pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
    assert(pxStackBufferTemp != NULL);
    assert(pxTCBBufferTemp != NULL);
    // Write back pointers
    *ppxIdleTaskStackBuffer = pxStackBufferTemp;
    *ppxIdleTaskTCBBuffer = pxTCBBufferTemp;
    *pulIdleTaskStackSize = CONFIG_FREERTOS_IDLE_TASK_STACKSIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize )
{
    StaticTask_t *pxTCBBufferTemp;
    StackType_t *pxStackBufferTemp;
    /* Stack always grows downwards (from high address to low address) on all
     * ESP Xtensa targets. Given that the heap allocator likely allocates memory
     * from low to high address, we allocate the stack first and then the TCB so
     * that the stack does not grow downwards into the TCB.
     *
     * Allocate TCB and stack buffer in internal memory. */
    pxStackBufferTemp = pvPortMalloc(configTIMER_TASK_STACK_DEPTH);
    pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
    assert(pxStackBufferTemp != NULL);
    assert(pxTCBBufferTemp != NULL);
    // Write back pointers
    *ppxTimerTaskStackBuffer = pxStackBufferTemp;
    *ppxTimerTaskTCBBuffer = pxTCBBufferTemp;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif //( configSUPPORT_STATIC_ALLOCATION == 1 )

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

const DRAM_ATTR uint32_t offset_pxEndOfStack = offsetof(StaticTask_t, pxDummy8);
#if ( configUSE_CORE_AFFINITY == 1 && configNUM_CORES > 1 )
const DRAM_ATTR uint32_t offset_uxCoreAffinityMask = offsetof(StaticTask_t, uxDummy25);
#endif // ( configUSE_CORE_AFFINITY == 1 && configNUM_CORES > 1 )
const DRAM_ATTR uint32_t offset_cpsa = XT_CP_SIZE;

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
    StackType_t *sp, *tp;
    XtExcFrame  *frame;
#if XCHAL_CP_NUM > 0
    uint32_t *p;
#endif
    uint32_t *threadptr;
    void *task_thread_local_start;
    extern int _thread_local_start, _thread_local_end, _flash_rodata_start, _flash_rodata_align;
    // TODO: check that TLS area fits the stack
    uint32_t thread_local_sz = (uint8_t *)&_thread_local_end - (uint8_t *)&_thread_local_start;

    thread_local_sz = ALIGNUP(0x10, thread_local_sz);

    /* Initialize task's stack so that we have the following structure at the top:

        ----LOW ADDRESSES ----------------------------------------HIGH ADDRESSES----------
         task stack | interrupt stack frame | thread local vars | co-processor save area |
        ----------------------------------------------------------------------------------
                    |                                                                     |
                    SP                                                                 pxTopOfStack

        All parts are aligned to 16 byte boundary. */
    sp = (StackType_t *) (((UBaseType_t)pxTopOfStack - XT_CP_SIZE - thread_local_sz - XT_STK_FRMSZ) & ~0xf);

    /* Clear the entire frame (do not use memset() because we don't depend on C library) */
    for (tp = sp; tp <= pxTopOfStack; ++tp) {
        *tp = 0;
    }

    frame = (XtExcFrame *) sp;

    /* Explicitly initialize certain saved registers */
#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
    frame->pc    = (UBaseType_t) vPortTaskWrapper;    /* task wrapper                        */
#else
    frame->pc   = (UBaseType_t) pxCode;                /* task entrypoint                    */
#endif
    frame->a0    = 0;                                /* to terminate GDB backtrace        */
    frame->a1    = (UBaseType_t) sp + XT_STK_FRMSZ;    /* physical top of stack frame        */
    frame->exit = (UBaseType_t) _xt_user_exit;        /* user exception exit dispatcher    */

    /* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
    /* Also set entry point argument parameter. */
#ifdef __XTENSA_CALL0_ABI__
#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
    frame->a2 = (UBaseType_t) pxCode;
    frame->a3 = (UBaseType_t) pvParameters;
#else
    frame->a2 = (UBaseType_t) pvParameters;
#endif
    frame->ps = PS_UM | PS_EXCM;
#else /* __XTENSA_CALL0_ABI__ */
    /* + for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd). */
#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
    frame->a6 = (UBaseType_t) pxCode;
    frame->a7 = (UBaseType_t) pvParameters;
#else
    frame->a6 = (UBaseType_t) pvParameters;
#endif
    frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
#endif /* __XTENSA_CALL0_ABI__ */

#ifdef XT_USE_SWPRI
    /* Set the initial virtual priority mask value to all 1's. */
    frame->vpri = 0xFFFFFFFF;
#endif

    /* Init threadptr register and set up TLS run-time area.
     * The diagram in port/riscv/port.c illustrates the calculations below.
     */
    task_thread_local_start = (void *)(((uint32_t)pxTopOfStack - XT_CP_SIZE - thread_local_sz) & ~0xf);
    memcpy(task_thread_local_start, &_thread_local_start, thread_local_sz);
    threadptr = (uint32_t *)(sp + XT_STK_EXTRA);
    /* Calculate THREADPTR value.
     * The generated code will add THREADPTR value to a constant value determined at link time,
     * to get the address of the TLS variable.
     * The constant value is calculated by the linker as follows
     * (search for 'tpoff' in elf32-xtensa.c in BFD):
     *    offset = address - tls_section_vma + align_up(TCB_SIZE, tls_section_alignment)
     * where TCB_SIZE is hardcoded to 8.
     * Note this is slightly different compared to the RISC-V port, where offset = address - tls_section_vma.
     */
    const uint32_t tls_section_alignment = (uint32_t) &_flash_rodata_align;  /* ALIGN value of .flash.rodata section */
    const uint32_t tcb_size = 8; /* Unrelated to FreeRTOS, this is the constant from BFD */
    const uint32_t base = (tcb_size + tls_section_alignment - 1) & (~(tls_section_alignment - 1));
    *threadptr = (uint32_t)task_thread_local_start - ((uint32_t)&_thread_local_start - (uint32_t)&_flash_rodata_start) - base;

#if XCHAL_CP_NUM > 0
    /* Init the coprocessor save area (see xtensa_context.h) */
    /* No access to TCB here, so derive indirectly. Stack growth is top to bottom.
     * //p = (uint32_t *) xMPUSettings->coproc_area;
     */
    p = (uint32_t *)(((uint32_t) pxTopOfStack - XT_CP_SIZE) & ~0xf);
    configASSERT( ( uint32_t ) p >= frame->a1 );
    p[0] = 0;
    p[1] = 0;
    p[2] = (((uint32_t) p) + 12 + XCHAL_TOTAL_SA_ALIGN - 1) & -XCHAL_TOTAL_SA_ALIGN;
#endif /* XCHAL_CP_NUM */

    configASSERT(((uint32_t) sp & portBYTE_ALIGNMENT_MASK) == 0);
    return sp;
}

// -------------------- Co-Processor -----------------------
#if ( XCHAL_CP_NUM > 0 && configUSE_CORE_AFFINITY == 1 && configNUM_CORES > 1 )

void _xt_coproc_release(volatile void *coproc_sa_base, BaseType_t xCoreID);

void vPortCleanUpCoprocArea( void *pxTCB )
{
    StackType_t *coproc_area;
    BaseType_t xCoreID;

    /* Calculate the coproc save area in the stack from the TCB base */
    coproc_area = ( StackType_t * ) ( ( uint32_t ) ( pxTCB + offset_pxEndOfStack ));
    coproc_area = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) coproc_area ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
    coproc_area = ( StackType_t * ) ( ( ( uint32_t ) coproc_area - XT_CP_SIZE ) & ~0xf );

    /* Extract core ID from the affinity mask */
    xCoreID = __builtin_ffs( * ( UBaseType_t * ) ( pxTCB + offset_uxCoreAffinityMask ) );
    assert( xCoreID >= 1 );
    xCoreID -= 1;

    /* If task has live floating point registers somewhere, release them */
    _xt_coproc_release( coproc_area, xCoreID );
}
#endif // ( XCHAL_CP_NUM > 0 && configUSE_CORE_AFFINITY == 1 && configNUM_CORES > 1 )

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
                // We call EARLY log here as currently portCLEAN_UP_TCB() is called in a critical section
                ESP_EARLY_LOGE("FreeRTOS", "Fatal error: TLSP deletion callback at index %d overwritten with non-excutable pointer %p", x, pvThreadLocalStoragePointersDelCallback[ x ]);
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
    portbenchmarkIntLatency();
    traceISR_ENTER(SYSTICK_INTR_ID);
    BaseType_t ret;
    esp_vApplicationTickHook();
    if (portGET_CORE_ID() == 0) {
        // FreeRTOS SMP requires that only core 0 calls xTaskIncrementTick()
        ret = xTaskIncrementTick();
    } else {
        ret = pdFALSE;
    }
    if (ret != pdFALSE) {
        portYIELD_FROM_ISR();
    } else {
        traceISR_EXIT();
    }
    return ret;
}

// ------------------- Hook Functions ----------------------

#include <stdlib.h>

#if ( configCHECK_FOR_STACK_OVERFLOW > 0 )
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
void vPortCleanUpTCB ( void *pxTCB )
{
#if ( CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS )
    /* Call TLS pointers deletion callbacks */
    vPortTLSPointersDelCb( pxTCB );
#endif /* CONFIG_FREERTOS_TLSP_DELETION_CALLBACKS */

#if ( XCHAL_CP_NUM > 0 && configUSE_CORE_AFFINITY == 1 && configNUM_CORES > 1 )
    /* Cleanup coproc save area */
    vPortCleanUpCoprocArea( pxTCB );
#endif // ( XCHAL_CP_NUM > 0 && configUSE_CORE_AFFINITY == 1 && configNUM_CORES > 1 )
}
