// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
