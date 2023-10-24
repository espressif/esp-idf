/** @file
 *  @brief Bluetooth Mesh Configuration Server Model APIs.
 */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_CFG_SRV_H_
#define _BLE_MESH_CFG_SRV_H_

#include "mesh/access.h"

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_cfg_srv Bluetooth Mesh Configuration Server Model
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Mesh Configuration Server Model Context */
struct bt_mesh_cfg_srv {
    struct bt_mesh_model *model;

    uint8_t net_transmit;      /* Network Transmit state */
    uint8_t relay;             /* Relay Mode state */
    uint8_t relay_retransmit;  /* Relay Retransmit state */
    uint8_t beacon;            /* Secure Network Beacon state */
    uint8_t gatt_proxy;        /* GATT Proxy state */
    uint8_t frnd;              /* Friend state */
    uint8_t default_ttl;       /* Default TTL */

    /* Heartbeat Publication */
    struct bt_mesh_hb_pub {
        struct k_delayed_work timer;

        uint16_t dst;
        uint16_t count;
        uint8_t  period;
        uint8_t  ttl;
        uint16_t feat;
        uint16_t net_idx;
    } hb_pub;

    /* Heartbeat Subscription */
    struct bt_mesh_hb_sub {
        int64_t  expiry;

        uint16_t src;
        uint16_t dst;
        uint16_t count;
        uint8_t  min_hops;
        uint8_t  max_hops;

        /* Optional subscription tracking function */
        void (*func)(uint8_t hops, uint16_t feat);
    } hb_sub;
};

extern const struct bt_mesh_model_op bt_mesh_cfg_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_cfg_srv_cb;

#define BLE_MESH_MODEL_CFG_SRV(srv_data)            \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_CFG_SRV,   \
            bt_mesh_cfg_srv_op, NULL, srv_data, &bt_mesh_cfg_srv_cb)

typedef union {
    struct {
        uint8_t beacon;
    } cfg_beacon_set;
    struct {
        uint8_t ttl;
    } cfg_default_ttl_set;
    struct {
        uint8_t gatt_proxy;
    } cfg_gatt_proxy_set;
    struct {
        uint8_t relay;
        uint8_t retransmit;
    } cfg_relay_set;
    struct {
        uint16_t elem_addr;
        uint16_t pub_addr;
        uint16_t app_idx;
        bool  cred_flag;
        uint8_t  ttl;
        uint8_t  period;
        uint8_t  transmit;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_pub_set;
    struct {
        uint16_t elem_addr;
        uint8_t  pub_addr[16];
        uint16_t app_idx;
        bool  cred_flag;
        uint8_t  ttl;
        uint8_t  period;
        uint8_t  transmit;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_pub_va_set;
    struct {
        uint16_t elem_addr;
        uint16_t sub_addr;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_sub_add;
    struct {
        uint16_t elem_addr;
        uint8_t  sub_addr[16];
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_sub_va_add;
    struct {
        uint16_t elem_addr;
        uint16_t sub_addr;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_sub_delete;
    struct {
        uint16_t elem_addr;
        uint8_t  sub_addr[16];
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_sub_va_delete;
    struct {
        uint16_t elem_addr;
        uint16_t sub_addr;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_sub_overwrite;
    struct {
        uint16_t elem_addr;
        uint8_t  sub_addr[16];
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_sub_va_overwrite;
    struct {
        uint16_t elem_addr;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_sub_delete_all;
    struct {
        uint16_t net_idx;
        uint8_t  net_key[16];
    } cfg_netkey_add;
    struct {
        uint16_t net_idx;
        uint8_t  net_key[16];
    } cfg_netkey_update;
    struct {
        uint16_t net_idx;
    } cfg_netkey_delete;
    struct {
        uint16_t net_idx;
        uint16_t app_idx;
        uint8_t  app_key[16];
    } cfg_appkey_add;
    struct {
        uint16_t net_idx;
        uint16_t app_idx;
        uint8_t  app_key[16];
    } cfg_appkey_update;
    struct {
        uint16_t net_idx;
        uint16_t app_idx;
    } cfg_appkey_delete;
    struct {
        uint16_t net_idx;
        uint8_t  identity;
    } cfg_node_identity_set;
    struct {
        uint16_t elem_addr;
        uint16_t app_idx;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_app_bind;
    struct {
        uint16_t elem_addr;
        uint16_t app_idx;
        uint16_t cid;
        uint16_t mod_id;
    } cfg_mod_app_unbind;
    struct {
        uint8_t frnd;
    } cfg_friend_set;
    struct {
        uint16_t net_idx;
        uint8_t  kr_phase;
    } cfg_kr_phase_set;
    struct {
        uint16_t dst;
        uint8_t  count;
        uint8_t  period;
        uint8_t  ttl;
        uint16_t feat;
        uint16_t net_idx;
    } cfg_hb_pub_set;
    struct {
        uint16_t src;
        uint16_t dst;
        uint8_t  period;
    } cfg_hb_sub_set;
    struct {
        uint8_t transmit;
    } cfg_net_transmit_set;
} bt_mesh_cfg_server_state_change_t;

void bt_mesh_relay_local_set(bool enable);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BLE_MESH_CFG_SRV_H */
