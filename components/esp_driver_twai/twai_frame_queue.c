/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_private/twai_frame_queue.h"

/**
 * @brief Priority queue item for TWAI frame pointers.
 *
 * The queue stores TWAI frame pointers and orders them by priority.
 * Higher priority values are popped first; equal priorities keep FIFO order.
 */
typedef struct {
    const twai_frame_t *data;  /**< TWAI frame pointer stored in the queue */
    uint32_t priority;         /**< Higher value is dequeued first */
    uint64_t seq;              /**< Sequence number used to keep FIFO order for equal priority */
} twai_frame_queue_item_t;

/**
 * @brief Priority queue used by the TWAI driver to schedule queued frames.
 */
struct twai_frame_queue_s {
    portMUX_TYPE spinlock;            /**< Protects heap storage in task and ISR context */
    SemaphoreHandle_t spaces_sem;     /**< Counts available queue slots */
    twai_frame_queue_item_t *items;   /**< Binary heap storage */
    size_t capacity;                  /**< Maximum number of items */
    size_t count;                     /**< Current number of queued items */
    uint64_t next_seq;                /**< Next sequence number assigned on push */
};

static inline IRAM_ATTR bool twai_frame_queue_item_greater(const twai_frame_queue_item_t *a, const twai_frame_queue_item_t *b)
{
    if (a->priority != b->priority) {
        return a->priority > b->priority;
    }
    return a->seq < b->seq;
}

static inline IRAM_ATTR void twai_frame_queue_swap_item(twai_frame_queue_item_t *a, twai_frame_queue_item_t *b)
{
    twai_frame_queue_item_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static IRAM_ATTR void twai_frame_queue_sift_up(twai_frame_queue_t queue, size_t index)
{
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (!twai_frame_queue_item_greater(&queue->items[index], &queue->items[parent])) {
            break;
        }
        twai_frame_queue_swap_item(&queue->items[index], &queue->items[parent]);
        index = parent;
    }
}

static IRAM_ATTR void twai_frame_queue_sift_down(twai_frame_queue_t queue, size_t index)
{
    while (true) {
        size_t left = index * 2 + 1;
        size_t right = left + 1;
        size_t highest = index;

        if ((left < queue->count) && twai_frame_queue_item_greater(&queue->items[left], &queue->items[highest])) {
            highest = left;
        }
        if ((right < queue->count) && twai_frame_queue_item_greater(&queue->items[right], &queue->items[highest])) {
            highest = right;
        }
        if (highest == index) {
            break;
        }
        twai_frame_queue_swap_item(&queue->items[index], &queue->items[highest]);
        index = highest;
    }
}

static inline IRAM_ATTR void twai_frame_queue_push_locked(twai_frame_queue_t queue, const twai_frame_t *data, uint32_t priority)
{
    size_t index = queue->count++;
    queue->items[index] = (twai_frame_queue_item_t) {
        .data = data,
        .priority = priority,
        .seq = queue->next_seq++,
    };
    twai_frame_queue_sift_up(queue, index);
}

static inline IRAM_ATTR bool twai_frame_queue_pop_locked(twai_frame_queue_t queue, const twai_frame_t **data)
{
    if (queue->count == 0) {
        return false;
    }

    *data = queue->items[0].data;
    queue->count--;
    if (queue->count > 0) {
        queue->items[0] = queue->items[queue->count];
        twai_frame_queue_sift_down(queue, 0);
    }
    return true;
}

esp_err_t twai_frame_queue_new(twai_frame_queue_t *queue, size_t capacity, uint32_t mem_caps)
{
    if (!queue || !capacity) {
        return ESP_ERR_INVALID_ARG;
    }

    twai_frame_queue_t q_ctx = heap_caps_calloc(1, sizeof(struct twai_frame_queue_s), mem_caps);
    if (!q_ctx) {
        return ESP_ERR_NO_MEM;
    }
    q_ctx->items = heap_caps_calloc(capacity, sizeof(twai_frame_queue_item_t), mem_caps);
    if (!q_ctx->items) {
        heap_caps_free(q_ctx);
        return ESP_ERR_NO_MEM;
    }
    q_ctx->spaces_sem = xSemaphoreCreateCountingWithCaps(capacity, capacity, mem_caps);
    if (!q_ctx->spaces_sem) {
        heap_caps_free(q_ctx->items);
        heap_caps_free(q_ctx);
        return ESP_ERR_NO_MEM;
    }
    q_ctx->spinlock = (portMUX_TYPE) portMUX_INITIALIZER_UNLOCKED;
    q_ctx->capacity = capacity;

    *queue = q_ctx;
    return ESP_OK;
}

