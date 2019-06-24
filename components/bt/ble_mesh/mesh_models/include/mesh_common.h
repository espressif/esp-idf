// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _MESH_COMMON_H_
#define _MESH_COMMON_H_

#include "osi/allocator.h"

#include "mesh_types.h"
#include "mesh_buf.h"
#include "mesh_trace.h"

/**
 * @brief This function allocates memory to store outgoing message.
 *
 * @param[in] size: Length of memory allocated to store message value
 *
 * @return NULL-fail, pointer of a net_buf_simple structure-success
 */
struct net_buf_simple *bt_mesh_alloc_buf(u16_t size);

/**
 * @brief This function releases the memory allocated for the outgoing message.
 *
 * @param[in] buf: Pointer to the net_buf_simple structure to be freed
 *
 * @return none
 */
void bt_mesh_free_buf(struct net_buf_simple *buf);

#endif /* _MESH_COMMON_H_ */