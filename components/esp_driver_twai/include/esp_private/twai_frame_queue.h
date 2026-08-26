/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_twai_types.h"

////////////////////////////////////////////////////////////////////
//        !! This queue is ONLY for TWAI driver internal use      //
////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

typedef struct twai_frame_queue_s *twai_frame_queue_t;

/**
 * @brief Initialize a TWAI frame priority queue.
 *
 * Items are ordered by priority first. For equal priority values, items are popped in push order.
 */
esp_err_t twai_frame_queue_new(twai_frame_queue_t *queue, size_t capacity, uint32_t mem_caps);

/**
 * @brief Release resources used by a TWAI frame priority queue.
 *
 * @return
 *      - ESP_OK: Queue was released successfully
 *      - ESP_ERR_INVALID_ARG: Queue is invalid
 */
esp_err_t twai_frame_queue_del(twai_frame_queue_t queue);

/**
 * @brief Push an item with priority from task context.
 *
 * @return
 *      - ESP_OK: Item was queued successfully
 *      - ESP_ERR_INVALID_ARG: Queue is invalid
 *      - ESP_ERR_TIMEOUT: No free slot became available before timeout
 */
esp_err_t twai_frame_queue_push(twai_frame_queue_t queue, const twai_frame_t *data, uint32_t priority, int timeout_ms);

/**
 * @brief Push an item with priority from ISR context.
 *
 * @return
 *      - ESP_OK: Item was queued successfully
 *      - ESP_ERR_INVALID_ARG: Queue is invalid
 *      - ESP_ERR_TIMEOUT: Queue is full
 */
esp_err_t twai_frame_queue_push_from_isr(twai_frame_queue_t queue, const twai_frame_t *data, uint32_t priority, bool *task_woken);

/**
 * @brief Push an item with priority from task or ISR context.
 *
 * @return
 *      - ESP_OK: Item was queued successfully
 *      - ESP_ERR_INVALID_ARG: Queue is invalid
 *      - ESP_ERR_TIMEOUT: Queue is full or no free slot became available before timeout
 */
esp_err_t twai_frame_queue_push_safe(twai_frame_queue_t queue, const twai_frame_t *data, uint32_t priority, int timeout_ms);

/**
 * @brief Pop the highest-priority item from task context.
 *
 * @return
 *      - ESP_OK: Item was popped successfully
 *      - ESP_ERR_INVALID_ARG: Queue is invalid
 *      - ESP_ERR_NOT_FOUND: Queue is empty
 */
esp_err_t twai_frame_queue_pop(twai_frame_queue_t queue, const twai_frame_t **data);

/**
 * @brief Pop the highest-priority item from ISR context.
 *
 * @return
 *      - ESP_OK: Item was popped successfully
 *      - ESP_ERR_INVALID_ARG: Queue is invalid
 *      - ESP_ERR_NOT_FOUND: Queue is empty
 */
esp_err_t twai_frame_queue_pop_from_isr(twai_frame_queue_t queue, const twai_frame_t **data, bool *task_woken);

/**
 * @brief Pop the highest-priority item from task or ISR context.
 *
 * @return
 *      - ESP_OK: Item was popped successfully
 *      - ESP_ERR_INVALID_ARG: Queue is invalid
 *      - ESP_ERR_NOT_FOUND: Queue is empty
 */
esp_err_t twai_frame_queue_pop_safe(twai_frame_queue_t queue, const twai_frame_t **data);

/**
 * @brief Get the current number of queued items.
 */
size_t twai_frame_queue_get_count(twai_frame_queue_t queue);

/**
 * @brief Get the number of available queue slots.
 */
size_t twai_frame_queue_get_free_space(twai_frame_queue_t queue);

#ifdef __cplusplus
}
#endif
