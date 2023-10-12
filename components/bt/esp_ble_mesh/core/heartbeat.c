/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "lpn.h"
#include "mesh.h"
#include "mesh/main.h"
#include "access.h"
#include "foundation.h"
#include "transport.h"
#include "heartbeat.h"
#include "mesh/cfg_srv.h"

static uint16_t hb_sub_dst = BLE_MESH_ADDR_UNASSIGNED;

void bt_mesh_set_hb_sub_dst(uint16_t addr)
{
    hb_sub_dst = addr;
}

uint16_t bt_mesh_get_hb_sub_dst(void)
{
    return hb_sub_dst;
}

void bt_mesh_heartbeat_recv(uint16_t src, uint16_t dst, uint8_t hops, uint16_t feat)
{
    struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();

    if (cfg == NULL) {
        BT_WARN("No configuration server context available");
        return;
    }

    if (src != cfg->hb_sub.src || dst != cfg->hb_sub.dst) {
        BT_WARN("No subscription for received heartbeat");
        return;
    }

    if (k_uptime_get() > cfg->hb_sub.expiry) {
        BT_WARN("Heartbeat subscription period expired");
        return;
    }

    cfg->hb_sub.min_hops = MIN(cfg->hb_sub.min_hops, hops);
    cfg->hb_sub.max_hops = MAX(cfg->hb_sub.max_hops, hops);

    if (cfg->hb_sub.count < 0xffff) {
        cfg->hb_sub.count++;
    }

    BT_DBG("src 0x%04x dst 0x%04x hops %u min %u max %u count %u", src,
           dst, hops, cfg->hb_sub.min_hops, cfg->hb_sub.max_hops,
           cfg->hb_sub.count);

    if (cfg->hb_sub.func) {
        cfg->hb_sub.func(hops, feat);
    }
}

void bt_mesh_heartbeat_send(void)
{
    struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();
    uint16_t feat = 0U;
    struct __attribute__((packed)) {
        uint8_t  init_ttl;
        uint16_t feat;
    } hb;
    struct bt_mesh_msg_ctx ctx = {
        .net_idx   = cfg->hb_pub.net_idx,
        .app_idx   = BLE_MESH_KEY_UNUSED,
        .addr      = cfg->hb_pub.dst,
        .send_ttl  = cfg->hb_pub.ttl,
        .send_cred = BLE_MESH_FLOODING_CRED,
    };
    struct bt_mesh_net_tx tx = {
        .sub  = bt_mesh_subnet_get(cfg->hb_pub.net_idx),
        .ctx  = &ctx,
        .src  = bt_mesh_model_elem(cfg->model)->addr,
        .xmit = bt_mesh_net_transmit_get(),
    };

    /* Do nothing if heartbeat publication is not enabled */
    if (cfg->hb_pub.dst == BLE_MESH_ADDR_UNASSIGNED) {
        return;
    }

    hb.init_ttl = cfg->hb_pub.ttl;

    if (bt_mesh_relay_get() == BLE_MESH_RELAY_ENABLED) {
        feat |= BLE_MESH_FEAT_RELAY;
    }

    if (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED) {
        feat |= BLE_MESH_FEAT_PROXY;
    }

    if (bt_mesh_friend_get() == BLE_MESH_FRIEND_ENABLED) {
        feat |= BLE_MESH_FEAT_FRIEND;
    }

    if (bt_mesh_lpn_established()) {
        feat |= BLE_MESH_FEAT_LOW_POWER;
    }

    hb.feat = sys_cpu_to_be16(feat);

    BT_INFO("InitTTL %u feat 0x%04x", cfg->hb_pub.ttl, feat);

    bt_mesh_ctl_send(&tx, TRANS_CTL_OP_HEARTBEAT, &hb, sizeof(hb),
                     NULL, NULL);
}

#if CONFIG_BLE_MESH_PROVISIONER_RECV_HB

#define HEARTBEAT_FILTER_ACCEPTLIST     0x00
#define HEARTBEAT_FILTER_REJECTLIST     0x01

#define HEARTBEAT_FILTER_ADD            0x00
#define HEARTBEAT_FILTER_REMOVE         0x01

static struct heartbeat_recv {
    struct heartbeat_filter {
        uint16_t src;               /* Heartbeat SRC (unicast address) */
        uint16_t dst;               /* Heartbeat DST (unicast address or group address) */
    } filter[CONFIG_BLE_MESH_PROVISIONER_RECV_HB_FILTER_SIZE];

    uint8_t type;                   /* Heartbeat filter type */

    bt_mesh_pvnr_hb_recv_cb_t cb;   /* Heartbeat receive callback */
} hb_rx;

int bt_mesh_pvnr_register_hb_recv_cb(bt_mesh_pvnr_hb_recv_cb_t cb)
{
    memset(&hb_rx, 0, sizeof(hb_rx));

    /* Start with an empty rejectlist, which means all heartbeat messages will be reported */
    hb_rx.type = HEARTBEAT_FILTER_REJECTLIST;
    hb_rx.cb = cb;

    return 0;
}

