/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_QUEUE_H_
#define _BLE_MESH_QUEUE_H_

#include "mesh/kernel.h"
#include "mesh/slist.h"
#include "mesh/atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    QueueHandle_t handle;
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    StaticQueue_t *buffer;
    uint8_t *storage;
#endif
} bt_mesh_queue_t;

int bt_mesh_queue_init(bt_mesh_queue_t *queue, uint16_t queue_size, uint8_t item_size);
int bt_mesh_queue_deinit(bt_mesh_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_QUEUE_H_ */
