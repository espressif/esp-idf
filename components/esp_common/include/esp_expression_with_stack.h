#ifndef __ESP_EXPRESSION_WITH_STACK_H
#define __ESP_EXPRESSION_WITH_STACK_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @brief Executes a 1-line expression with a application alocated stack
 * @param lock Mutex object to protect in case of shared stack
 * @param stack Pointer to user alocated stack, it must points to its top
 * @param expression Expression or function to be executed using the stack
 */
#define ESP_EXECUTE_EXPRESSION_WITH_STACK(lock, stack, expression)  \
({                                                                  \
    if(lock) {                                                      \
        uint32_t backup;                                            \
        xSemaphoreTake(lock, portMAX_DELAY);                        \
        esp_switch_stack_enter(stack, &backup);                     \
        {                                                           \
            expression;                                             \
        }                                                           \
        esp_switch_stack_exit(&backup);                             \
        xSemaphoreGive(lock);                                       \
    }                                                               \
})

/**
 * These functions are intended to be use by the macro above, and
 * Should never be called directly, otherwise crashes could
 * occur
 */
extern void esp_switch_stack_enter(portSTACK_TYPE *stack, uint32_t *backup_stack);
extern void esp_switch_stack_exit(uint32_t *backup_stack);

#endif