// Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
