/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLE_MESH_DEBUG)

#include "adv.h"
#include "prov.h"
#include "beacon.h"
#include "lpn.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "settings.h"
#include "mesh.h"
#include "mesh_hci.h"
#include "mesh_common.h"
#include "proxy_client.h"
#include "proxy_server.h"
#include "provisioner_prov.h"
#include "provisioner_main.h"

#define ACTION_ENTER    0x01
#define ACTION_SUSPEND  0x02
#define ACTION_EXIT     0x03

int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
                      u8_t flags, u32_t iv_index, u16_t addr,
                      const u8_t dev_key[16])
{
    bool pb_gatt_enabled = false;
    int err = 0;

    BT_INFO("Primary Element: 0x%04x", addr);
    BT_INFO("net_idx 0x%04x flags 0x%02x iv_index 0x%04x",
           net_idx, flags, iv_index);
    BT_INFO("dev_key %s", bt_hex(dev_key, 16));

    if (bt_mesh_atomic_test_and_set_bit(bt_mesh.flags, BLE_MESH_VALID)) {
        return -EALREADY;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
        if (bt_mesh_proxy_prov_disable(false) == 0) {
            pb_gatt_enabled = true;
        } else {
            pb_gatt_enabled = false;
        }
    } else {
        pb_gatt_enabled = false;
    }

    err = bt_mesh_net_create(net_idx, flags, net_key, iv_index);
    if (err) {
        bt_mesh_atomic_clear_bit(bt_mesh.flags, BLE_MESH_VALID);

        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) && pb_gatt_enabled) {
            bt_mesh_proxy_prov_enable();
        }

        return err;
    }

    bt_mesh.seq = 0U;

    bt_mesh_comp_provision(addr);

    memcpy(bt_mesh.dev_key, dev_key, 16);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        BT_DBG("Storing network information persistently");
        bt_mesh_store_net();
        bt_mesh_store_subnet(&bt_mesh.sub[0]);
        bt_mesh_store_iv(false);
    }

    /* Add this to avoid "already active status" for bt_mesh_scan_enable() */
    bt_mesh_scan_disable();

    bt_mesh_net_start();

    return 0;
}

void bt_mesh_reset(void)
{
    if (!bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_VALID)) {
        BT_WARN("%s, Not provisioned", __func__);
        return;
    }

    bt_mesh.iv_index = 0U;
    bt_mesh.seq = 0U;

    memset(bt_mesh.flags, 0, sizeof(bt_mesh.flags));

    k_delayed_work_cancel(&bt_mesh.ivu_timer);

    bt_mesh_cfg_reset();

    bt_mesh_rx_reset();
    bt_mesh_tx_reset();

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_disable(true);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_clear_net_idx(BLE_MESH_KEY_ANY);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        bt_mesh_proxy_gatt_disable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_net();
    }

    (void)memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

    bt_mesh_scan_disable();
    bt_mesh_beacon_disable();

    bt_mesh_comp_unprovision();

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_role();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROV)) {
        bt_mesh_prov_reset();
    }
}

bool bt_mesh_is_node(void)
{
    return bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_NODE);
}

bool bt_mesh_is_provisioned(void)
{
    if (bt_mesh_is_node()) {
        return bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_VALID);
    } else {
        return false;
    }
}

bool bt_mesh_is_provisioner(void)
{
    return bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_PROVISIONER);
}

bool bt_mesh_is_provisioner_en(void)
{
    if (bt_mesh_is_provisioner()) {
        return bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_VALID_PROV);
    } else {
        return false;
    }
}

int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_NODE);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_role();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            (bearers & BLE_MESH_PROV_ADV)) {
        /* Make sure we're scanning for provisioning inviations */
        bt_mesh_scan_enable();
        /* Enable unprovisioned beacon sending */
        bt_mesh_beacon_enable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_enable();
        bt_mesh_adv_update();
    }

    return 0;
}

int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        return -EALREADY;
    }

    bt_mesh_atomic_clear_bit(bt_mesh.flags, BLE_MESH_NODE);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_role();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            (bearers & BLE_MESH_PROV_ADV)) {
        bt_mesh_beacon_disable();
        bt_mesh_scan_disable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_proxy_prov_disable(true);
    }

    return 0;
}

static void model_suspend(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                          bool vnd, bool primary, void *user_data)
{
    if (mod->pub && mod->pub->update) {
        mod->pub->count = 0U;
        k_delayed_work_cancel(&mod->pub->timer);
    }
}

