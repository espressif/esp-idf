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
#include "esp_bt.h"

typedef struct {
    hci_driver_forward_fn *forward_cb;
    const esp_vhci_host_callback_t *host_recv_cb;
} hci_driver_vhci_env_t;

static hci_driver_vhci_env_t s_hci_driver_vhci_env;

static int
hci_driver_vhci_controller_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length)
{
    int rc;
    uint16_t buf_len = 0;
    uint8_t *buf = NULL;
    struct os_mbuf *om;

    if (data_type == HCI_DRIVER_TYPE_ACL) {
        om = (struct os_mbuf *)data;
        buf_len = length + 1;
        buf = malloc(buf_len);
        /* TODO: If there is no memory, should handle it in the controller. */
        assert(buf);
        buf[0] = HCI_DRIVER_TYPE_ACL;
        os_mbuf_copydata(om, 0, length, &buf[1]);
        os_mbuf_free_chain(om);
    } else if (data_type == HCI_DRIVER_TYPE_EVT) {
        buf_len = length + 1;
        buf = malloc(buf_len);
        /* TODO: If there is no memory, should handle it in the controller. */
        assert(buf != NULL);
        buf[0] = HCI_DRIVER_TYPE_EVT;
        memcpy(&buf[1], data, length);
        r_ble_hci_trans_buf_free(data);
    }

    rc = s_hci_driver_vhci_env.forward_cb(data_type, buf, buf_len, HCI_DRIVER_DIR_C2H);
    free(buf);

    return rc;
}

static int
hci_driver_vhci_host_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length)
{
    uint8_t *cmd;
    struct os_mbuf *om;

    if (data_type == HCI_DRIVER_TYPE_ACL) {
        om = os_msys_get_pkthdr(length, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
        assert(om);
        assert(os_mbuf_append(om, &data[1], length - 1) == 0);
        data = (uint8_t *)om;
    } else if (data_type == HCI_DRIVER_TYPE_CMD) {
        cmd = r_ble_hci_trans_buf_alloc(ESP_HCI_INTERNAL_BUF_CMD);
        assert(cmd);
        memcpy(cmd, data + 1, length - 1);
        data = cmd;
    }

    return s_hci_driver_vhci_env.forward_cb(data_type, data, length, HCI_DRIVER_DIR_H2C);
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

/* Special APIs declared in the `esp_bt.h'. */
static int
hci_driver_vhci_host_recv_cb(hci_trans_pkt_ind_t type, uint8_t *data, uint16_t len)
{
    static const esp_vhci_host_callback_t *host_recv_cb;

    host_recv_cb = s_hci_driver_vhci_env.host_recv_cb;
    if (host_recv_cb) {
        return host_recv_cb->notify_host_recv(data, len);
    }

    return -1;
}

esp_err_t
esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_FAIL;
    }

    s_hci_driver_vhci_env.host_recv_cb = callback;
    if(hci_transport_host_callback_register(hci_driver_vhci_host_recv_cb)) {
        s_hci_driver_vhci_env.host_recv_cb = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

void
esp_vhci_host_send_packet(uint8_t *data, uint16_t len)
{
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return;
    }

    hci_driver_vhci_tx(data[0], data, len, HCI_DRIVER_DIR_H2C);
}

bool
esp_vhci_host_check_send_available(void)
{
    return true;
}
