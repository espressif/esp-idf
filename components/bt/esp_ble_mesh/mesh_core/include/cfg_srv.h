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

typedef union {
    struct {
        u8_t beacon;
    } cfg_beacon_set;
    struct {
        u8_t ttl;
    } cfg_default_ttl_set;
    struct {
        u8_t gatt_proxy;
    } cfg_gatt_proxy_set;
    struct {
        u8_t relay;
        u8_t retransmit;
    } cfg_relay_set;
    struct {
        u16_t elem_addr;
        u16_t pub_addr;
        u16_t app_idx;
        bool  cred_flag;
        u8_t  ttl;
        u8_t  period;
        u8_t  transmit;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_pub_set;
    struct {
        u16_t elem_addr;
        u8_t  pub_addr[16];
        u16_t app_idx;
        bool  cred_flag;
        u8_t  ttl;
        u8_t  period;
        u8_t  transmit;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_pub_va_set;
    struct {
        u16_t elem_addr;
        u16_t sub_addr;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_sub_add;
    struct {
        u16_t elem_addr;
        u8_t  sub_addr[16];
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_sub_va_add;
    struct {
        u16_t elem_addr;
        u16_t sub_addr;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_sub_delete;
    struct {
        u16_t elem_addr;
        u8_t  sub_addr[16];
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_sub_va_delete;
    struct {
        u16_t elem_addr;
        u16_t sub_addr;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_sub_overwrite;
    struct {
        u16_t elem_addr;
        u8_t  sub_addr[16];
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_sub_va_overwrite;
    struct {
        u16_t elem_addr;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_sub_delete_all;
    struct {
        u16_t net_idx;
        u8_t  net_key[16];
    } cfg_netkey_add;
    struct {
        u16_t net_idx;
        u8_t  net_key[16];
    } cfg_netkey_update;
    struct {
        u16_t net_idx;
    } cfg_netkey_delete;
    struct {
        u16_t net_idx;
        u16_t app_idx;
        u8_t  app_key[16];
    } cfg_appkey_add;
    struct {
        u16_t net_idx;
        u16_t app_idx;
        u8_t  app_key[16];
    } cfg_appkey_update;
    struct {
        u16_t net_idx;
        u16_t app_idx;
    } cfg_appkey_delete;
    struct {
        u16_t net_idx;
        u8_t  identity;
    } cfg_node_identity_set;
    struct {
        u16_t elem_addr;
        u16_t app_idx;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_app_bind;
    struct {
        u16_t elem_addr;
        u16_t app_idx;
        u16_t cid;
        u16_t mod_id;
    } cfg_mod_app_unbind;
    struct {
        u8_t frnd;
    } cfg_friend_set;
    struct {
        u16_t net_idx;
        u8_t  kr_phase;
    } cfg_kr_phase_set;
    struct {
        u16_t dst;
        u8_t  count;
        u8_t  period;
        u8_t  ttl;
        u16_t feat;
        u16_t net_idx;
    } cfg_hb_pub_set;
    struct {
        u16_t src;
        u16_t dst;
        u8_t  period;
    } cfg_hb_sub_set;
    struct {
        u8_t transmit;
    } cfg_net_transmit_set;
} bt_mesh_cfg_server_state_change_t;

/**
 * @}
 */

#endif /* __BLE_MESH_CFG_SRV_H */
