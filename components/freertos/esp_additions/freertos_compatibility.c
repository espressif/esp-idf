/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
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

BaseType_t xQueueGenericReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xPeek )
{
	if ( xPeek == pdTRUE )
	{
		return xQueuePeek( xQueue, pvBuffer, xTicksToWait );
	}

	if ( pvBuffer == NULL )
	{
		return xQueueSemaphoreTake( xQueue, xTicksToWait );
	}

	return xQueueReceive( xQueue, pvBuffer, xTicksToWait );
}
