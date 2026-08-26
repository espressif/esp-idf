/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_v11_TAG_H_
#define _BLE_MESH_v11_TAG_H_

#include <stdint.h>
#include <stdbool.h>

#include "mesh.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !CONFIG_BLE_MESH_V11_SUPPORT

/* TAG - additional metadata */
#define BLE_MESH_TAG_SEND_SEGMENTED     BIT(0)
#define BLE_MESH_TAG_IMMUTABLE_CRED     BIT(1)
#define BLE_MESH_TAG_RELAY              BIT(3)
#define BLE_MESH_TAG_FRIENDSHIP         BIT(4)

static inline bool bt_mesh_tag_send_segmented(uint8_t tag)
{
    return (tag & BLE_MESH_TAG_SEND_SEGMENTED);
}

static inline bool bt_mesh_tag_immutable_cred(uint8_t tag)
{
    return (tag & BLE_MESH_TAG_IMMUTABLE_CRED);
}

static inline bool bt_mesh_tag_relay(uint8_t tag)
{
    return (tag & BLE_MESH_TAG_RELAY);
}

static inline bool bt_mesh_tag_friendship(uint8_t tag)
{
    return (tag & BLE_MESH_TAG_FRIENDSHIP);
}

#endif /* !CONFIG_BLE_MESH_V11_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_v11_TAG_H_ */
