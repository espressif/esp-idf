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
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "esp_heap_caps_init.h"
#include "esp_private/startup_internal.h"   /* Required by g_spiram_ok. [refactor-todo] for g_spiram_ok */
#include "esp32/spiram.h"                   /* Required by esp_spiram_reserve_dma_pool() */
#ifdef CONFIG_APPTRACE_ENABLE
#include "esp_app_trace.h"
#endif
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
#include "esp_gdbstub.h"                    /* Required by esp_gdbstub_init() */
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

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

extern void _frxt_tick_timer_init(void);
extern void _xt_tick_divisor_init(void);

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

// --------------------- App Start-up ----------------------

static const char *TAG = "cpu_start";

extern void app_main(void);

static void main_task(void* args)
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
    if (g_spiram_ok) {
        esp_err_t r = esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
        if (r != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Could not reserve internal/DMA pool (error 0x%x)", r);
            abort();
        }
    }
#endif

    //Initialize task wdt if configured to do so
#ifdef CONFIG_ESP_TASK_WDT_PANIC
    ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, true));
#elif CONFIG_ESP_TASK_WDT
    ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, false));
#endif

    //Add IDLE 0 to task wdt
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    if(idle_0 != NULL){
        ESP_ERROR_CHECK(esp_task_wdt_add(idle_0));
    }
#endif
    //Add IDLE 1 to task wdt
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
    TaskHandle_t idle_1 = xTaskGetIdleTaskHandleForCPU(1);
    if(idle_1 != NULL){
        ESP_ERROR_CHECK(esp_task_wdt_add(idle_1));
    }
#endif

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

    TaskHandle_t main_task_hdl;
    portDISABLE_INTERRUPTS();
    portBASE_TYPE res = xTaskCreatePinnedToCore(main_task, "main",
                                                ESP_TASK_MAIN_STACK, NULL,
                                                ESP_TASK_MAIN_PRIO, &main_task_hdl, ESP_TASK_MAIN_CORE);
#if ( configUSE_CORE_AFFINITY == 1 && configNUM_CORES > 1 )
    //We only need to set affinity when using dual core with affinities supported
    vTaskCoreAffinitySet(main_task_hdl, 1 << 1);
#endif
    portENABLE_INTERRUPTS();
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

void vPortFree( void * pv )
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
    StaticTask_t *pxTCBBufferTemp;
    StackType_t *pxStackBufferTemp;
    //Allocate TCB and stack buffer in internal memory
    pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
    pxStackBufferTemp = pvPortMalloc(CONFIG_FREERTOS_IDLE_TASK_STACKSIZE);
    assert(pxTCBBufferTemp != NULL);
    assert(pxStackBufferTemp != NULL);
    //Write back pointers
    *ppxIdleTaskTCBBuffer = pxTCBBufferTemp;
    *ppxIdleTaskStackBuffer = pxStackBufferTemp;
    *pulIdleTaskStackSize = CONFIG_FREERTOS_IDLE_TASK_STACKSIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize )
{
    StaticTask_t *pxTCBBufferTemp;
    StackType_t *pxStackBufferTemp;
    //Allocate TCB and stack buffer in internal memory
    pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
    pxStackBufferTemp = pvPortMalloc(configTIMER_TASK_STACK_DEPTH);
    assert(pxTCBBufferTemp != NULL);
    assert(pxStackBufferTemp != NULL);
    //Write back pointers
    *ppxTimerTaskTCBBuffer = pxTCBBufferTemp;
    *ppxTimerTaskStackBuffer = pxStackBufferTemp;
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

#if ( portHAS_STACK_OVERFLOW_CHECKING == 1 )
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                        StackType_t * pxEndOfStack,
                                        TaskFunction_t pxCode,
                                        void * pvParameters,
                                        BaseType_t xRunPrivileged )
#else
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters,
                                     BaseType_t xRunPrivileged )
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

    return sp;
}

// -------------------- Tick Handler -----------------------

extern void esp_vApplicationIdleHook(void);
extern void esp_vApplicationTickHook(void);

BaseType_t xPortSysTickHandler(void)
{
    portbenchmarkIntLatency();
    traceISR_ENTER(SYSTICK_INTR_ID);
    BaseType_t ret;
    if (portGET_CORE_ID() == 0) {
        //Only Core 0 calls xTaskIncrementTick();
        ret = xTaskIncrementTick();
    } else {
        //Manually call the IDF tick hooks
        esp_vApplicationTickHook();
        ret = pdFALSE;
    }
    if(ret != pdFALSE) {
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

#if  (  configUSE_TICK_HOOK > 0 )
void vApplicationTickHook( void )
{
    esp_vApplicationTickHook();
}
#endif

#if ( configUSE_IDLE_HOOK == 1 )
void vApplicationIdleHook( void )
{
    esp_vApplicationIdleHook();
}
#endif

#if ( configUSE_MINIMAL_IDLE_HOOK == 1 )
void vApplicationMinimalIdleHook( void )
{
    esp_vApplicationIdleHook();
}
#endif

/* ---------------------------------------------- Misc Implementations -------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// -------------------- Co-Processor -----------------------

/*
 * Used to set coprocessor area in stack. Current hack is to reuse MPU pointer for coprocessor area.
 */
#if portUSING_MPU_WRAPPERS
void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION *const xRegions, StackType_t *pxBottomOfStack, uint32_t usStackDepth )
{
#if XCHAL_CP_NUM > 0
    xMPUSettings->coproc_area = ( StackType_t * ) ( ( uint32_t ) ( pxBottomOfStack + usStackDepth - 1 ));
    xMPUSettings->coproc_area = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) xMPUSettings->coproc_area ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
    xMPUSettings->coproc_area = ( StackType_t * ) ( ( ( uint32_t ) xMPUSettings->coproc_area - XT_CP_SIZE ) & ~0xf );


    /* NOTE: we cannot initialize the coprocessor save area here because FreeRTOS is going to
     * clear the stack area after we return. This is done in pxPortInitialiseStack().
     */
#endif
}

void vPortReleaseTaskMPUSettings( xMPU_SETTINGS *xMPUSettings )
{
    /* If task has live floating point registers somewhere, release them */
    _xt_coproc_release( xMPUSettings->coproc_area );
}
#endif /* portUSING_MPU_WRAPPERS */
