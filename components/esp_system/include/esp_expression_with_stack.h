// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