int bt_mesh_suspend(void)
{
    int err = 0;

    if (!bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_VALID)) {
        return -EINVAL;
    }

    if (bt_mesh_atomic_test_and_set_bit(bt_mesh.flags, BLE_MESH_SUSPENDED)) {
        return -EALREADY;
    }

    err = bt_mesh_scan_disable();
    if (err) {
        bt_mesh_atomic_clear_bit(bt_mesh.flags, BLE_MESH_SUSPENDED);
        BT_WARN("%s, Disabling scanning failed (err %d)", __func__, err);
        return err;
    }

    bt_mesh_hb_pub_disable();

    if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_disable();
    }

    bt_mesh_model_foreach(model_suspend, NULL);

    return 0;
}

static void model_resume(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                         bool vnd, bool primary, void *user_data)
{
    if (mod->pub && mod->pub->update) {
        s32_t period_ms = bt_mesh_model_pub_period_get(mod);

        if (period_ms) {
            k_delayed_work_submit(&mod->pub->timer, period_ms);
        }
    }
}

int bt_mesh_resume(void)
{
    int err = 0;

    if (!bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_VALID)) {
        return -EINVAL;
    }

    if (!bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_SUSPENDED)) {
        return -EALREADY;
    }

    err = bt_mesh_scan_enable();
    if (err) {
        BT_WARN("%s, Re-enabling scanning failed (err %d)", __func__, err);
        bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_SUSPENDED);
        return err;
    }

    if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    }

    bt_mesh_model_foreach(model_resume, NULL);

    return err;
}

int bt_mesh_init(const struct bt_mesh_prov *prov,
                 const struct bt_mesh_comp *comp)
{
    int err = 0;

    bt_mesh_k_init();

    bt_mesh_hci_init();

    bt_mesh_adapt_init();

    err = bt_mesh_comp_register(comp);
    if (err) {
        return err;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROXY)) {
        bt_mesh_gatt_init();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROV)) {
        if (IS_ENABLED(CONFIG_BLE_MESH_NODE)) {
            err = bt_mesh_prov_init(prov);
            if (err) {
                return err;
            }
        }
        if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER)) {
            err = bt_mesh_provisioner_prov_init(prov);
            if (err) {
                return err;
            }
        }
    }

    bt_mesh_net_init();
    bt_mesh_trans_init();

    /* Changed by Espressif, add a random delay (0 ~ 3s) */
    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV)) {
        u32_t delay = 0;
        bt_mesh_rand(&delay, sizeof(u32_t));
        vTaskDelay((delay % 3000) / portTICK_PERIOD_MS);
    }

    bt_mesh_beacon_init();

    bt_mesh_adv_init();

    if (IS_ENABLED(CONFIG_BLE_MESH_PROXY)) {
        if ((IS_ENABLED(CONFIG_BLE_MESH_NODE) &&
            IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) ||
            IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
            bt_mesh_proxy_init();
        }
        if ((IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
            IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) ||
            IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)) {
            bt_mesh_proxy_prov_client_init();
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER)) {
        bt_mesh_provisioner_init();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_settings_init();
    }

    return 0;
}

int bt_mesh_deinit(struct bt_mesh_deinit_param *param)
{
    int err = 0;

    if (param == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV)) {
            bt_mesh_beacon_disable();
            bt_mesh_scan_disable();
        }

        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_proxy_prov_disable(true);
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_provisioner_pb_gatt_disable();
        }

        bt_mesh_scan_disable();
        bt_mesh_atomic_clear_bit(bt_mesh.flags, BLE_MESH_VALID_PROV);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROV)) {
        if (IS_ENABLED(CONFIG_BLE_MESH_NODE)) {
            err = bt_mesh_prov_deinit();
            if (err) {
                return err;
            }
        }
        if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER)) {
            err = bt_mesh_provisioner_prov_deinit(param->erase);
            if (err) {
                return err;
            }
        }
    }

    bt_mesh_trans_deinit(param->erase);
    bt_mesh_net_deinit(param->erase);

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE)) {
        bt_mesh_beacon_deinit();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROXY)) {
        if (IS_ENABLED(CONFIG_BLE_MESH_NODE)) {
            bt_mesh_proxy_deinit();
        }
    }

    if ((IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
        IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) ||
        IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)) {
        bt_mesh_proxy_prov_client_deinit();
    }

    bt_mesh_gatt_deinit();

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER)) {
        err = bt_mesh_provisioner_deinit(param->erase);
        if (err) {
            return err;
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_deinit();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_deinit();
    }

    bt_mesh_adv_deinit();

    err = bt_mesh_comp_deregister();
    if (err) {
        return err;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        if (param->erase) {
            bt_mesh_clear_role();
        }
        bt_mesh_settings_deinit();
    }

    bt_mesh_k_deinit();

    return 0;
}

#if defined(CONFIG_BLE_MESH_PROVISIONER)
int bt_mesh_provisioner_net_start(bt_mesh_prov_bearer_t bearers)
{
    bt_mesh_provisioner_set_prov_bearer(bearers, false);

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            (bearers & BLE_MESH_PROV_ADV)) {
        bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_ADD,
                                        BLE_MESH_EXCEP_INFO_MESH_BEACON, NULL);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_ADD,
                                        BLE_MESH_EXCEP_INFO_MESH_PROV_ADV, NULL);
    }
