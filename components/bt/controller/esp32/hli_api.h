/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BTDM_CTRL_HLI

/*** Queues ***/

struct hli_queue_t
{
    size_t elem_size;
    char* begin;
    char* end;
    const char* bufend;
    QueueHandle_t downstream;
    int flags;
    char buf[0];
};

/**
 * @brief Register a high level interrupt function
 *
 * @param handler  interrupt handler function
 * @param arg      argument to pass to the interrupt handler
 * @param intr_reg   address of the peripheral register containing the interrupt status,
 *                   or value 0 to get the status from CPU INTERRUPT register
 * @param intr_mask  mask of the interrupt, in the interrupt status register
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if too many handlers are registered
 */
esp_err_t hli_intr_register(intr_handler_t handler, void* arg, uint32_t intr_reg, uint32_t intr_mask);

/**
 * @brief Mask all interrupts (including high level ones) on the current CPU
 *
 * @return uint32_t interrupt status, pass it to hli_intr_restore
 */
uint32_t hli_intr_disable(void);

/**
 * @brief Re-enable interrupts
 *
 * @param state value returned by hli_intr_disable
 */
void hli_intr_restore(uint32_t state);

/**
 * @brief Type of a hli queue
 */
typedef struct hli_queue_t* hli_queue_handle_t;

/**
 * @brief Initialize hli_queue module. Must be called once before using hli queue APIs.
 */
void hli_queue_setup(void);

/**
 * @brief Shutdown hli_queue module.
 */
void hli_queue_shutdown(void);

/**
 * @brief Create a hli queue, wrapping a FreeRTOS queue
 *
 * This queue can be used from high level interrupts,
 * but **ONLY ON THE CPU WHERE hli_queue_setup WAS CALLED**. Values sent to this
 * queue are automatically forwarded to "downstream" FreeRTOS queue using a level 3
 * software interrupt.
 *
 * @param nelem  number of elements in the queue
 * @param elem_size  size of one element; must match element size of a downstream queue
 * @param downstream  FreeRTOS queue to send the values to
 * @return hli_queue_handle_t  handle of the created queue, or NULL on failure
 */
hli_queue_handle_t hli_queue_create(size_t nelem, size_t elem_size, QueueHandle_t downstream);

/**
 * @brief Create a customer hli queue, wrapping a FreeRTOS queue
 *
 * This queue can be used from high level interrupts,
 * but **ONLY ON THE CPU WHERE hli_queue_setup WAS CALLED**. Values sent to this
 * queue are automatically forwarded to "downstream" FreeRTOS queue using a level 3
 * software interrupt.
 *
 * @param nelem  number of elements in the queue
 * @param elem_size  size of one element; must match element size of a downstream queue
 * @param downstream  FreeRTOS queue to send the values to
 * @return hli_queue_handle_t  handle of the created queue, or NULL on failure
 */
hli_queue_handle_t hli_customer_queue_create(size_t nelem, size_t elem_size, QueueHandle_t downstream);

/**
 * @brief Create a hli queue, wrapping a FreeRTOS semaphore
 *
 * See notes on hli_queue_create.
 *
 * @param max_count  maximum semaphore count
 * @param downstream  FreeRTOS semaphore to forward the calls to
 * @return hli_queue_handle_t  handle of the created queue, or NULL on failure
 */
hli_queue_handle_t hli_semaphore_create(size_t max_count, SemaphoreHandle_t downstream);

/**
 * @brief Delete a hli queue
 *
 * Make sure noone is using the queue before deleting it.
 *
 * @param queue  handle returned by hli_queue_create or hli_semaphore_create
 */
void hli_queue_delete(hli_queue_handle_t queue);

/**
 * @brief Get one element from a hli queue
 *
 * Usually not used, values get sent to a downstream FreeRTOS queue automatically.
 * However if downstream queue is NULL, this API can be used to get values from a hli queue.
 *
 * @param queue  handle of a queue
 * @param out  pointer where to store the element
 * @return true if the element was successfully read from the queue
 */
bool hli_queue_get(hli_queue_handle_t queue, void* out);

/**
 * @brief Put one element into a hli queue
 *
 * This puts copies an element into the queue and raises a software interrupt (level 3).
 * In the interrupt, the value is copied to a FreeRTOS "downstream" queue.
 *
 * Note that if the value does not fit into a downstream queue, no error is returned,
 * and the value is lost.
 *
 * @param queue handle of a queue
 * @param data  pointer to the element to be sent
 * @return true if data was placed into the hli queue successfully
 */
bool hli_queue_put(hli_queue_handle_t queue, const void* data);

/**
 * @brief "Give" a semaphore wrapped by a hli queue
 *
 * @param queue  handle returned by hli_semaphore_create
 * @return true  if the event was sent to a hli queue successfully
 */
bool hli_semaphore_give(hli_queue_handle_t queue);

#endif /* CONFIG_BTDM_CTRL_HLI */

#ifdef __cplusplus
}
#endif
