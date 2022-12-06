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
#include "scan.h"
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

static bool mesh_init = false;

bool bt_mesh_is_initialized(void)
{
    return mesh_init;
}

int bt_mesh_provision(const uint8_t net_key[16], uint16_t net_idx,
                      uint8_t flags, uint32_t iv_index, uint16_t addr,
                      const uint8_t dev_key[16])
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
        if (bt_mesh_proxy_server_prov_disable(false) == 0) {
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
            bt_mesh_proxy_server_prov_enable();
        }

        return err;
    }

    bt_mesh.seq = 0U;

    bt_mesh_comp_provision(addr);

    memcpy(bt_mesh.dev_key, dev_key, 16);

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        IS_ENABLED(CONFIG_BLE_MESH_LPN_SUB_ALL_NODES_ADDR)) {
        bt_mesh_lpn_group_add(BLE_MESH_ADDR_ALL_NODES);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        BT_DBG("Storing network information persistently");
        bt_mesh_store_net();
        bt_mesh_store_subnet(&bt_mesh.sub[0]);
        bt_mesh_store_iv(false);
    }

    bt_mesh_net_start();

    return 0;
}

void bt_mesh_node_reset(void)
{
    if (!bt_mesh_is_provisioned()) {
        BT_WARN("%s, Not provisioned", __func__);
        return;
    }

    bt_mesh.iv_index = 0U;
    bt_mesh.seq = 0U;

    bt_mesh_atomic_clear_bit(bt_mesh.flags, BLE_MESH_VALID);

    k_delayed_work_cancel(&bt_mesh.ivu_timer);

    bt_mesh_cfg_reset(true);

    bt_mesh_rx_reset(true);
    bt_mesh_tx_reset();

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        if (IS_ENABLED(CONFIG_BLE_MESH_LPN_SUB_ALL_NODES_ADDR)) {
            uint16_t group = BLE_MESH_ADDR_ALL_NODES;

            bt_mesh_lpn_group_del(&group, 1);
        }

        bt_mesh_lpn_disable(true);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_clear_net_idx(BLE_MESH_KEY_ANY);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        bt_mesh_proxy_server_gatt_disable();
    }

    (void)memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));

    bt_mesh_scan_disable();
    bt_mesh_beacon_disable();

    bt_mesh_comp_unprovision();

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_net();
        bt_mesh_clear_seq();
        bt_mesh_clear_role();
    }

    memset(bt_mesh.flags, 0, sizeof(bt_mesh.flags));

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

static bool prov_bearers_valid(bt_mesh_prov_bearer_t bearers)
{
    if ((!(bearers & (BLE_MESH_PROV_ADV | BLE_MESH_PROV_GATT))) ||
        (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            !IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            !(bearers & BLE_MESH_PROV_ADV)) ||
        (!IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            !(bearers & BLE_MESH_PROV_GATT))) {
        BT_ERR("Invalid bearers 0x%02x", bearers);
        return false;
    }
    return true;
}

int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (prov_bearers_valid(bearers) == false) {
        return -EINVAL;
    }

    /* Add this judgement here in case the device worked as a
     * Provisioner previously. Before the corresponding info
     * of Provisioner is erased from flash, users try to use
     * the device as a node, which will cause the information
     * in NVS been handled incorrectly.
     */
    uint8_t role = bt_mesh_atomic_get(bt_mesh.flags) & BLE_MESH_SETTINGS_ROLE_BIT_MASK;
    if (role != BLE_MESH_SETTINGS_ROLE_NONE &&
        role != BLE_MESH_SETTINGS_ROLE_NODE) {
        BT_ERR("%s, Mismatch role %u", __func__, role);
        return -EIO;
    }

    if (role == BLE_MESH_SETTINGS_ROLE_NONE) {
        bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_NODE);
    }
    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS_BACKWARD_COMPATIBILITY) ||
        role == BLE_MESH_SETTINGS_ROLE_NONE) {
        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_role();
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            (bearers & BLE_MESH_PROV_ADV)) {
        /* Make sure we're scanning for provisioning invitations */
        bt_mesh_scan_enable();
        /* Enable unprovisioned beacon sending */
        bt_mesh_beacon_enable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_proxy_server_prov_enable();
        bt_mesh_adv_update();
    }

    return 0;
}

