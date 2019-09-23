/** @file
 *  @brief Bluetooth Mesh Configuration Server Model APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_CFG_SRV_H_
#define _BLE_MESH_CFG_SRV_H_

#include "mesh_access.h"
#include "mesh_kernel.h"

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_cfg_srv Bluetooth Mesh Configuration Server Model
 * @ingroup bt_mesh
 * @{
 */

/** Mesh Configuration Server Model Context */
struct bt_mesh_cfg_srv {
    struct bt_mesh_model *model;

    u8_t net_transmit;         /* Network Transmit state */
    u8_t relay;                /* Relay Mode state */
    u8_t relay_retransmit;     /* Relay Retransmit state */
    u8_t beacon;               /* Secure Network Beacon state */
    u8_t gatt_proxy;           /* GATT Proxy state */
    u8_t frnd;                 /* Friend state */
    u8_t default_ttl;          /* Default TTL */

    /* Heartbeat Publication */
    struct bt_mesh_hb_pub {
        struct k_delayed_work timer;

        u16_t dst;
        u16_t count;
        u8_t  period;
        u8_t  ttl;
        u16_t feat;
        u16_t net_idx;
    } hb_pub;

    /* Heartbeat Subscription */
    struct bt_mesh_hb_sub {
        s64_t  expiry;

        u16_t src;
        u16_t dst;
        u16_t count;
        u8_t  min_hops;
        u8_t  max_hops;

        /* Optional subscription tracking function */
        void (*func)(u8_t hops, u16_t feat);
    } hb_sub;
};

extern const struct bt_mesh_model_op bt_mesh_cfg_srv_op[];

#define BLE_MESH_MODEL_CFG_SRV(srv_data)            \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_CFG_SRV,   \
            bt_mesh_cfg_srv_op, NULL, srv_data)

/**
 * @}
 */

#endif /* __BLE_MESH_CFG_SRV_H */
