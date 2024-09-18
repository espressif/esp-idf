/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "adv.h"
#include "scan.h"
#include "mesh.h"
#include "crypto.h"
#include "beacon.h"
#include "access.h"
#include "foundation.h"
#include "proxy_client.h"
#include "mesh/main.h"
#include "prov_common.h"
#include "prov_node.h"
#include "prov_pvnr.h"
#include "pvnr_mgmt.h"
#include "mesh/common.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif

#if defined(CONFIG_BLE_MESH_UNPROVISIONED_BEACON_INTERVAL)
#define UNPROV_BEACON_INTERVAL     K_SECONDS(CONFIG_BLE_MESH_UNPROVISIONED_BEACON_INTERVAL)
#else
#define UNPROV_BEACON_INTERVAL     K_SECONDS(5)
#endif

#define SECURE_BEACON_INTERVAL     K_SECONDS(10)

/* 3 transmissions, 20ms interval */
#define UNPROV_XMIT                BLE_MESH_TRANSMIT(2, 20)

/* 1 transmission, 20ms interval */
#define SNB_XMIT                   BLE_MESH_TRANSMIT(0, 20)

/* For a device, using the snb_timer when sending Unprovisioned Device Beacon;
 * For a node, using the snb_timer when sending Secure Network Beacon.
 */
static struct k_delayed_work snb_timer;

struct bt_mesh_subnet *cache_check(uint8_t data[21], bool private_beacon)
{
    size_t subnet_size = 0U;
    uint8_t *cache = NULL;
    int i = 0;

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

#if CONFIG_BLE_MESH_PRIVATE_BEACON
        cache = private_beacon ? sub->mpb_cache : sub->snb_cache;
#else
        cache = sub->snb_cache;
#endif

        if (!memcmp(cache, data, 21)) {
            return sub;
        }
    }

    return NULL;
}

void cache_add(uint8_t data[21], struct bt_mesh_subnet *sub, bool private_beacon)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    if (private_beacon) {
        memcpy(sub->mpb_cache, data, 21);
    } else
#endif
    {
        memcpy(sub->snb_cache, data, 21);
    }
}

static void secure_beacon_complete(int err, void *user_data)
{
    struct bt_mesh_subnet *sub = NULL;
    uint16_t net_idx = BLE_MESH_KEY_UNUSED;

    BT_DBG("err %d", err);

    net_idx = (uint16_t)NET_IDX_GET(user_data);

    /* For node, directly updating the "beacon_sent" timestamp is fine,
     * since the subnet is pre-allocated.
     * For Provisioner, before updating the "beacon_sent" timestamp, we
     * need to make sure that the subnet still exists, because there is
     * a chance that the subnet is removed just before the completion of
     * sending the Secure Network Beacon.
     */
    sub = bt_mesh_subnet_get(net_idx);
    if (sub) {
        sub->snb_sent = k_uptime_get_32();
    }
}

void bt_mesh_secure_beacon_create(struct bt_mesh_subnet *sub,
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

    BT_DBG("SNB: net_idx 0x%03x iv_index 0x%08x flags 0x%02x",
            sub->net_idx, bt_mesh.iv_index, flags);
    BT_DBG("SNB: NetID %s Auth %s", bt_hex(keys->net_id, 8),
            bt_hex(sub->auth, 8));
}

static int secure_beacon_send(void)
{
    static const struct bt_mesh_send_cb send_cb = {
        .end = secure_beacon_complete,
    };
    uint32_t now = k_uptime_get_32();
    size_t subnet_size = 0U;
    int i = 0;

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);
        struct net_buf *buf;
        uint32_t time_diff;

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        time_diff = now - sub->snb_sent;
        if (time_diff < K_SECONDS(600) &&
            time_diff < BEACON_THRESHOLD(sub->snb_last)) {
            continue;
        }

        /* If a node enables the Proxy Client functionality, and it
         * succeeds to send Secure Network Beacon with GATT bearer,
         * here we will continue to send Secure Network Beacon of
         * other subnets.
         */
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
        if (bt_mesh_proxy_client_beacon_send(sub, false)) {
            continue;
        }
#endif

        buf = bt_mesh_adv_create(BLE_MESH_ADV_BEACON, K_NO_WAIT);
        if (!buf) {
            BT_ERR("Out of secure beacon buffer");
            return -ENOBUFS;
        }

        bt_mesh_secure_beacon_create(sub, &buf->b);

        /* Care should be taken here. Previously the user_data is the
         * pointer of a subnet. When the device is a Provisioner, its
         * subnet is created dynamically. If the corresponding subnet
         * is removed right after the Secure Network Beacon is sent,
         * update its "snb_sent" timestamp in secure_beacon_complete()
         * will cause exception.
         * Here we use the "net_idx" of the subnet instead. And in the
         * secure_beacon_complete(), we will try to get the subnet before
         * updating its "snb_sent" timestamp.
         */
        bt_mesh_adv_send(buf, SNB_XMIT, &send_cb, NET_IDX_SET(sub->net_idx));
        net_buf_unref(buf);
    }

    return 0;
}

