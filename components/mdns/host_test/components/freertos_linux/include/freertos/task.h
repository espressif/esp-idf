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

#include "freertos/FreeRTOS.h"

#define xTaskHandle TaskHandle_t
#define vSemaphoreDelete( xSemaphore ) vQueueDelete( ( QueueHandle_t ) ( xSemaphore ) )

void vTaskDelay( const TickType_t xTicksToDelay );

void xTaskNotifyGive(TaskHandle_t task);

TaskHandle_t xTaskGetCurrentTaskHandle(void);

BaseType_t xTaskNotifyWait(uint32_t bits_entry_clear, uint32_t bits_exit_clear, uint32_t *value, TickType_t wait_time );

BaseType_t xTaskCreatePinnedToCore(	TaskFunction_t pvTaskCode,
                                       const char * const pcName,
                                       const uint32_t usStackDepth,
                                       void * const pvParameters,
                                       UBaseType_t uxPriority,
                                       TaskHandle_t * const pvCreatedTask,
                                       const BaseType_t xCoreID);

void xTaskCreate(TaskFunction_t pvTaskCode, const char * const pcName, const uint32_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pvCreatedTask);

TickType_t xTaskGetTickCount( void );

void vQueueDelete( QueueHandle_t xQueue );

QueueHandle_t xSemaphoreCreateBinary(void);

QueueHandle_t xSemaphoreCreateMutex(void);

BaseType_t xSemaphoreGive( QueueHandle_t xQueue);

BaseType_t xSemaphoreTake( QueueHandle_t xQueue, TickType_t pvTask );

void vTaskDelete(TaskHandle_t *task);

QueueHandle_t xQueueCreate( uint32_t uxQueueLength,
                            uint32_t uxItemSize );

uint32_t xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait);

uint32_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);