int bt_mesh_pvnr_set_hb_recv_filter_type(uint8_t type)
{
    if (type > HEARTBEAT_FILTER_REJECTLIST) {
        BT_ERR("Invalid heartbeat filter type 0x%02x", type);
        return -EINVAL;
    }

    /* If the heartbeat filter type is different with previous one,
     * clear the existing filter entries.
     */
    if (hb_rx.type != type) {
        memset(&hb_rx, 0, offsetof(struct heartbeat_recv, cb));
        hb_rx.type = type;
    }

    return 0;
}

static int hb_filter_alloc(uint16_t src, uint16_t dst)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(hb_rx.filter); i++) {
        struct heartbeat_filter *filter = &hb_rx.filter[i];

        if (filter->src == BLE_MESH_ADDR_UNASSIGNED &&
            filter->dst == BLE_MESH_ADDR_UNASSIGNED) {
            filter->src = src;
            filter->dst = dst;
            return 0;
        }
    }

    BT_ERR("Heartbeat filter is full!");
    return -ENOMEM;
}

static int hb_filter_add(uint16_t src, uint16_t dst)
{
    int i;

    if (!(BLE_MESH_ADDR_IS_UNICAST(src) &&
        (BLE_MESH_ADDR_IS_UNICAST(dst) || BLE_MESH_ADDR_IS_GROUP(dst)))) {
        BT_ERR("Invalid filter address, src 0x%04x, dst 0x%04x", src, dst);
        return -EINVAL;
    }

    /* Check if filter entries with the same src or dst exist. */
    for (i = 0; i < ARRAY_SIZE(hb_rx.filter); i++) {
        struct heartbeat_filter *filter = &hb_rx.filter[i];

        if (filter->src == src && filter->dst == dst) {
            BT_WARN("Filter already exists, src 0x%04x dst 0x%04x", filter->src, filter->dst);
            return 0;
        }
    }

    return hb_filter_alloc(src, dst);
}

static int hb_filter_remove(uint16_t src, uint16_t dst)
{
    int i;

    if (!(BLE_MESH_ADDR_IS_UNICAST(src) &&
        (BLE_MESH_ADDR_IS_UNICAST(dst) || BLE_MESH_ADDR_IS_GROUP(dst)))) {
        BT_ERR("Invalid filter address, src 0x%04x, dst 0x%04x", src, dst);
        return -EINVAL;
    }

    for (i = 0; i < ARRAY_SIZE(hb_rx.filter); i++) {
        struct heartbeat_filter *filter = &hb_rx.filter[i];

        if (filter->src == src && filter->dst == dst) {
            memset(filter, 0, sizeof(struct heartbeat_filter));
        }
    }

    return 0;
}

int bt_mesh_pvnr_set_hb_recv_filter_info(uint8_t op, uint16_t src, uint16_t dst)
{
    switch (op) {
    case HEARTBEAT_FILTER_ADD:
        return hb_filter_add(src, dst);
    case HEARTBEAT_FILTER_REMOVE:
        return hb_filter_remove(src, dst);
    default:
        BT_ERR("Invalid heartbeat filter opcode 0x%02x", op);
        return -EINVAL;
    }
}

static bool filter_with_rejectlist(uint16_t hb_src, uint16_t hb_dst)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(hb_rx.filter); i++) {
        struct heartbeat_filter *filter = &hb_rx.filter[i];
        if (hb_src == filter->src && hb_dst == filter->dst) {
            return true;
        }
    }

    return false;
}

static bool filter_with_acceptlist(uint16_t hb_src, uint16_t hb_dst)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(hb_rx.filter); i++) {
        struct heartbeat_filter *filter = &hb_rx.filter[i];
        if (hb_src == filter->src && hb_dst == filter->dst) {
            return false;
        }
    }

    return true;
}

void bt_mesh_pvnr_heartbeat_recv(uint16_t hb_src, uint16_t hb_dst,
                                 uint8_t init_ttl, uint8_t rx_ttl,
                                 uint8_t hops, uint16_t feat, int8_t rssi)
{
    if (hb_rx.cb == NULL) {
        BT_DBG("Receiving heartbeat is not enabled");
        return;
    }

    if (hb_rx.type == HEARTBEAT_FILTER_REJECTLIST) {
        if (filter_with_rejectlist(hb_src, hb_dst)) {
            BT_INFO("Filtered by rejectlist, src 0x%04x, dst 0x%04x", hb_src, hb_dst);
            return;
        }
    } else {
        if (filter_with_acceptlist(hb_src, hb_dst)) {
            BT_INFO("Filtered by acceptlist, src 0x%04x, dst 0x%04x", hb_src, hb_dst);
            return;
        }
    }

    if (hb_rx.cb) {
        hb_rx.cb(hb_src, hb_dst, init_ttl, rx_ttl, hops, feat, rssi);
    }
}

#endif /* CONFIG_BLE_MESH_PROVISIONER_RECV_HB */
