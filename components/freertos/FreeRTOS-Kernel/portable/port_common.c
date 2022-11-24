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

// -------------------- Heap Related -----------------------

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

// ------------- FreeRTOS Static Allocation ----------------

/*
This function is required by FreeRTOS when configSUPPORT_STATIC_ALLOCATION is
enabled and is used by FreeRTOS to obtain memory for its IDLE tasks.

Like the pvPortMallocTcbMem() and pvPortMallocStackMem() macros, TCB and stack
memory MUST be placed in internal RAM.
*/
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
        pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
        pxStackBufferTemp = pvPortMallocStackMem(configIDLE_TASK_STACK_SIZE);
    }
    #else /* portSTACK_GROWTH */
    {
        //Allocate TCB and stack buffer in internal memory
        pxStackBufferTemp = pvPortMallocStackMem(configIDLE_TASK_STACK_SIZE);
        pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
    }
    #endif /* portSTACK_GROWTH */

    assert(pxTCBBufferTemp != NULL);
    assert(pxStackBufferTemp != NULL);
    //Write back pointers
    *ppxIdleTaskTCBBuffer = pxTCBBufferTemp;
    *ppxIdleTaskStackBuffer = pxStackBufferTemp;
    *pulIdleTaskStackSize = configIDLE_TASK_STACK_SIZE;
}

/*
This function is required by FreeRTOS when configSUPPORT_STATIC_ALLOCATION is
enabled and is used by the FreeRTOS Timer to obtain memory for its daemone task.


Like the pvPortMallocTcbMem() and pvPortMallocStackMem() macros, TCB and stack
memory MUST be placed in internal RAM.
*/
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
        pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
        pxStackBufferTemp = pvPortMallocStackMem(configTIMER_TASK_STACK_DEPTH);
    }
    #else /* portSTACK_GROWTH */
    {
        //Allocate TCB and stack buffer in internal memory
        pxStackBufferTemp = pvPortMallocStackMem(configTIMER_TASK_STACK_DEPTH);
        pxTCBBufferTemp = pvPortMallocTcbMem(sizeof(StaticTask_t));
    }
    #endif /* portSTACK_GROWTH */

    assert(pxTCBBufferTemp != NULL);
    assert(pxStackBufferTemp != NULL);
    //Write back pointers
    *ppxTimerTaskTCBBuffer = pxTCBBufferTemp;
    *ppxTimerTaskStackBuffer = pxStackBufferTemp;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
