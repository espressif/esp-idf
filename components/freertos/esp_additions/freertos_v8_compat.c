/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* This API is kept for backward ABI compatibility with prebuilt libraries against FreeRTOS v8/v9 in ESP-IDF */
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
