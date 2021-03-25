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

#include <esp_expression_with_stack.h>
#include <riscv/rvruntime-frames.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

static portMUX_TYPE shared_stack_spinlock = portMUX_INITIALIZER_UNLOCKED;
static void *current_task_stack = NULL;

extern void esp_shared_stack_invoke_function(shared_stack_function function, void *stack);

static StackType_t *esp_switch_stack_setup(StackType_t *stack, size_t stack_size)
{
    //We need also to tweak current task stackpointer to avoid erroneous
    //stack overflow indication, so fills the stack with freertos known pattern:
    memset(stack, 0xa5U, stack_size * sizeof(StackType_t));

    StaticTask_t *current = (StaticTask_t *)xTaskGetCurrentTaskHandle();
    //Then put the fake stack inside of TCB:
    current_task_stack = current->pxDummy6;
    current->pxDummy6 = (void *)stack;

    StackType_t *top_of_stack = stack + stack_size;

    //Align stack to a 16byte boundary, as required by CPU specific:
    top_of_stack =  (StackType_t *)(((UBaseType_t)(top_of_stack - 16) & ~0xf));
    StackType_t *adjusted_top_of_stack = top_of_stack - RV_STK_FRMSZ;

#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
    vPortSetStackWatchpoint(stack);
#endif
    return ((StackType_t *)adjusted_top_of_stack);
}


void esp_execute_shared_stack_function(SemaphoreHandle_t lock, void *stack, size_t stack_size, shared_stack_function function)
{
    assert(lock);
    assert(stack);
    assert(stack_size > 0 && stack_size >= CONFIG_ESP_MINIMAL_SHARED_STACK_SIZE);
    assert(function);

    xSemaphoreTake(lock, portMAX_DELAY);
    portENTER_CRITICAL(&shared_stack_spinlock);
    stack = esp_switch_stack_setup(stack, stack_size);
    portEXIT_CRITICAL(&shared_stack_spinlock);

    esp_shared_stack_invoke_function(function, stack);

    portENTER_CRITICAL(&shared_stack_spinlock);
    StaticTask_t *current = (StaticTask_t *)xTaskGetCurrentTaskHandle();

    //Restore current task stack:
    current->pxDummy6 = (StackType_t *)current_task_stack;
    vPortSetStackWatchpoint(current->pxDummy6);
    portEXIT_CRITICAL(&shared_stack_spinlock);

    xSemaphoreGive(lock);
}
