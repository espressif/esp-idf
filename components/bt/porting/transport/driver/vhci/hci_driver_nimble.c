/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include "esp_hci_internal.h"
#include "esp_hci_driver.h"

typedef struct {
    hci_driver_forward_fn *forward_cb;
} hci_driver_vhci_env_t;

static hci_driver_vhci_env_t s_hci_driver_vhci_env;

static int
hci_driver_vhci_controller_tx(hci_driver_data_type_t data_type, uint8_t *data)
{
    /* The length is contained in the data. */
    return s_hci_driver_vhci_env.forward_cb(data_type, data, 0, HCI_DRIVER_DIR_C2H);
}

static int
hci_driver_vhci_host_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length)
{
    return s_hci_driver_vhci_env.forward_cb(data_type, data, length, HCI_DRIVER_DIR_H2C);
}

static int
hci_driver_vhci_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                       hci_driver_direction_t dir)
{
    int rc;

    if (dir == HCI_DRIVER_DIR_C2H) {
        rc = hci_driver_vhci_controller_tx(data_type, data);
    } else {
        rc = hci_driver_vhci_host_tx(data_type, data, length);
    }
    return rc;
}

static int
hci_driver_vhci_init(hci_driver_forward_fn *cb)
{
    s_hci_driver_vhci_env.forward_cb = cb;
    return 0;
}

static void
hci_driver_vhci_deinit(void)
{
    memset(&s_hci_driver_vhci_env, 0, sizeof(hci_driver_vhci_env_t));
}

hci_driver_ops_t hci_driver_vhci_ops = {
    .hci_driver_tx = hci_driver_vhci_tx,
    .hci_driver_init = hci_driver_vhci_init,
    .hci_driver_deinit = hci_driver_vhci_deinit,
};
