/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh_hci.h"
#include "stack/bt_types.h"
#include "device/controller.h"
#include "sdkconfig.h"

#if CONFIG_BT_MESH

struct bt_dev bt_dev;

void mesh_hci_init(void)
{
    const uint8_t *features = controller_get_interface()->get_features_ble()->as_array;
    if (features != NULL) {
        // We just copy the ble feature here, we don't care the classic bt feature in the ble mesh system.
        memcpy(bt_dev.features[0], features, 8);
    }

    /**TODO: We support 20ms non-connectable adv interval, but not hci 5.0, so we need to add another flag
     * to indicate if we support 20ms adv interval
     * */
#ifdef CONFIG_HCI_5_0_VERSION
    bt_dev.hci_version = BT_HCI_VERSION_5_0;
#else
    bt_dev.hci_version = controller_get_interface()->get_bt_version()->hci_version;
#endif
    bt_dev.lmp_version = controller_get_interface()->get_bt_version()->lmp_version;
    bt_dev.hci_revision = controller_get_interface()->get_bt_version()->hci_revision;
    bt_dev.lmp_subversion = controller_get_interface()->get_bt_version()->lmp_subversion;
    bt_dev.manufacturer = controller_get_interface()->get_bt_version()->manufacturer;
    // Still to copy the features to the ble features struct.
    memcpy(bt_dev.le.features, features, 8);

    const uint8_t *p = controller_get_interface()->get_ble_supported_states();
    uint64_t states_fh = 0, states_sh = 0;
    // We don't have the STREAM_TO_UINT64 macro, so we should used stream to u32 two times add then put them together.
    STREAM_TO_UINT32(states_fh, p);
    STREAM_TO_UINT32(states_sh, p);
    bt_dev.le.states = (states_sh << 32) | states_fh;

#if defined(CONFIG_BT_CONN)
    bt_dev.le.mtu = controller_get_interface()->get_acl_data_size_ble();
#endif /* CONFIG_BT_CONN */

#if defined(CONFIG_BT_SMP)
    bt_dev.le.rl_size = controller_get_interface()->get_ble_resolving_list_max_size();
#endif /* #if defined(CONFIG_BT_SMP) */

}

#endif /* #if CONFIG_BT_MESH */
