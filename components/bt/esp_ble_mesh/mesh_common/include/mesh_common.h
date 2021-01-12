// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

/** @file
 *  @brief Bluetooth Mesh Model Common APIs.
 */

#ifndef _BLE_MESH_COMMON_H_
#define _BLE_MESH_COMMON_H_

#include <stddef.h>
#include <stdlib.h>

#include "esp_attr.h"
#include "esp_heap_caps.h"

#include "mesh_byteorder.h"
#include "mesh_ffs.h"
#include "mesh_trace.h"
#include "mesh_mutex.h"
#include "mesh_access.h"

#ifdef __cplusplus
extern "C" {
#endif

IRAM_ATTR void *bt_mesh_malloc(size_t size);

IRAM_ATTR void *bt_mesh_calloc(size_t size);

IRAM_ATTR void bt_mesh_free(void *ptr);

/**
 * @brief This function allocates memory to store outgoing message.
 *
 * @param[in] size: Length of memory allocated to store message value
 *
 * @return NULL-fail, pointer of a net_buf_simple structure-success
 */
struct net_buf_simple *bt_mesh_alloc_buf(uint16_t size);

/**
 * @brief This function releases the memory allocated for the outgoing message.
 *
 * @param[in] buf: Pointer to the net_buf_simple structure to be freed
 *
 * @return none
 */
void bt_mesh_free_buf(struct net_buf_simple *buf);

/**
 * @brief This function gets device role for stack internal use.
 *
 * @Note  Currently Provisioner only support client models, Node supports
 *        client models and server models. Hence if srv_send is set to be
 *        TRUE, then role NODE will be returned.
 *
 * @param[in] model:    Pointer to the model structure
 * @param[in] srv_send: Indicate if the message is sent by a server model
 *
 * @return 0 - Node, 1 - Provisioner
 */
uint8_t bt_mesh_get_device_role(struct bt_mesh_model *model, bool srv_send);

int bt_mesh_rand(void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_COMMON_H_ */