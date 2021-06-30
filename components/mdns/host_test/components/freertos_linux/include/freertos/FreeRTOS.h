// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define portTICK_PERIOD_MS 1
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

typedef void * xSemaphoreHandle;
typedef void * SemaphoreHandle_t;
typedef void * xQueueHandle;
typedef void * QueueHandle_t;
typedef void * TaskHandle_t;
typedef uint32_t TickType_t;
typedef uint32_t portTickType;

typedef void (*TaskFunction_t)( void * );
typedef unsigned int	UBaseType_t;
typedef int 			BaseType_t;

#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )

#define portTICK_RATE_MS portTICK_PERIOD_MS
#define pdMS_TO_TICKS(tick)    (tick)

uint32_t esp_get_free_heap_size(void);
uint32_t esp_random(void);
