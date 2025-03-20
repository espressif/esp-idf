/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh/common.h"
#include "mesh/queue.h"

int bt_mesh_queue_init(bt_mesh_queue_t *queue, uint16_t queue_size, uint8_t item_size)
{
    __ASSERT(queue && queue_size && item_size, "Invalid queue init parameters");

#if !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    queue->handle = xQueueCreate(queue_size, item_size);
    __ASSERT(queue->handle, "Failed to create queue");
#else /* !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    queue->buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    queue->buffer = heap_caps_calloc_prefer(1, sizeof(StaticQueue_t), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(queue->buffer, "Failed to create queue buffer");
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_EXTERNAL
    queue->storage = heap_caps_calloc_prefer(1, (queue_size * item_size), 2, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#elif CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC_IRAM_8BIT
    queue->storage = heap_caps_calloc_prefer(1, (queue_size * item_size), 2, MALLOC_CAP_INTERNAL|MALLOC_CAP_IRAM_8BIT, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
#endif
    __ASSERT(queue->storage, "Failed to create queue storage");
    queue->handle = xQueueCreateStatic(queue_size, item_size, (uint8_t*)queue->storage, queue->buffer);
    __ASSERT(queue->handle, "Failed to create static queue");
#endif /* !CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC */
    return 0;
}

int bt_mesh_queue_deinit(bt_mesh_queue_t *queue)
{
    __ASSERT(queue, "Invalid queue init parameters");
    vQueueDelete(queue->handle);
    queue->handle = NULL;
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    heap_caps_free(queue->buffer);
    queue->buffer = NULL;
    heap_caps_free(queue->storage);
    queue->storage = NULL;
#endif
    return 0;
}
