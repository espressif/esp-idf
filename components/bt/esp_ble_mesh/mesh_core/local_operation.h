/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LOCAL_OPERATION_H_
#define _LOCAL_OPERATION_H_

#include "mesh_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_mesh_model_subscribe_group_addr(u16_t elem_addr, u16_t mod_id,
                                       u16_t cid, u16_t group_addr);

int bt_mesh_model_unsubscribe_group_addr(u16_t elem_addr, u16_t cid,
                                         u16_t mod_id, u16_t group_addr);

#ifdef __cplusplus
}
#endif

#endif /* _LOCAL_OPERATION_H_ */
