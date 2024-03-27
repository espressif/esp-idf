/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 *  @brief Bluetooth Mesh Model Common APIs.
 */

#ifndef _BLE_MESH_COMMON_H_
#define _BLE_MESH_COMMON_H_

#include <stddef.h>
#include <stdlib.h>

#include "esp_attr.h"
#include "esp_heap_caps.h"

#include "mesh/byteorder.h"
#include "mesh/ffs.h"
#include "mesh/trace.h"
#include "mesh/mutex.h"
#include "mesh/access.h"

#ifdef __cplusplus
extern "C" {
#endif

void *bt_mesh_malloc(size_t size);

void *bt_mesh_calloc(size_t size);

void bt_mesh_free(void *ptr);

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

int bt_mesh_rand(void *buf, size_t len);

uint32_t bt_mesh_get_rand(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_COMMON_H_ */
