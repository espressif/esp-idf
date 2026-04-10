/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include "ble_mbuf.h"
#include "esp_hci_driver.h"
#include "esp_hci_internal.h"
#include "esp_bt.h"

typedef struct {
    hci_driver_forward_fn *forward_cb;
    const esp_vhci_host_callback_t *host_recv_cb;
} hci_driver_vhci_env_t;

static hci_driver_vhci_env_t s_hci_driver_vhci_env;


#if 0
typedef int ble_host_rx_iso_data_fn(uint8_t *data, uint16_t len);
static ble_host_rx_iso_data_fn *ble_host_iso_rx_cb = NULL;

void ble_hci_register_rx_iso_data_cb(void *cb)
{
    /* If the iso rx cb is already registered, we will give
     * a warning log here, and the cb will still be updated.
     */
    if (ble_host_iso_rx_cb) {
        printf("iso rx cb %p already registered\n", ble_host_iso_rx_cb);
    }

    ble_host_iso_rx_cb = cb;
}

#endif // CONFIG_BT_LE_ISO_SUPPORT

static int
hci_driver_vhci_host_recv_with_type(uint8_t data_type, uint8_t *data, uint16_t len)
{
    static const esp_vhci_host_callback_t *host_recv_cb;

    host_recv_cb = s_hci_driver_vhci_env.host_recv_cb;
    if (!host_recv_cb) {
        return -1;
    }
#if 0
    if (data_type == HCI_DRIVER_TYPE_ISO) {
        if (ble_host_iso_rx_cb) {
            return ble_host_iso_rx_cb(data, len);
        }
    } else
#endif // CONFIG_BT_LE_ISO_SUPPORT
    {
        if (host_recv_cb->notify_host_recv) {
            return host_recv_cb->notify_host_recv(data, len);
        }
    }

    return -1;
}

#if 0
static void
hci_driver_vhci_host_send_available(void)
{
    static const esp_vhci_host_callback_t *host_recv_cb;

    host_recv_cb = s_hci_driver_vhci_env.host_recv_cb;
    if (host_recv_cb && host_recv_cb->notify_host_send_available) {
        host_recv_cb->notify_host_send_available();
    }
}
#endif

static int
hci_driver_vhci_controller_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length, hci_driver_direction_t dir)
{
    int rc = 0;
    uint16_t buf_len = length + 1;;
    uint8_t *buf = NULL;
    struct ble_mbuf *om;
    uint8_t old_value = 0;
    hci_driver_packet_t *pkt = (hci_driver_packet_t *)data;

    if (data_type == HCI_DRIVER_TYPE_ACL) {
#if UC_BT_CTRL_BLE_IS_ENABLE
        if (dir == HCI_DRIVER_DIR_LEC2H) {
            om = (struct ble_mbuf *)data;
            buf = malloc(buf_len);
            /* TODO: If there is no memory, should handle it in the controller. */
            assert(buf);
            buf[0] = HCI_DRIVER_TYPE_ACL;
            ble_mbuf_copydata(om, 0, length, &buf[1]);
            ble_mbuf_free_chain(om);
            rc = hci_driver_vhci_host_recv_with_type(data_type, buf, buf_len);
            free(buf);
        }
#endif // UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        if (dir == HCI_DRIVER_DIR_BREDRC2H) {
            buf = pkt->data - 1;
            old_value = *buf;
            *buf = data_type;
            rc = hci_driver_vhci_host_recv_with_type(data_type, buf, buf_len);
            *buf = old_value;
            if (rc >= 0) {
                bredr_hci_trans_acl_tx_done(pkt);
                bredr_hci_trans_acl_free(pkt);
            }
        }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
    } else if (data_type == HCI_DRIVER_TYPE_EVT) {
        /* TODO: If there is no memory, should handle it in the controller. */
        if (dir == HCI_DRIVER_DIR_LEC2H) {
            buf = malloc(buf_len);
            assert(buf != NULL);
            buf[0] = HCI_DRIVER_TYPE_EVT;
            memcpy(&buf[1], data, length);
            rc = hci_driver_vhci_host_recv_with_type(data_type, buf, buf_len);
            r_ble_hci_trans_buf_free(data);
            free(buf);
        }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
         else if (dir == HCI_DRIVER_DIR_BREDRC2H) {
            buf = pkt->data - 1;
            old_value = *buf;
            *buf = data_type;
            rc = hci_driver_vhci_host_recv_with_type(data_type, buf, buf_len);
            *buf = old_value;
            if (rc >= 0) {
                bredr_hci_trans_evt_tx_done((hci_driver_packet_t *)data);
                bredr_hci_trans_evt_free((hci_driver_packet_t *)data);
            }
        }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
        else {
            buf = pkt->data - 1;
            old_value = *buf;
            *buf = data_type;
            rc = hci_driver_vhci_host_recv_with_type(data_type, buf, buf_len);
            *buf = old_value;
            btdm_hci_trans_buf_free(pkt);
        }

    }
