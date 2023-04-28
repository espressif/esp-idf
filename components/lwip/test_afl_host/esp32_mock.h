/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP32_MOCK_H_
#define _ESP32_MOCK_H_

#include <stdint.h>
#include <unistd.h>

/* ------------------------------------------------- ESP32 Port Mock ---------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- lwipopts.h ------------------------

#define ESP_TASK_TCPIP_STACK
#define ESP_TASK_TCPIP_PRIO

uint32_t esp_random(void);

// --------------------- sys_arch.h ------------------------

// Required to get linux assert.h to work ???
#define __ASSERT_FUNC __ASSERT_FUNCTION

typedef void * SemaphoreHandle_t;
typedef void * TaskHandle_t;
typedef void * QueueHandle_t;

#define vTaskDelay(ms)      usleep((m)*0)

#endif // _ESP32_MOCK_H_
