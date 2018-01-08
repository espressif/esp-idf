/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "mesh_buf.h"
#include "sdkconfig.h"

#if CONFIG_BT_MESH
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_MESH_DEBUG)
#include "mesh_trace.h"

#include "mesh_main.h"
#include "adv.h"
#include "prov.h"
#include "net.h"
#include "beacon.h"
#include "lpn.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "proxy.h"
#include "mesh.h"
#include "mesh_hci.h"

#include "provisioner_prov.h"
#include "provisioner_proxy.h"
#include "provisioner_main.h"

static volatile bool provisioner_en;

#define ACTION_ENTER    BIT(0)
#define ACTION_SUSPEND  BIT(1)
#define ACTION_EXIT     BIT(2)

#if CONFIG_BT_MESH_NODE

static bool provisioned;

int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
                      u8_t flags, u32_t iv_index, u32_t seq,
                      u16_t addr, const u8_t dev_key[16])
{
    int err;

    BT_DBG("Primary Element: 0x%04x", addr);
    BT_DBG("net_idx 0x%04x flags 0x%02x iv_index 0x%04x",
           net_idx, flags, iv_index);
    BT_DBG("Device key: %s", bt_hex(dev_key, 16));

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
        bt_mesh_proxy_prov_disable();
    }

    err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);
    if (err) {
        if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
            bt_mesh_proxy_prov_enable();
        }

        return err;
    }

    bt_mesh.seq = seq;

    bt_mesh_comp_provision(addr);

    memcpy(bt_mesh.dev_key, dev_key, 16);

    provisioned = true;

    if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    } else {
        bt_mesh_beacon_disable();
    }

    BT_DBG("%s, gatt_proxy = %d", __func__, bt_mesh_gatt_proxy_get());
    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
            bt_mesh_gatt_proxy_get() != BT_MESH_GATT_PROXY_NOT_SUPPORTED) {
        bt_mesh_proxy_gatt_enable();
        bt_mesh_adv_update();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_init();
    } else {
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
        bt_mesh_friend_init();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_complete(net_idx, addr);
    }

    return 0;
}

void bt_mesh_reset(void)
{
    if (!provisioned) {
        BT_WARN("%s: not provisioned", __func__);
        return;
    }

    bt_mesh_comp_unprovision();

    bt_mesh.iv_index = 0;
    bt_mesh.seq = 0;
    bt_mesh.iv_update = 0;
    bt_mesh.pending_update = 0;
    bt_mesh.valid = 0;
    bt_mesh.last_update = 0;
    bt_mesh.ivu_initiator = 0;

    k_delayed_work_cancel(&bt_mesh.ivu_complete);

    bt_mesh_cfg_reset();

    bt_mesh_rx_reset();
    bt_mesh_tx_reset();

    if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
        bt_mesh_lpn_disable(true);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_FRIEND)) {
        bt_mesh_friend_clear_net_idx(BT_MESH_KEY_ANY);
    }

    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
        bt_mesh_proxy_gatt_disable();
    }

    memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

    memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));

    provisioned = false;

    bt_mesh_scan_disable();
    bt_mesh_beacon_disable();

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
        bt_mesh_prov_reset();
    }
}

bool bt_mesh_is_provisioned(void)
{
    return provisioned;
}

int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
            (bearers & BT_MESH_PROV_ADV)) {
        /* Make sure we're scanning for provisioning invitations */
        bt_mesh_scan_enable();
        /* Enable unprovisioned beacon sending */
        bt_mesh_beacon_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_enable();
        bt_mesh_adv_update();
    }

    provisioner_en = false;

    return 0;
}

int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
            (bearers & BT_MESH_PROV_ADV)) {
        bt_mesh_beacon_disable();
        bt_mesh_scan_disable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_disable();
        bt_mesh_adv_update();
    }

    return 0;
}

#endif /* CONFIG_BT_MESH_NODE */

bool bt_mesh_is_provisioner_en(void)
{
    return provisioner_en;
}

#if CONFIG_BT_MESH_PROVISIONER

int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers)
{
    int err;

    if (bt_mesh_is_provisioner_en()) {
        BT_ERR("Provisioner already enabled");
        return -EALREADY;
    }

    err = provisioner_upper_init();
    if (err) {
        BT_ERR("%s: provisioner_upper_init fail", __func__);
        return err;
    }

    if ((IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
            (bearers & BT_MESH_PROV_ADV)) ||
            (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
             (bearers & BT_MESH_PROV_GATT))) {
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT)) {
        provisioner_pb_gatt_enable();
    }

    provisioner_en = true;

    return 0;
}