#endif

    if ((IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            (bearers & BLE_MESH_PROV_ADV)) ||
            (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
             (bearers & BLE_MESH_PROV_GATT))) {
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_provisioner_pb_gatt_enable();
    }

    bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_VALID_PROV);

    if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_init();
    }

    return 0;
}

int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers)
{
    int err = 0;

    if (bt_mesh_is_provisioner_en()) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    err = bt_mesh_provisioner_set_prov_info();
    if (err) {
        BT_ERR("%s, Failed to set provisioning info", __func__);
        return err;
    }

    err = bt_mesh_provisioner_net_create();
    if (err) {
        BT_ERR("%s, Failed to create network", __func__);
        return err;
    }

    bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_PROVISIONER);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_role();
    }

    return bt_mesh_provisioner_net_start(bearers);
}

int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers)
{
    bt_mesh_prov_bearer_t enable = 0U;

    if (!bt_mesh_is_provisioner_en()) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    enable = bt_mesh_provisioner_get_prov_bearer();
    if (!(enable & bearers)) {
        BT_ERR("%s, Bearers mismatch", __func__);
        return -EINVAL;
    }

    bt_mesh_provisioner_set_prov_bearer(bearers, true);

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (enable & BLE_MESH_PROV_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_provisioner_pb_gatt_disable();
#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_REMOVE,
                                        BLE_MESH_EXCEP_INFO_MESH_PROV_ADV, NULL);
#endif
    }

    if (!(enable & (~bearers))) {
        /* Provisioner is disabled completely, disable scan here */
        bt_mesh_scan_disable();

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
                (enable & BLE_MESH_PROV_ADV)) {
            bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_REMOVE,
                                            BLE_MESH_EXCEP_INFO_MESH_BEACON, NULL);
        }
#endif

        /* Clear corresponding flags */
        bt_mesh_atomic_and(bt_mesh.flags, ~(BIT(BLE_MESH_PROVISIONER) | BIT(BLE_MESH_VALID_PROV)));

        /* When Provisioner is disabled, the device role indicated by bt_mesh.flags
         * will not be cleared, because when Provisioner is restarted after disabled,
         * its previous information can be recovered from flash properly.
         */

        if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
            bt_mesh_beacon_disable();
        }

        if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
            bt_mesh_friend_clear_net_idx(BLE_MESH_KEY_ANY);
        }
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PROVISIONER */

/* The following API is for fast provisioning */

#if CONFIG_BLE_MESH_FAST_PROV
u8_t bt_mesh_set_fast_prov_action(u8_t action)
{
    if (!action || action > ACTION_EXIT) {
        return 0x01;
    }

    if ((!bt_mesh_is_provisioner_en() && (action == ACTION_SUSPEND || action == ACTION_EXIT)) ||
            (bt_mesh_is_provisioner_en() && (action == ACTION_ENTER))) {
        BT_WARN("%s, Already", __func__);
        return 0x0;
    }

    if (action == ACTION_ENTER) {
#if 0
        /* If the device is provisioned using PB-GATT and connected to
         * the phone with proxy service, proxy_gatt shall not be disabled
         * here. The node needs to send some status messages to the phone
         * while it is connected.
         */
        if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
            bt_mesh_proxy_gatt_disable();
        }
#endif
        if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
            bt_mesh_beacon_disable();
        }
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_provisioner_pb_gatt_enable();
        }
        bt_mesh_provisioner_set_prov_bearer(BLE_MESH_PROV_ADV, false);
        bt_mesh_provisioner_fast_prov_enable(true);
        bt_mesh_atomic_or(bt_mesh.flags, BIT(BLE_MESH_PROVISIONER) | BIT(BLE_MESH_VALID_PROV));
    } else {
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_provisioner_pb_gatt_disable();
        }
        if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
            bt_mesh_beacon_enable();
        }
#if 0
        /* Mesh Proxy GATT will be re-enabled on application layer */
        if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
                bt_mesh_gatt_proxy_get() != BLE_MESH_GATT_PROXY_NOT_SUPPORTED) {
            bt_mesh_proxy_gatt_enable();
            bt_mesh_adv_update();
        }
#endif
        bt_mesh_atomic_and(bt_mesh.flags, ~(BIT(BLE_MESH_PROVISIONER) | BIT(BLE_MESH_VALID_PROV)));
        bt_mesh_provisioner_fast_prov_enable(false);
        if (action == ACTION_EXIT) {
            bt_mesh_provisioner_remove_node(NULL);
        }
    }

    return 0x0;
}
#endif /* CONFIG_BLE_MESH_FAST_PROV */