#if CONFIG_BT_LE_ISO_SUPPORT
    else if (data_type == HCI_DRIVER_TYPE_ISO) {
        rc = hci_driver_vhci_host_recv_with_type(data_type, data, length);
        free(data);
    }
#endif // CONFIG_BT_LE_ISO_SUPPORT
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    else if (data_type == HCI_DRIVER_TYPE_SYNC) {
        /* SCO/ESCO data */
        buf = pkt->data - 1;
        old_value = *buf;
        *buf = data_type;
        rc = hci_driver_vhci_host_recv_with_type(data_type, buf, buf_len);
        *buf = old_value;
        if (rc >= 0) {
            bredr_hci_trans_sync_tx_done(pkt);
            bredr_hci_trans_sync_free(pkt);
        }
    }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
    else {
        assert(0);
    }

    return rc;
}

static int
hci_driver_vhci_host_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length)
{
    struct ble_mbuf *om;
    uint16_t pkt_len;
    uint16_t conn_handle;
    hci_driver_packet_t *pkt = NULL;
    uint8_t data_source = 0xFF;

    pkt_len = length - 1;
    switch (data_type) {
    case HCI_DRIVER_TYPE_CMD:
        pkt = btdm_hci_trans_buf_alloc(data_type, 0);
        assert(pkt);
        //TODO: check if pkt_len is not larger than the allocated one.
        memcpy(pkt->data, &data[1], pkt_len);
        data = (uint8_t *)pkt;
        break;

    case HCI_DRIVER_TYPE_ACL:
        conn_handle = btdm_get_le16(&data[1]) & HCI_INTERNAL_CONN_MASK;
#if UC_BT_CTRL_BLE_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BLE(conn_handle)) {
            om = ble_msys_get_pkthdr(pkt_len, ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE);
            assert(om);
            assert(ble_mbuf_append(om, &data[1], length - 1) == 0);
            data = (uint8_t *)om;
            data_source = HCI_DRIVER_LE_ACL;
        }
#endif // UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        if (HCI_INTERNAL_CONN_IS_BREDR(conn_handle)) {
            pkt = btdm_hci_trans_buf_alloc(data_type, conn_handle);
            assert(pkt);
            memcpy(pkt->data, &data[1], pkt_len);
            data = (uint8_t *)pkt;
            data_source = HCI_DRIVER_BREDR_ACL;
        }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
        break;

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    case HCI_DRIVER_TYPE_SYNC:
        conn_handle = btdm_get_le16(&data[1]) & HCI_INTERNAL_CONN_MASK;
        pkt = btdm_hci_trans_buf_alloc(data_type, conn_handle);
        assert(pkt);
        memcpy(pkt->data, &data[1], pkt_len);
        data = (uint8_t *)pkt;
        data_source = HCI_DRIVER_BREDR_SYNC;
        break;
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
    case HCI_DRIVER_TYPE_ISO:
        data_source = HCI_DRIVER_LE_ISO;
        break;
    default:
        assert(0);
        break;
    }

    if (pkt) {
        pkt->length = pkt_len;
    }

    return s_hci_driver_vhci_env.forward_cb(data_type, data, pkt_len, HCI_DRIVER_DIR_H2C, data_source);
}

static int
hci_driver_vhci_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                   hci_driver_direction_t dir)
{
    int rc;
    if (dir == HCI_DRIVER_DIR_H2C) {
        rc = hci_driver_vhci_host_tx(data_type, data, length);
    } else {
        rc = hci_driver_vhci_controller_tx(data_type, data, length, dir);
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
esp_err_t
esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback)
{
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return ESP_FAIL;
    }

    s_hci_driver_vhci_env.host_recv_cb = callback;
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
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return false;
    }
    return true;
}
