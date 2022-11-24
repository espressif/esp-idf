/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "esp_system.h"
#include "esp_memory_utils.h"
#include "sdkconfig.h"

/* ----------------------------------------- Port Implementations (Common)  --------------------------------------------
 * - Common FreeRTOS port function implementations
 * - These functions are common to all FreeRTOS ports (i.e., on all architectures and all FreeRTOS implementations).
 * ------------------------------------------------------------------------------------------------------------------ */

// -------------------- Heap Related -----------------------

#if !CONFIG_FREERTOS_SMP    // IDF-3997
bool xPortCheckValidTCBMem(const void *ptr)
{
    return esp_ptr_internal(ptr) && esp_ptr_byte_accessible(ptr);
}

bool xPortcheckValidStackMem(const void *ptr)
{
#ifdef CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
    return esp_ptr_byte_accessible(ptr);
#else
    return esp_ptr_internal(ptr) && esp_ptr_byte_accessible(ptr);
#endif
}
#endif

// ------------- FreeRTOS Static Allocation ----------------

/*
These function are required by FreeRTOS when configSUPPORT_STATIC_ALLOCATION is
enabled and is used by FreeRTOS to obtain memory for its IDLE/Timer tasks.

Like the pvPortMallocTcbMem() and pvPortMallocStackMem() macros, TCB and stack
memory MUST be placed in internal RAM.
*/
#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize )
{
    StaticTask_t *pxTCBBufferTemp;
    StackType_t *pxStackBufferTemp;

    /* If the stack grows down then allocate the stack then the TCB so the stack
     * does not grow into the TCB.  Likewise if the stack grows up then allocate
     * the TCB then the stack. */
    #if (portSTACK_GROWTH > 0)
    {
        //Allocate TCB and stack buffer in internal memory
        #if CONFIG_FREERTOS_SMP     // IDF-3997
            pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
            pxStackBufferTemp = pvPortMalloc(configMINIMAL_STACK_SIZE);
        #else
            pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
            pxStackBufferTemp = pvPortMallocStackMem(configMINIMAL_STACK_SIZE);
        #endif /* CONFIG_FREERTOS_SMP */
    }
    #else /* portSTACK_GROWTH */
    {
        //Allocate TCB and stack buffer in internal memory
        #if CONFIG_FREERTOS_SMP     // IDF-3997
            pxStackBufferTemp = pvPortMalloc(configMINIMAL_STACK_SIZE);
            pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
        #else
            pxStackBufferTemp = pvPortMallocStackMem(configMINIMAL_STACK_SIZE);
            pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
        #endif /* CONFIG_FREERTOS_SMP */
    }
    #endif /* portSTACK_GROWTH */

    assert(pxTCBBufferTemp != NULL);
    assert(pxStackBufferTemp != NULL);
    //Write back pointers
    *ppxIdleTaskTCBBuffer = pxTCBBufferTemp;
    *ppxIdleTaskStackBuffer = pxStackBufferTemp;
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
        #if CONFIG_FREERTOS_SMP     // IDF-3997
            pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
            pxStackBufferTemp = pvPortMalloc(configTIMER_TASK_STACK_DEPTH);
        #else
            pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
            pxStackBufferTemp = pvPortMallocStackMem(configTIMER_TASK_STACK_DEPTH);
        #endif /* CONFIG_FREERTOS_SMP */
    }
    #else /* portSTACK_GROWTH */
    {
        //Allocate TCB and stack buffer in internal memory
        #if CONFIG_FREERTOS_SMP     // IDF-3997
            pxStackBufferTemp = pvPortMalloc(configTIMER_TASK_STACK_DEPTH);
            pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
        #else
            pxStackBufferTemp = pvPortMallocStackMem(configTIMER_TASK_STACK_DEPTH);
            pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
        #endif /* CONFIG_FREERTOS_SMP */
    }
    #endif /* portSTACK_GROWTH */

    assert(pxTCBBufferTemp != NULL);
    assert(pxStackBufferTemp != NULL);
    //Write back pointers
    *ppxTimerTaskTCBBuffer = pxTCBBufferTemp;
    *ppxTimerTaskStackBuffer = pxStackBufferTemp;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif // configSUPPORT_STATIC_ALLOCATION == 1
