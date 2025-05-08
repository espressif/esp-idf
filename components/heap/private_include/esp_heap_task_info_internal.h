/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef CONFIG_HEAP_TASK_TRACKING

#ifdef __cplusplus
extern "C" {
#endif

void heap_caps_update_per_task_info_alloc(heap_t *heap, void *ptr, size_t size, uint32_t caps);
void heap_caps_update_per_task_info_free(heap_t *heap, void *ptr);
void heap_caps_update_per_task_info_realloc(heap_t *heap, void *old_ptr, void *new_ptr, size_t old_size, TaskHandle_t old_task, size_t new_size, uint32_t caps);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_HEAP_TASK_TRACKING
