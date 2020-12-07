/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "adv.h"
#include "mesh.h"
#include "prov.h"
#include "crypto.h"
#include "beacon.h"
#include "access.h"
#include "foundation.h"
#include "proxy_client.h"
#include "mesh_main.h"
#include "provisioner_prov.h"
#include "provisioner_main.h"

#if defined(CONFIG_BLE_MESH_FAST_PROV)
#define UNPROVISIONED_INTERVAL     K_SECONDS(3)
#else
#define UNPROVISIONED_INTERVAL     K_SECONDS(5)
#endif /* CONFIG_BLE_MESH_FAST_PROV */
#define PROVISIONED_INTERVAL       K_SECONDS(10)

#define BEACON_TYPE_UNPROVISIONED  0x00
#define BEACON_TYPE_SECURE         0x01

/* 3 transmissions, 20ms interval */
#define UNPROV_XMIT                BLE_MESH_TRANSMIT(2, 20)

/* 1 transmission, 20ms interval */
#define PROV_XMIT                  BLE_MESH_TRANSMIT(0, 20)

#define SNB_NET_IDX_SET(_val)      ((void *)((uint32_t)(_val)))
#define SNB_NET_IDX_GET(_ptr)      ((uint32_t)(_ptr))

static struct k_delayed_work beacon_timer;

static struct bt_mesh_subnet *cache_check(uint8_t data[21])
{
    size_t subnet_size = 0U;
    int i = 0;

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        if (!memcmp(sub->beacon_cache, data, 21)) {
            return sub;
        }
    }

    return NULL;
}

static void cache_add(uint8_t data[21], struct bt_mesh_subnet *sub)
{
    memcpy(sub->beacon_cache, data, 21);
}

static void beacon_complete(int err, void *user_data)
{
    struct bt_mesh_subnet *sub = NULL;
    uint16_t net_idx = BLE_MESH_KEY_UNUSED;

    BT_DBG("err %d", err);

    net_idx = (uint16_t)SNB_NET_IDX_GET(user_data);

    /* For node, directly updating the "beacon_sent" timestamp is fine,
     * since the subnet is pre-allocated.
     * For Provisioner, before updating the "beacon_sent" timestamp, we
     * need to make sure that the subnet still exists, because there is
     * a chance that the subnet is removed just before the completion of
     * sending the Secure Network Beacon.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        sub = bt_mesh_subnet_get(net_idx);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
        bt_mesh_is_provisioner_en()) {
        sub = bt_mesh_provisioner_subnet_get(net_idx);
    }

    if (sub) {
        sub->beacon_sent = k_uptime_get_32();
    }
}

void bt_mesh_beacon_create(struct bt_mesh_subnet *sub,
                           struct net_buf_simple *buf)
{
    uint8_t flags = bt_mesh_net_flags(sub);
    struct bt_mesh_subnet_keys *keys = NULL;

    net_buf_simple_add_u8(buf, BEACON_TYPE_SECURE);

    if (sub->kr_flag) {
        keys = &sub->keys[1];
    } else {
        keys = &sub->keys[0];
    }

    net_buf_simple_add_u8(buf, flags);

    /* Network ID */
    net_buf_simple_add_mem(buf, keys->net_id, 8);

    /* IV Index */
    net_buf_simple_add_be32(buf, bt_mesh.iv_index);

    net_buf_simple_add_mem(buf, sub->auth, 8);

    BT_INFO("net_idx 0x%03x iv_index 0x%08x flags 0x%02x",
            sub->net_idx, bt_mesh.iv_index, flags);
    BT_DBG("NetID %s Auth %s", bt_hex(keys->net_id, 8),
            bt_hex(sub->auth, 8));
}

/* If the interval has passed or is within 5 seconds from now send a beacon */
#define BEACON_THRESHOLD(sub) (K_SECONDS(10 * ((sub)->beacons_last + 1)) - K_SECONDS(5))

static int secure_beacon_send(void)
{
    static const struct bt_mesh_send_cb send_cb = {
        .end = beacon_complete,
    };
    uint32_t now = k_uptime_get_32();
    size_t subnet_size = 0U;
    int i = 0;

    BT_DBG("%s", __func__);

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);
        struct net_buf *buf;
        uint32_t time_diff;

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        time_diff = now - sub->beacon_sent;
        if (time_diff < K_SECONDS(600) &&
                time_diff < BEACON_THRESHOLD(sub)) {
            continue;
        }

        /**
         * If a node enables the Proxy Client functionality, and it
         * succeeds to send Secure Network Beacon with GATT bearer,
         * here we will continue to send Secure Network Beacon of
         * other subnets.
         */
