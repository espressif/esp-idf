/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include "esp_hci_driver.h"
#include "esp_hci_internal.h"

typedef struct {
    hci_driver_forward_fn *forward_cb;
    hci_driver_host_recv_fn *host_recv_cb;
} hci_driver_vhci_env_t;

static hci_driver_vhci_env_t s_hci_driver_vhci_env;

static int
hci_driver_vhci_host_recv(hci_driver_data_type_t type, uint8_t *data, uint16_t len)
{
    static hci_driver_host_recv_fn *host_recv_cb;

    host_recv_cb = s_hci_driver_vhci_env.host_recv_cb;
    if (host_recv_cb) {
        return host_recv_cb(type, data, len);
    }

    return -1;
}

static int
hci_driver_vhci_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                   hci_driver_direction_t dir)
{
#if 1
    hci_driver_packet_t *pkt;

    if (dir == HCI_DRIVER_DIR_BTDMC2H) {
        uint8_t *hci_evt = r_ble_hci_trans_buf_alloc(ESP_HCI_INTERNAL_BUF_CMD);
        assert(hci_evt);
        pkt = (hci_driver_packet_t *)data;
        memcpy(hci_evt, pkt->data, length);
        btdm_hci_trans_buf_free(pkt);
        data = hci_evt;
    }
#endif
    return hci_driver_vhci_host_recv(data_type, data, length);
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

/* Special APIs used by host to send message. */
int
hci_driver_host_cmd_tx(uint8_t *data)
{
#if 0
    hci_driver_packet_t *pkt = NULL;
    uint16_t pkt_len;

    pkt_len = data[2] + 3;
    pkt = btdm_hci_trans_buf_alloc(HCI_DRIVER_TYPE_CMD, 0);
    assert(pkt);
    pkt->type = HCI_DRIVER_TYPE_CMD;
    pkt->length = pkt_len;
    memcpy(pkt->data, &data[0], pkt_len);
    r_ble_hci_trans_buf_free(data);
    return s_hci_driver_vhci_env.forward_cb(HCI_DRIVER_TYPE_CMD, (uint8_t *)pkt, pkt_len, HCI_DRIVER_DIR_H2C, HCI_DRIVER_CMD);
#else
    return s_hci_driver_vhci_env.forward_cb(HCI_DRIVER_TYPE_CMD, data, 0, HCI_DRIVER_DIR_H2C, HCI_DRIVER_CMD);
#endif
}

int
hci_driver_host_acl_tx(uint8_t *data, uint32_t length)
{
    return s_hci_driver_vhci_env.forward_cb(HCI_DRIVER_TYPE_ACL, data, length, HCI_DRIVER_DIR_H2C, HCI_DRIVER_LE_ACL);
}

#if CONFIG_BT_LE_ISO_SUPPORT
int
hci_driver_host_iso_tx(uint8_t *data, uint32_t length)
{
    return s_hci_driver_vhci_env.forward_cb(HCI_DRIVER_TYPE_ISO, data, length, HCI_DRIVER_DIR_H2C, HCI_DRIVER_LE_ISO);
}
#endif // CONFIG_BT_LE_ISO_SUPPORT

int
hci_driver_host_callback_register(hci_driver_host_recv_fn *callback)
{
    s_hci_driver_vhci_env.host_recv_cb = callback;
    return 0;
}
