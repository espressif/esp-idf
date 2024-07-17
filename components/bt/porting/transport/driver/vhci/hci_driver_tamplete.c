/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include "os/os_mbuf.h"
#include "esp_hci_transport.h"
#include "esp_hci_internal.h"
#include "esp_hci_driver.h"

typedef struct {
    hci_driver_forward_fn *forward_cb;
} hci_driver_vhci_env_t;

static hci_driver_vhci_env_t s_hci_driver_vhci_env;

static int
hci_driver_vhci_controller_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length)
{
    int rc;
    uint16_t len = 0;
    uint8_t *buf = NULL;
    struct os_mbuf *om;

    if (data_type == HCI_DRIVER_TYPE_ACL) {
        /* The ACL data will be packaged as structure of `os_mbuf`.
         * 1. Allocate a buffer suitable for the host. Use the following method to copy the data
         * from the os_mbuf to the newly allocated memory.
         * ```c
         *      buf = malloc(length);
         *      os_mbuf_copydata(om, 0, length, buf);
         * ```
         * 2. Free the controller's os_mbuf
         * ```c
         *      os_mbuf_free_chain(om);
         * ```
         */
    } else if (data_type == HCI_DRIVER_TYPE_EVT) {
        /* The event data will be packaged as an array.
         * 1. Allocate a buffer suitable for the host. Use the following method to copy the data
         * from the controller buffer to the newly allocated memory.
         * ```c
         *      buf = malloc(length);
         *      memcpy(buf, data, length);
         * ```
         * 2. Free the controller's buffer.
         * ```c
         *      r_ble_hci_trans_buf_free(data);
         * ```
         */
    } else {
        assert(0);
    }

    rc = s_hci_driver_vhci_env.forward_cb(data_type, buf, len, HCI_DRIVER_DIR_C2H);
    free(buf);

    return rc;
}

static int
hci_driver_vhci_host_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length)
{
    uint8_t *hci_data;
    struct os_mbuf *om;

    if (data_type == HCI_DRIVER_TYPE_ACL) {
        /* The ACL data needs to be packaged as structure of `os_mbuf`.
         * 1. Get an os_mbuf in the following way.
         * ```c
         *      om = os_msys_get_pkthdr(length, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
         * ```
         * 2. Copy the host's data into this os_mbuf using the following method.
         * ```c
         *      assert(os_mbuf_append(om, data, length) == 0);
         *      hci_data = (uint8_t *)om;
         * ```
         * 3. Free the host's buffer if needed.
         */
    } else if (data_type == HCI_DRIVER_TYPE_CMD) {
        /* The COMMAND data needs to be packaged as an array.
         * 1. Get a command buffer from the controller.
         * ```c
         *      hci_data = r_ble_hci_trans_buf_alloc(ESP_HCI_INTERNAL_BUF_CMD);
         * ```
         * 2. Copy the host's data into this buffer.
         * ```c
         *      memcpy(hci_data, data, length);
         * ```
         * 3. Free the host's buffer if needed.
         */
    } else {
        assert(0);
    }

    return s_hci_driver_vhci_env.forward_cb(data_type, hci_data, length, HCI_DRIVER_DIR_H2C);
}

static int
hci_driver_vhci_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                   hci_driver_direction_t dir)
{
    int rc;

    if (dir == HCI_DRIVER_DIR_C2H) {
        rc = hci_driver_vhci_controller_tx(data_type, data, length);
    } else {
        rc = hci_driver_vhci_host_tx(data_type, data, length);
    }
    return rc;
}

static int
hci_driver_vhci_init(hci_driver_forward_fn *cb)
{
    memset(&s_hci_driver_vhci_env, 0, sizeof(hci_driver_vhci_env_t));
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
