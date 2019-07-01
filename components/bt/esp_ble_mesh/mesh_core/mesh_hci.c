/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include "stack/bt_types.h"
#include "device/controller.h"

#include "mesh_hci.h"

struct bt_mesh_dev bt_mesh_dev;

void bt_mesh_hci_init(void)
{
    const uint8_t *features = controller_get_interface()->get_features_ble()->as_array;
    if (features != NULL) {
        memcpy(bt_mesh_dev.features[0], features, 8);
        memcpy(bt_mesh_dev.le.features, features, 8);
    }

    /**
     * Currently 20ms non-connectable adv interval is supported, and we need to add
     * a flag to indicate this support.
     */
#ifdef CONFIG_BLE_MESH_HCI_5_0
    bt_mesh_dev.hci_version = BLE_MESH_HCI_VERSION_5_0;
#else
    bt_mesh_dev.hci_version = controller_get_interface()->get_bt_version()->hci_version;
#endif
    bt_mesh_dev.lmp_version = controller_get_interface()->get_bt_version()->lmp_version;
    bt_mesh_dev.hci_revision = controller_get_interface()->get_bt_version()->hci_revision;
    bt_mesh_dev.lmp_subversion = controller_get_interface()->get_bt_version()->lmp_subversion;
    bt_mesh_dev.manufacturer = controller_get_interface()->get_bt_version()->manufacturer;

    const uint8_t *p = controller_get_interface()->get_ble_supported_states();
    uint64_t states_fh = 0, states_sh = 0;
    STREAM_TO_UINT32(states_fh, p);
    STREAM_TO_UINT32(states_sh, p);
    bt_mesh_dev.le.states = (states_sh << 32) | states_fh;
}
