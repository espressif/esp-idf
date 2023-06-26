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

// ------------- FreeRTOS Static Allocation ----------------

/*
These function are required by FreeRTOS when configSUPPORT_STATIC_ALLOCATION is
enabled and is used by FreeRTOS to obtain memory for its IDLE/Timer tasks.

We simply allocate the IDLE/Timer tasks memory from the FreeRTOS heap.
*/
#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize )
{
    StaticTask_t *pxTCBBufferTemp;
    StackType_t *pxStackBufferTemp;

    /* Allocate TCB and stack buffer from the FreeRTOS heap
     *
     * If the stack grows down then allocate the stack then the TCB so the stack
     * does not grow into the TCB.  Likewise if the stack grows up then allocate
     * the TCB then the stack. */
    #if (portSTACK_GROWTH > 0)
    {
        pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
        pxStackBufferTemp = pvPortMalloc(configMINIMAL_STACK_SIZE);
    }
    #else /* portSTACK_GROWTH */
    {
        pxStackBufferTemp = pvPortMalloc(configMINIMAL_STACK_SIZE);
        pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
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

    /* Allocate TCB and stack buffer from the FreeRTOS heap
     *
     * If the stack grows down then allocate the stack then the TCB so the stack
     * does not grow into the TCB.  Likewise if the stack grows up then allocate
     * the TCB then the stack. */
    #if (portSTACK_GROWTH > 0)
    {
        pxTCBBufferTemp = pvPortMalloc(sizeof(StaticTask_t));
        pxStackBufferTemp = pvPortMalloc(configTIMER_TASK_STACK_DEPTH);
    }
    #else /* portSTACK_GROWTH */
    {
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
#endif // configSUPPORT_STATIC_ALLOCATION == 1