esp_err_t twai_frame_queue_del(twai_frame_queue_t queue)
{
    if (!queue) {
        return ESP_ERR_INVALID_ARG;
    }
    if (queue->spaces_sem) {
        vSemaphoreDeleteWithCaps(queue->spaces_sem);
    }
    if (queue->items) {
        heap_caps_free(queue->items);
    }
    heap_caps_free(queue);
    return ESP_OK;
}

esp_err_t twai_frame_queue_push(twai_frame_queue_t queue, const twai_frame_t *data, uint32_t priority, int timeout_ms)
{
    if (!queue || !queue->spaces_sem) {
        return ESP_ERR_INVALID_ARG;
    }

    TickType_t ticks_to_wait = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    if (xSemaphoreTake(queue->spaces_sem, ticks_to_wait) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    portENTER_CRITICAL(&queue->spinlock);
    twai_frame_queue_push_locked(queue, data, priority);
    portEXIT_CRITICAL(&queue->spinlock);
    return ESP_OK;
}

esp_err_t twai_frame_queue_push_from_isr(twai_frame_queue_t queue, const twai_frame_t *data, uint32_t priority, bool *task_woken)
{
    if (!queue || !queue->spaces_sem) {
        return ESP_ERR_INVALID_ARG;
    }

    if (xSemaphoreTakeFromISR(queue->spaces_sem, (BaseType_t *)task_woken) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    portENTER_CRITICAL_ISR(&queue->spinlock);
    twai_frame_queue_push_locked(queue, data, priority);
    portEXIT_CRITICAL_ISR(&queue->spinlock);
    return ESP_OK;
}

esp_err_t twai_frame_queue_push_safe(twai_frame_queue_t queue, const twai_frame_t *data, uint32_t priority, int timeout_ms)
{
    if (xPortInIsrContext()) {
        bool task_woken = false;
        esp_err_t ret = twai_frame_queue_push_from_isr(queue, data, priority, &task_woken);
        if (task_woken) {
            portYIELD_FROM_ISR();
        }
        return ret;
    }
    TickType_t ticks_to_wait = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return twai_frame_queue_push(queue, data, priority, ticks_to_wait);
}

esp_err_t twai_frame_queue_pop(twai_frame_queue_t queue, const twai_frame_t **data)
{
    bool ret;

    if (!queue || !queue->spaces_sem) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&queue->spinlock);
    ret = twai_frame_queue_pop_locked(queue, data);
    portEXIT_CRITICAL(&queue->spinlock);

    if (ret) {
        xSemaphoreGive(queue->spaces_sem);
    }
    return ret ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t twai_frame_queue_pop_from_isr(twai_frame_queue_t queue, const twai_frame_t **data, bool *task_woken)
{
    bool ret;

    if (!queue || !queue->spaces_sem) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_ISR(&queue->spinlock);
    ret = twai_frame_queue_pop_locked(queue, data);
    portEXIT_CRITICAL_ISR(&queue->spinlock);

    if (ret) {
        xSemaphoreGiveFromISR(queue->spaces_sem, (BaseType_t *)task_woken);
    }
    return ret ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t twai_frame_queue_pop_safe(twai_frame_queue_t queue, const twai_frame_t **data)
{
    if (xPortInIsrContext()) {
        bool task_woken = false;
        esp_err_t ret = twai_frame_queue_pop_from_isr(queue, data, &task_woken);
        if (task_woken) {
            portYIELD_FROM_ISR();
        }
        return ret;
    }
    return twai_frame_queue_pop(queue, data);
}

size_t twai_frame_queue_get_count(twai_frame_queue_t queue)
{
    size_t count;

    if (!queue || !queue->spaces_sem) {
        return 0;
    }

    portENTER_CRITICAL(&queue->spinlock);
    count = queue->count;
    portEXIT_CRITICAL(&queue->spinlock);
    return count;
}

size_t twai_frame_queue_get_free_space(twai_frame_queue_t queue)
{
    size_t count;

    if (!queue || !queue->spaces_sem) {
        return 0;
    }

    portENTER_CRITICAL(&queue->spinlock);
    count = queue->count;
    portEXIT_CRITICAL(&queue->spinlock);
    return queue->capacity - count;
}