#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_ADV)
static int unprovisioned_beacon_send(void)
{
    uint8_t uri_hash[16] = {0};
    struct net_buf *buf = NULL;
    uint16_t oob_info = 0U;

    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    buf = bt_mesh_adv_create(BLE_MESH_ADV_BEACON, K_NO_WAIT);
    if (!buf) {
        BT_ERR("Out of unprov beacon buffer");
        return -ENOBUFS;
    }

    net_buf_add_u8(buf, BEACON_TYPE_UNPROVISIONED);
    net_buf_add_mem(buf, bt_mesh_prov_get()->uuid, 16);

    if (bt_mesh_prov_get()->uri &&
        bt_mesh_s1(bt_mesh_prov_get()->uri, uri_hash) == 0) {
        oob_info = bt_mesh_prov_get()->oob_info | BLE_MESH_PROV_OOB_URI;
    } else {
        oob_info = bt_mesh_prov_get()->oob_info;
    }

    net_buf_add_be16(buf, oob_info);
    net_buf_add_mem(buf, uri_hash, 4);

    bt_mesh_adv_send(buf, UNPROV_XMIT, NULL, NULL);
    net_buf_unref(buf);

    if (bt_mesh_prov_get()->uri) {
        size_t len = 0;

        buf = bt_mesh_adv_create(BLE_MESH_ADV_URI, K_NO_WAIT);
        if (!buf) {
            BT_ERR("Unable to allocate URI buffer");
            return -ENOBUFS;
        }

        len = strlen(bt_mesh_prov_get()->uri);

        if (net_buf_tailroom(buf) < len) {
            BT_WARN("Too long URI to fit advertising data");
        } else {
            net_buf_add_mem(buf, bt_mesh_prov_get()->uri, len);
            bt_mesh_adv_send(buf, UNPROV_XMIT, NULL, NULL);
        }

        net_buf_unref(buf);
    }

    return 0;
}
#else /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_ADV) */
static int unprovisioned_beacon_send(void)
{
    return 0;
}
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_ADV) */

void update_beacon_observation(bool private_beacon)
{
    static bool snb_first_half;
    size_t subnet_size = 0U;
    int i = 0;

    /* Observation period is 20 seconds, whereas the beacon timer
     * runs every 10 seconds. We process what's happened during
     * the window only after the second half.
     */
#if CONFIG_BLE_MESH_PRB_SRV
    static bool mpb_first_half;

    if (private_beacon) {
        mpb_first_half = !mpb_first_half;
        if (mpb_first_half) {
            return;
        }
    } else
#endif
    {
        snb_first_half = !snb_first_half;
        if (snb_first_half) {
            return;
        }
    }

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

#if CONFIG_BLE_MESH_PRB_SRV
        if (private_beacon) {
            sub->mpb_last = sub->mpb_cur;
            sub->mpb_cur = 0U;
        } else
#endif
        {
            sub->snb_last = sub->snb_cur;
            sub->snb_cur = 0U;
        }
    }
}

static bool ready_to_send(void)
{
    if (bt_mesh_is_provisioned() || bt_mesh_is_provisioner_en()) {
        return true;
    }
    return false;
}

static void secure_beacon_send_timeout(struct k_work *work)
{
    /* Don't send anything if we have an active provisioning link */
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
        IS_ENABLED(CONFIG_BLE_MESH_PROV) && bt_mesh_prov_active()) {
        k_delayed_work_submit(&snb_timer, UNPROV_BEACON_INTERVAL);
        return;
    }

    if (ready_to_send()) {
        update_beacon_observation(false);

        secure_beacon_send();

        /* Only resubmit if beaconing is still enabled */
        if (bt_mesh_secure_beacon_get() == BLE_MESH_SECURE_BEACON_ENABLED ||
            bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_INITIATOR)) {
            k_delayed_work_submit(&snb_timer, SECURE_BEACON_INTERVAL);
        }
    } else {
        if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node()) {
            unprovisioned_beacon_send();
            k_delayed_work_submit(&snb_timer, UNPROV_BEACON_INTERVAL);
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

    if (buf->len != 21) {
        BT_ERR("Malformed secure beacon (len %u)", buf->len);
        return;
    }

    sub = cache_check(buf->data, false);
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

    sub = bt_mesh_subnet_find_with_snb(net_id, flags, iv_index, auth, &new_key);
    if (!sub) {
        BT_DBG("No subnet that matched secure beacon");
        return;
    }

    if (sub->kr_phase == BLE_MESH_KR_PHASE_2 && !new_key) {
        BT_WARN("Ignoring Phase 2 KR Update secured using old key");
        return;
    }

    cache_add(data, sub, false);

    /* If we have NetKey0 accept initiation only from it.
     *
     * Spec v1.1, Section 3.8.4:
     * If a device on a primary subnet receives an update on
     * the primary subnet, it shall propagate the IV update
     * to all other subnets. If a device on a primary subnet
     * receives an IV update on any other subnet, the update
     * shall be ignored.
     */
    if (bt_mesh_primary_subnet_exist() &&
        sub->net_idx != BLE_MESH_KEY_PRIMARY &&
        BLE_MESH_IV_UPDATE(flags) &&
        !BLE_MESH_KEY_REFRESH(flags)) {
        BT_WARN("Ignoring secure beacon on non-primary subnet");
        goto update_stats;
    }

    BT_DBG("SNB: net_idx 0x%03x iv_index 0x%08x current iv_index 0x%08x",
            sub->net_idx, iv_index, bt_mesh.iv_index);

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_INITIATOR) &&
        (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS) ==
         BLE_MESH_IV_UPDATE(flags))) {
        bt_mesh_beacon_ivu_initiator(false);
    }

    /* If a node on a primary subnet receives an IV update on any other subnet,
     * the IV update shall be ignored. And if a node on a non-primary subnet
     * receives an IV update on primary subnet, the IV update shall be ignored,
     * because it doesn't have a primary network key.
     */
    if ((bt_mesh_primary_subnet_exist() && sub->net_idx == BLE_MESH_KEY_PRIMARY) ||
        (!bt_mesh_primary_subnet_exist() && sub->net_idx != BLE_MESH_KEY_PRIMARY)) {
        iv_change = bt_mesh_net_iv_update(iv_index, BLE_MESH_IV_UPDATE(flags));
    }

    kr_change = bt_mesh_kr_update(sub, BLE_MESH_KEY_REFRESH(flags), new_key);
    if (kr_change) {
        bt_mesh_net_secure_beacon_update(sub);
    }

    if (iv_change) {
        /* Update all subnets */
        bt_mesh_net_sec_update(NULL);
    } else if (kr_change) {
        /* Key Refresh without IV Update only impacts one subnet */
        bt_mesh_net_sec_update(sub);
    }