int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers)
{
    if (!bt_mesh_is_provisioner_en()) {
        BT_ERR("Provisioner already disabled");
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
            (bearers & BT_MESH_PROV_GATT)) {
        provisioner_pb_gatt_disable();
    }

    if ((IS_ENABLED(CONFIG_BT_MESH_PB_ADV) &&
            (bearers & BT_MESH_PROV_ADV)) &&
            (IS_ENABLED(CONFIG_BT_MESH_PB_GATT) &&
             (bearers & BT_MESH_PROV_GATT))) {
        bt_mesh_scan_disable();
    }

    provisioner_en = false;

    return 0;
}

/* Temporary Provisioner APIs */

int bt_mesh_temp_prov_set_action(u8_t action, u8_t *status)
{
    if (!status || (action > ACTION_EXIT) || (__builtin_popcount(action) > 1)) {
        return -EINVAL;
    }

    if (!action) {
        BT_WARN("%s: action is none", __func__);
        *status = 0x0;
        return 0;
    }

    if ((!provisioner_en && (action == ACTION_SUSPEND || action == ACTION_EXIT)) ||
            (provisioner_en && (action == ACTION_ENTER))) {
        BT_WARN("%s: role already exists", __func__);
        *status = 0x0;
        return 0;
    }

    if (action == ACTION_ENTER) {
        /* Disable node */
        if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
            bt_mesh_proxy_gatt_disable();
        }
        if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
            bt_mesh_beacon_disable();
        }
        if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
            provisioner_pb_gatt_enable();
        }
        provisioner_temp_prov_flag_set(true);
        provisioner_en = true;
    } else {
        if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
            provisioner_pb_gatt_disable();
        }
        if (bt_mesh_beacon_get() == BT_MESH_BEACON_ENABLED) {
            bt_mesh_beacon_enable();
        }
        if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY) &&
                bt_mesh_gatt_proxy_get() != BT_MESH_GATT_PROXY_NOT_SUPPORTED) {
            bt_mesh_proxy_gatt_enable();
            bt_mesh_adv_update();
        }

        provisioner_temp_prov_flag_set(false);
        provisioner_en = false;

        if (action == ACTION_EXIT) {
            /* clear node information in provisioner_main.c */
            provisioner_upper_reset_all_nodes();
            /* clear node information in provisioner_main.c */
            provisioner_prov_reset_all_nodes();
        }
    }

    *status = 0x0;
    return 0;
}
#endif /* CONFIG_BT_MESH_PROVISIONER */

int bt_mesh_init(const struct bt_mesh_prov *prov,
                 const struct bt_mesh_comp *comp)
{
    int err;

    //err = bt_mesh_test();
    //if (err) {
    //  return err;
    //}
    // We should init the mesh kernel first.
    mesh_k_init();

    mesh_hci_init();

    bt_mesh_adapt_init();

    err = bt_mesh_comp_register(comp);
    if (err) {
        return err;
    }

    /** We init gatt here in order to register services later */
    bt_mesh_gatt_init();

    /** Register service in the init function,
     * then start it according to bt_mesh_proxy_gatt_enable
     */
#if CONFIG_BT_MESH_NODE
    extern struct bt_gatt_service proxy_svc;
    if (IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY)) {
        bt_gatt_service_register(&proxy_svc);
    }

    /** Register service in the init function,
     * then start it according to bt_mesh_proxy_prov_enable
     */
    extern struct bt_gatt_service prov_svc;
    if (IS_ENABLED(CONFIG_BT_MESH_PB_GATT)) {
        bt_gatt_service_register(&prov_svc);
    }
#endif

    if (IS_ENABLED(CONFIG_BT_MESH_PROV)) {
#if CONFIG_BT_MESH_NODE
        err = bt_mesh_prov_init(prov);
        if (err) {
            return err;
        }
#endif
#if CONFIG_BT_MESH_PROVISIONER
        err = provisioner_prov_init(prov);
        if (err) {
            return err;
        }
#endif
    }

    bt_mesh_net_init();
    bt_mesh_trans_init();
#if CONFIG_BT_MESH_NODE
    bt_mesh_beacon_init();
#endif
    bt_mesh_adv_init();

    if (IS_ENABLED(CONFIG_BT_MESH_PROXY)) {
#if CONFIG_BT_MESH_NODE
        bt_mesh_proxy_init();
#endif
#if CONFIG_BT_MESH_PROVISIONER
        provisioner_proxy_init();
#endif
    }

#if !CONFIG_BT_MESH_NODE && CONFIG_BT_MESH_PROVISIONER
    /* If node & provisioner are both enabled and the
     * device starts with the node role, we just need
     * to wait the device been provisioned */
    err = provisioner_upper_init();
    if (err) {
        return err;
    }
#endif

    return 0;
}

#endif /* #if CONFIG_BT_MESH */
