/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_debug_helpers.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*shared_stack_function)(void);

#define ESP_EXECUTE_EXPRESSION_WITH_STACK(lock, stack, stack_size, expression) \
    esp_execute_shared_stack_function(lock, stack, stack_size, expression)

/**
 * @brief Calls user defined shared stack space function
 * @param lock Mutex object to protect in case of shared stack
 * @param stack Pointer to user alocated stack
 * @param stack_size Size of current stack in bytes
 * @param function pointer to the shared stack function to be executed
 * @note  if either lock, stack or stack size is invalid, the expression will
 *          be called using the current stack.
 */
void esp_execute_shared_stack_function(SemaphoreHandle_t lock,
                                      void *stack,
                                      size_t stack_size,
                                      shared_stack_function function);


#ifdef __cplusplus
}
#endif