#if defined(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)
        if (bt_mesh_proxy_client_beacon_send(sub)) {
            continue;
        }
#endif

        buf = bt_mesh_adv_create(BLE_MESH_ADV_BEACON, PROV_XMIT,
                                 K_NO_WAIT);
        if (!buf) {
            BT_ERR("Out of beacon buffer");
            return -ENOBUFS;
        }

        bt_mesh_beacon_create(sub, &buf->b);

        /* Care should be taken here. Previously the user_data is the
         * pointer of a subnet. When the device is a Provisioner, its
         * subnet is created dynamically. If the corresponding subnet
         * is removed right after the Secure Network Beacon is sent,
         * update its "beacon_sent" timestamp in beacon_complete() will
         * cause exception.
         * Here we use the "net_idx" of the subnet instead. And in the
         * beacon_complete(), we will try to get the subnet before
         * updating its "beacon_sent" timestamp.
         */
        bt_mesh_adv_send(buf, &send_cb, SNB_NET_IDX_SET(sub->net_idx));
        net_buf_unref(buf);
    }

    return 0;
}

#if defined(CONFIG_BLE_MESH_NODE)
static int unprovisioned_beacon_send(void)
{
#if defined(CONFIG_BLE_MESH_PB_ADV)
    const struct bt_mesh_prov *prov = NULL;
    uint8_t uri_hash[16] = { 0 };
    struct net_buf *buf = NULL;
    uint16_t oob_info = 0U;

    BT_DBG("%s", __func__);

    buf = bt_mesh_adv_create(BLE_MESH_ADV_BEACON, UNPROV_XMIT, K_NO_WAIT);
    if (!buf) {
        BT_ERR("Out of beacon buffer");
        return -ENOBUFS;
    }

    prov = bt_mesh_prov_get();

    net_buf_add_u8(buf, BEACON_TYPE_UNPROVISIONED);
    net_buf_add_mem(buf, prov->uuid, 16);

    if (prov->uri && bt_mesh_s1(prov->uri, uri_hash) == 0) {
        oob_info = prov->oob_info | BLE_MESH_PROV_OOB_URI;
    } else {
        oob_info = prov->oob_info;
    }

    net_buf_add_be16(buf, oob_info);
    net_buf_add_mem(buf, uri_hash, 4);

    bt_mesh_adv_send(buf, NULL, NULL);
    net_buf_unref(buf);

    if (prov->uri) {
        size_t len;

        buf = bt_mesh_adv_create(BLE_MESH_ADV_URI, UNPROV_XMIT,
                                 K_NO_WAIT);
        if (!buf) {
            BT_ERR("Unable to allocate URI buffer");
            return -ENOBUFS;
        }

        len = strlen(prov->uri);
        if (net_buf_tailroom(buf) < len) {
            BT_WARN("Too long URI to fit advertising data");
        } else {
            net_buf_add_mem(buf, prov->uri, len);
            bt_mesh_adv_send(buf, NULL, NULL);
        }

        net_buf_unref(buf);
    }

#endif /* CONFIG_BLE_MESH_PB_ADV */
    return 0;
}
#else /* CONFIG_BLE_MESH_NODE */
static int unprovisioned_beacon_send(void)
{
    return 0;
}
#endif /* CONFIG_BLE_MESH_NODE */

static void update_beacon_observation(void)
{
    static bool first_half;
    size_t subnet_size = 0U;
    int i = 0;

    /* Observation period is 20 seconds, whereas the beacon timer
     * runs every 10 seconds. We process what's happened during the
     * window only after the second half.
     */
    first_half = !first_half;
    if (first_half) {
        return;
    }

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        sub->beacons_last = sub->beacons_cur;
        sub->beacons_cur = 0U;
    }
}

static bool ready_to_send(void)
{
    if (bt_mesh_is_provisioned() || bt_mesh_is_provisioner_en()) {
        return true;
    }
    return false;
}

static void beacon_send(struct k_work *work)
{
    /* Don't send anything if we have an active provisioning link */
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
            IS_ENABLED(CONFIG_BLE_MESH_PROV) && bt_prov_active()) {
        k_delayed_work_submit(&beacon_timer, UNPROVISIONED_INTERVAL);
        return;
    }

    BT_DBG("%s", __func__);

    if (ready_to_send()) {
        update_beacon_observation();
        secure_beacon_send();

        /* Only resubmit if beaconing is still enabled */
        if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED ||
                bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_INITIATOR)) {
            k_delayed_work_submit(&beacon_timer,
                                  PROVISIONED_INTERVAL);
        }
    } else {
        if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node()) {
            unprovisioned_beacon_send();
            k_delayed_work_submit(&beacon_timer, UNPROVISIONED_INTERVAL);
        }
    }

}