update_stats:
    if (bt_mesh_secure_beacon_get() == BLE_MESH_SECURE_BEACON_ENABLED &&
        sub->snb_cur < 0xff) {
        sub->snb_cur++;
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
            IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            bt_mesh_is_provisioner_en()) {
            bt_mesh_provisioner_unprov_beacon_recv(buf, rssi);
        }

#if CONFIG_BLE_MESH_RPR_SRV
        if (bt_mesh_is_provisioned()) {
            const bt_mesh_addr_t *addr = bt_mesh_get_unprov_dev_addr();
            bt_mesh_unprov_dev_fifo_enqueue(buf->data, addr->val, bt_mesh_get_adv_type());
            bt_mesh_rpr_srv_unprov_beacon_recv(buf, bt_mesh_get_adv_type(), addr, rssi);
        }
#endif
        break;
    case BEACON_TYPE_SECURE:
        secure_beacon_recv(buf);
        break;
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    case BEACON_TYPE_PRIVATE:
        bt_mesh_private_beacon_recv(buf);
        break;
#endif
    default:
        BT_DBG("Unknown beacon type 0x%02x", type);
        break;
    }
}

void bt_mesh_beacon_init(void)
{
    /* secure beacon init */
    if (k_delayed_work_init(&snb_timer, secure_beacon_send_timeout)) {
        BT_ERR("Failed to create a snb_timer");
        return;
    }

#if CONFIG_BLE_MESH_PRB_SRV
    /* private beacon init */
    if (bt_mesh_private_beacon_timer_init()) {
        BT_ERR("Failed to create a mpb_timer");
        return;
    }
#endif
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_beacon_deinit(void)
{
    /* secure beacon deinit */
    k_delayed_work_free(&snb_timer);

#if CONFIG_BLE_MESH_PRB_SRV
    /* private beacon deinit */
    bt_mesh_private_beacon_timer_free();
#endif
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_beacon_ivu_initiator(bool enable)
{
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_INITIATOR, enable);

    if (enable) {
        k_delayed_work_submit(&snb_timer, K_NO_WAIT);
#if CONFIG_BLE_MESH_PRB_SRV
        if (bt_mesh_private_beacon_state_get() == BLE_MESH_PRIVATE_BEACON_ENABLED) {
            bt_mesh_private_beacon_timer_submit(K_NO_WAIT);
        }
#endif
    } else {
        if (bt_mesh_secure_beacon_get() == BLE_MESH_SECURE_BEACON_DISABLED) {
            k_delayed_work_cancel(&snb_timer);
        }
#if CONFIG_BLE_MESH_PRB_SRV
        if (bt_mesh_private_beacon_state_get() == BLE_MESH_PRIVATE_BEACON_DISABLED) {
            bt_mesh_private_beacon_timer_cancel();
        }
#endif
    }
}

void bt_mesh_secure_beacon_enable(void)
{
    size_t subnet_size = 0U;
    int i = 0;

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) &&
        bt_mesh_is_node() && !bt_mesh_is_provisioned()) {
        k_delayed_work_submit(&snb_timer, K_NO_WAIT);
        return;
    }

    subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        sub->snb_last = 0U;
        sub->snb_cur = 0U;

        bt_mesh_net_secure_beacon_update(sub);
    }

    k_delayed_work_submit(&snb_timer, K_NO_WAIT);
}

void bt_mesh_secure_beacon_disable(void)
{
    if (!bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_INITIATOR)) {
        k_delayed_work_cancel(&snb_timer);
    }
}
