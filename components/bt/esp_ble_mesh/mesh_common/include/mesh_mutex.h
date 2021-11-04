/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_MUTEX_H_
#define _BLE_MESH_MUTEX_H_

#include "mesh_kernel.h"
#include "mesh_slist.h"
#include "mesh_atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    SemaphoreHandle_t mutex;
#if CONFIG_BLE_MESH_FREERTOS_STATIC_ALLOC
    StaticQueue_t *buffer;
#endif
} bt_mesh_mutex_t;

void bt_mesh_mutex_create(bt_mesh_mutex_t *mutex);
void bt_mesh_mutex_free(bt_mesh_mutex_t *mutex);
void bt_mesh_mutex_lock(bt_mesh_mutex_t *mutex);
void bt_mesh_mutex_unlock(bt_mesh_mutex_t *mutex);

void bt_mesh_alarm_lock(void);
void bt_mesh_alarm_unlock(void);

void bt_mesh_list_lock(void);
void bt_mesh_list_unlock(void);

void bt_mesh_buf_lock(void);
void bt_mesh_buf_unlock(void);

void bt_mesh_atomic_lock(void);
void bt_mesh_atomic_unlock(void);

void bt_mesh_mutex_init(void);
void bt_mesh_mutex_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_MUTEX_H_ */
