/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_expression_with_stack.h>
#include <riscv/rvruntime-frames.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "sdkconfig.h"
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif

static StackType_t *esp_shared_stack_setup_context(StaticTask_t *tcb, void **sp_min, void **sp_max, StackType_t *stack, size_t stack_size)
{
    //We need also to tweak current task stackpointer to avoid erroneous
    //stack overflow indication, so fills the stack with freertos known pattern:
    memset(stack, 0xa5U, stack_size * sizeof(StackType_t));

    //Align stack to a 16-byte boundary, as required by CPU specific:
    StackType_t *top_of_stack = (StackType_t *) ALIGNUP(0x10, (uint32_t)(stack + stack_size));
    StackType_t *adjusted_top_of_stack = top_of_stack - RV_STK_FRMSZ;

    //Then put the fake stack inside of TCB:
    *sp_min = tcb->pxDummy6;
    tcb->pxDummy6 = (void *)stack;

    *sp_max = tcb->pxDummy8;
    tcb->pxDummy8 = adjusted_top_of_stack;

#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
    vPortSetStackWatchpoint(stack);
#endif
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    esp_hw_stack_guard_monitor_stop();
    esp_hw_stack_guard_set_bounds((uint32_t) tcb->pxDummy6, (uint32_t) tcb->pxDummy8);
#endif
    return ((StackType_t *)adjusted_top_of_stack);
}

static void esp_shared_stack_restore_context(StaticTask_t *tcb, void *sp_min, void *sp_max)
{
    tcb->pxDummy6 = sp_min;
    tcb->pxDummy8 = sp_max;

#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
    vPortSetStackWatchpoint(sp_min);
#endif
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    esp_hw_stack_guard_set_bounds((uint32_t) sp_min, (uint32_t) sp_max);
    esp_hw_stack_guard_monitor_start();
#endif
}

void esp_execute_shared_stack_function(SemaphoreHandle_t lock, void *stack, size_t stack_size, shared_stack_function function)
{
    assert(lock);
    assert(stack);
    assert(stack_size > 0 && stack_size >= CONFIG_ESP_MINIMAL_SHARED_STACK_SIZE);
    assert(function);

    static portMUX_TYPE shared_stack_spinlock = portMUX_INITIALIZER_UNLOCKED;
    StaticTask_t *tcb = (StaticTask_t *)xTaskGetCurrentTaskHandle();
    void *sp_min = NULL;
    void *sp_max = (void *) UINTPTR_MAX;

    xSemaphoreTake(lock, portMAX_DELAY);

    portENTER_CRITICAL(&shared_stack_spinlock);
    stack = esp_shared_stack_setup_context(tcb, &sp_min, &sp_max, stack, stack_size);
    __asm__ volatile("mv   t0, sp      \n"      /* save current SP */
                     "mv   sp, %0      \n"     /* set shared stack as new SP */
                     "addi sp, sp, -16 \n"     /* allocate memory for previous SP */
                     "sw   t0, 0(sp)   \n"     /* store previous SP in a safe place */
                     :: "r"(stack));
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    esp_hw_stack_guard_monitor_start();
#endif
    portEXIT_CRITICAL(&shared_stack_spinlock);

    function();

    portENTER_CRITICAL(&shared_stack_spinlock);
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    esp_hw_stack_guard_monitor_stop();
#endif
    __asm__ volatile("lw   sp, 0(sp)");         /* restore real SP of current task */
    esp_shared_stack_restore_context(tcb, sp_min, sp_max);
    portEXIT_CRITICAL(&shared_stack_spinlock);

    xSemaphoreGive(lock);
}