static void secure_beacon_recv(struct net_buf_simple *buf)
{
    uint8_t *data = NULL, *net_id = NULL, *auth = NULL;
    struct bt_mesh_subnet *sub = NULL;
    uint32_t iv_index = 0U;
    bool kr_change = false;
    bool iv_change = false;
    bool new_key = false;
    uint8_t flags = 0U;

    if (buf->len < 21) {
        BT_ERR("Too short secure beacon (len %u)", buf->len);
        return;
    }

    sub = cache_check(buf->data);
    if (sub) {
        /* We've seen this beacon before - just update the stats */
        goto update_stats;
    }

    /* So we can add to the cache if auth matches */
    data = buf->data;

    flags = net_buf_simple_pull_u8(buf);
    net_id = net_buf_simple_pull_mem(buf, 8);
    iv_index = net_buf_simple_pull_be32(buf);
    auth = buf->data;

    BT_DBG("flags 0x%02x id %s iv_index 0x%08x",
           flags, bt_hex(net_id, 8), iv_index);

    sub = bt_mesh_subnet_find(net_id, flags, iv_index, auth, &new_key);
    if (!sub) {
        BT_DBG("No subnet that matched beacon");
        return;
    }

    if (sub->kr_phase == BLE_MESH_KR_PHASE_2 && !new_key) {
        BT_WARN("Ignoring Phase 2 KR Update secured using old key");
        return;
    }

    cache_add(data, sub);

    /* If we have NetKey0 accept initiation only from it */
    if (bt_mesh_primary_subnet_exist() &&
            sub->net_idx != BLE_MESH_KEY_PRIMARY) {
        BT_WARN("Ignoring secure beacon on non-primary subnet");
        goto update_stats;
    }

    BT_INFO("net_idx 0x%03x iv_index 0x%08x current iv_index 0x%08x",
            sub->net_idx, iv_index, bt_mesh.iv_index);

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_INITIATOR) &&
            (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS) ==
             BLE_MESH_IV_UPDATE(flags))) {
        bt_mesh_beacon_ivu_initiator(false);
    }

    iv_change = bt_mesh_net_iv_update(iv_index, BLE_MESH_IV_UPDATE(flags));

    kr_change = bt_mesh_kr_update(sub, BLE_MESH_KEY_REFRESH(flags), new_key);
    if (kr_change) {
        bt_mesh_net_beacon_update(sub);
    }

    if (iv_change) {
        /* Update all subnets */
        bt_mesh_net_sec_update(NULL);
    } else if (kr_change) {
        /* Key Refresh without IV Update only impacts one subnet */
        bt_mesh_net_sec_update(sub);
    }

update_stats:
    if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED &&
            sub->beacons_cur < 0xff) {
        sub->beacons_cur++;
    }
}

void bt_mesh_beacon_recv(struct net_buf_simple *buf, int8_t rssi)
{
    uint8_t type = 0U;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (buf->len < 1) {
        BT_ERR("Too short beacon");
        return;
    }

    type = net_buf_simple_pull_u8(buf);
    switch (type) {
    case BEACON_TYPE_UNPROVISIONED:
        BT_DBG("Unprovisioned device beacon received");
        if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
                bt_mesh_is_provisioner_en()) {
            bt_mesh_provisioner_unprov_beacon_recv(buf, rssi);
        }
        break;
    case BEACON_TYPE_SECURE:
        secure_beacon_recv(buf);
        break;
    default:
        BT_DBG("Unknown beacon type 0x%02x", type);
        break;
    }
}

void bt_mesh_beacon_init(void)
{
    k_delayed_work_init(&beacon_timer, beacon_send);
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_beacon_deinit(void)
{
    k_delayed_work_free(&beacon_timer);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_beacon_ivu_initiator(bool enable)
{
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_INITIATOR, enable);

    if (enable) {
        k_work_submit(&beacon_timer.work);
    } else if (bt_mesh_beacon_get() == BLE_MESH_BEACON_DISABLED) {
        k_delayed_work_cancel(&beacon_timer);
    }
}

void bt_mesh_beacon_enable(void)
{
    size_t subnet_size = 0U;
    int i = 0;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
            !bt_mesh_is_provisioned()) {
        k_work_submit(&beacon_timer.work);
        return;
    }

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        sub->beacons_last = 0U;
        sub->beacons_cur = 0U;

        bt_mesh_net_beacon_update(sub);
    }

    k_work_submit(&beacon_timer.work);
}

void bt_mesh_beacon_disable(void)
{
    if (!bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_INITIATOR)) {
        k_delayed_work_cancel(&beacon_timer);
    }
}
