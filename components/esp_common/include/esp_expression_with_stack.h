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

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_debug_helpers.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Executes a 1-line expression with a application alocated stack
 * @param lock Mutex object to protect in case of shared stack
 * @param stack Pointer to user alocated stack
 * @param stack_size Size of current stack in bytes
 * @param expression Expression or function to be executed using the stack
 * @note  if either lock, stack or stack size is invalid, the expression will
 *          be called using the current stack.
 */
#define ESP_EXECUTE_EXPRESSION_WITH_STACK(lock, stack, stack_size, expression)      \
({                                                                                  \
    assert(lock && stack && (stack_size >= CONFIG_ESP_MINIMAL_SHARED_STACK_SIZE));  \
    uint32_t backup;                                                                \
    xSemaphoreTake(lock, portMAX_DELAY);                                            \
    StackType_t *top_of_stack = esp_switch_stack_setup(stack, stack_size);          \
    esp_switch_stack_enter(top_of_stack, &backup);                                  \
    {                                                                               \
        expression;                                                                 \
    }                                                                               \
    esp_switch_stack_exit(&backup);                                                 \
    xSemaphoreGive(lock);                                                           \
})

/**
 * @brief Fill stack frame with CPU-specifics value before use
 * @param stack Caller allocated stack pointer
 * @param stack_size Size of stack in bytes
 * @return New pointer to the top of stack
 * @note Application must not call this function directly
 */
StackType_t * esp_switch_stack_setup(StackType_t *stack, size_t stack_size);

/**
 * @brief Changes CPU sp-register to use another stack space and save the previous one
 * @param stack Caller allocated stack pointer
 * @param backup_stack Pointer to a place to save the current stack
 * @note Application must not call this function directly
 */
extern void esp_switch_stack_enter(StackType_t *stack, uint32_t *backup_stack);

/**
 * @brief Restores the previous CPU sp-register
 * @param backup_stack Pointer to the place where stack was saved
 * @note Application must not call this function directly
 */
extern void esp_switch_stack_exit(uint32_t *backup_stack);

#ifdef __cplusplus
}
#endif