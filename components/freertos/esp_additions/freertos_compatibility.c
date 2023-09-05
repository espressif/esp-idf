/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * FreeRTOS has changed some functions in to macros (and vice-versa) over multiple
 * releases. This is not a breaking API change for source code, but may cause issues
 * for pre-compiled libraries that call these removed APIs.
 *
 * This file maintains these legacy APIs until the next ESP-IDF major release.
 *
 * Todo: Clean up for ESP-IDF v6.0 (IDF-8144)
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

BaseType_t xQueueGenericReceive( QueueHandle_t xQueue,
                                 void * const pvBuffer,
                                 TickType_t xTicksToWait,
                                 const BaseType_t xPeek )
{
    if( xPeek == pdTRUE )
    {
        return xQueuePeek( xQueue, pvBuffer, xTicksToWait );
    }

    if( pvBuffer == NULL )
    {
        return xQueueSemaphoreTake( xQueue, xTicksToWait );
    }

    return xQueueReceive( xQueue, pvBuffer, xTicksToWait );
}

/*
 * vTaskDelayUntil() was deprecated into a macro and replaced by xTaskDelayUntil().
 * This is added for pre-compiled libraries that depend on ulTaskNotifyTake()
 * being a function.
 *
 * Todo: Remove this in v6.0 (IDF-3851)
 */
#undef vTaskDelayUntil
void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime,
                      const TickType_t xTimeIncrement )
{
    xTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );
}

/*
 * ulTaskNotifyTake() was turned into a macro. This is added for pre-compiled
 * libraries that depend on ulTaskNotifyTake() being a function.
 *
 * Todo: Remove this in v6.0 (IDF-3851)
 */
#undef ulTaskNotifyTake
uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit,
                           TickType_t xTicksToWait )
{
    return ulTaskGenericNotifyTake( tskDEFAULT_INDEX_TO_NOTIFY, xClearCountOnExit, xTicksToWait );
}

/*
 * xTaskNotifyWait() was turned into a macro. This is added for pre-compiled
 * libraries that depend on xTaskNotifyWait() being a function.
 *
 * Todo: Remove this in v6.0 (IDF-3851)
 */
#undef xTaskNotifyWait
BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry,
                            uint32_t ulBitsToClearOnExit,
                            uint32_t * pulNotificationValue,
                            TickType_t xTicksToWait )
{
    return xTaskGenericNotifyWait( tskDEFAULT_INDEX_TO_NOTIFY, ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait );
}