int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
{
    if (bt_mesh_is_provisioned()) {
        BT_WARN("%s, Already provisioned", __func__);
        return -EALREADY;
    }

    if (prov_bearers_valid(bearers) == false) {
        return -EINVAL;
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
        bt_mesh_proxy_server_prov_disable(true);
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
        BT_WARN("Disabling scanning failed (err %d)", err);
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
        int32_t period_ms = bt_mesh_model_pub_period_get(mod);

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
        BT_WARN("Re-enabling scanning failed (err %d)", err);
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

    if (mesh_init == true) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    bt_mesh_mutex_init();

    bt_mesh_timer_init();

    bt_mesh_hci_init();

    bt_mesh_adapt_init();

    err = bt_mesh_comp_register(comp);
    if (err) {
        return err;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROXY)) {
        bt_mesh_gatt_init();
    }

    if ((IS_ENABLED(CONFIG_BLE_MESH_NODE) &&
        IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) ||
        IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        bt_mesh_proxy_server_init();
    }

    if ((IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
        IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) ||
        IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)) {
        bt_mesh_proxy_client_init();
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
        uint32_t delay = 0;
        bt_mesh_rand(&delay, sizeof(uint32_t));
        vTaskDelay((delay % 3000) / portTICK_PERIOD_MS);
    }

    bt_mesh_beacon_init();

    bt_mesh_adv_init();

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER)) {
        bt_mesh_provisioner_init();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_settings_init();
    }

    mesh_init = true;
    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_deinit(struct bt_mesh_deinit_param *param)
{
    int err = 0;

    if (param == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (mesh_init == false) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    bt_mesh_scan_disable();
    bt_mesh_beacon_disable();

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node()) {
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            !bt_mesh_is_provisioned()) {
            bt_mesh_proxy_server_prov_disable(true);
        }

        if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            bt_mesh_is_provisioned()) {
            bt_mesh_proxy_server_gatt_disable();
        }

        if (bt_mesh_is_provisioned()) {
            /* Clear valid flag here in order to perform settings erase */
            bt_mesh_atomic_clear_bit(bt_mesh.flags, BLE_MESH_VALID);

            bt_mesh_cfg_reset(param->erase);
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_proxy_client_prov_disable();
        }

        /* Clear valid flag here in order to perform settings erase */
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

            err = bt_mesh_provisioner_deinit(param->erase);
            if (err) {
                return err;
            }
        }
    }

    bt_mesh_trans_deinit(param->erase);
    bt_mesh_net_deinit();

    bt_mesh_beacon_deinit();

    if ((IS_ENABLED(CONFIG_BLE_MESH_NODE) &&
        IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) ||
        IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        bt_mesh_proxy_server_deinit();
    }

    if ((IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
        IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) ||
        IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT)) {
        bt_mesh_proxy_client_deinit();
    }

    bt_mesh_gatt_deinit();

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_deinit();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_deinit();
    }

    bt_mesh_adv_deinit();

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_settings_deinit(param->erase);
    }

    err = bt_mesh_comp_deregister();
    if (err) {
        return err;
    }

    bt_mesh_comp_unprovision();

    memset(bt_mesh.flags, 0, sizeof(bt_mesh.flags));

    bt_mesh_timer_deinit();

    bt_mesh_mutex_deinit();

    mesh_init = false;
    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

#if defined(CONFIG_BLE_MESH_PROVISIONER)
int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers)
{
    int err = 0;

    if (bt_mesh_is_provisioner_en()) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (prov_bearers_valid(bearers) == false) {
        return -EINVAL;
    }

    /* Add this judgement here in case the device worked as a
     * node previously. Before the corresponding information
     * of the node is erased from flash, users try to use the
     * device as a Provisioner, which will cause the information
     * in NVS been handled incorrectly.
     */
    uint8_t role = bt_mesh_atomic_get(bt_mesh.flags) & BLE_MESH_SETTINGS_ROLE_BIT_MASK;
    if (role != BLE_MESH_SETTINGS_ROLE_NONE &&
        role != BLE_MESH_SETTINGS_ROLE_PROV) {
        BT_ERR("%s, Mismatch role %u", __func__, role);
        return -EIO;
    }

    if (role == BLE_MESH_SETTINGS_ROLE_NONE) {
        bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_PROVISIONER);

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_role();
        }
    }

    /* Enable Provisioner here, because during the following net
     * creation, some information needs to be stored in flash.
     */
    bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_VALID_PROV);

    err = bt_mesh_provisioner_net_create();
    if (err) {
        BT_ERR("Failed to create network");
        return err;
    }

    err = bt_mesh_provisioner_init_prov_info();
    if (err) {
        BT_ERR("Failed to init prov info");
        return err;
    }

    bt_mesh_provisioner_set_prov_bearer(bearers, false);

    bt_mesh_comp_provision(bt_mesh_provisioner_get_primary_elem_addr());

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            (bearers & BLE_MESH_PROV_ADV)) {
        bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                        BLE_MESH_EXCEP_LIST_TYPE_MESH_BEACON, NULL);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                        BLE_MESH_EXCEP_LIST_TYPE_MESH_PROV_ADV, NULL);
    }
#endif

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_proxy_client_prov_enable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_init();
    }

    if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    }

    bt_mesh_scan_enable();

    return 0;
}

int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers)
{
    bt_mesh_prov_bearer_t enable = 0U;

    if (!bt_mesh_is_provisioner_en()) {
        BT_WARN("%s, Already", __func__);
        return -EALREADY;
    }

    if (prov_bearers_valid(bearers) == false) {
        return -EINVAL;
    }

    enable = bt_mesh_provisioner_get_prov_bearer();
    if (!(enable & bearers)) {
        BT_ERR("Mismatch bearers 0x%02x", bearers);
        return -EINVAL;
    }

    bt_mesh_provisioner_set_prov_bearer(bearers, true);

    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            (enable & BLE_MESH_PROV_GATT) &&
            (bearers & BLE_MESH_PROV_GATT)) {
        bt_mesh_proxy_client_prov_disable();
#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                        BLE_MESH_EXCEP_LIST_TYPE_MESH_PROV_ADV, NULL);
#endif
    }

    if (!(enable & (~bearers))) {
        /* Provisioner is disabled completely, disable scan here */
        bt_mesh_scan_disable();

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
                (enable & BLE_MESH_PROV_ADV)) {
            bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                            BLE_MESH_EXCEP_LIST_TYPE_MESH_BEACON, NULL);
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
