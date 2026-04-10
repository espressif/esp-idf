/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_hci_transport.h"
#include "esp_hci_internal.h"
#include "esp_bt.h"

typedef struct hci_transport_env
{
    hci_driver_ops_t *driver_ops;
} hci_transport_env_t;

static hci_transport_env_t s_hci_transport_env;

/* Functions for controller Rx. */
static int
hci_transport_controller_rx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length, hci_driver_direction_t dir, uint8_t data_source)
{
    int rc;
    hci_driver_packet_t *pkt;

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return -1;
    }

    rc = 0;
    assert(dir == HCI_DRIVER_DIR_H2C);
    if (data_type == HCI_DRIVER_TYPE_CMD) {
        pkt = (hci_driver_packet_t *)data;
        rc = btdm_hci_trans_rx(pkt);
    } else if (data_type == HCI_DRIVER_TYPE_ACL) {
        if (data_source == HCI_DRIVER_LE_ACL) {
#if UC_BT_CTRL_BLE_IS_ENABLE
            rc = r_ble_hci_trans_hs_acl_tx((struct ble_mbuf *) data);
#endif // UC_BT_CTRL_BLE_IS_ENABLE
        } else {
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
            pkt = (hci_driver_packet_t *)data;
            pkt->length = length;
            rc = bredr_hci_trans_acl_rx(pkt);
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
        }
    }
#if CONFIG_BT_LE_ISO_SUPPORT
    else if (data_type == HCI_DRIVER_TYPE_ISO) {
        rc = ble_hci_trans_hs_iso_tx(data, length, NULL);
    }
#endif // CONFIG_BT_LE_ISO_SUPPORT
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    else if (data_type == HCI_DRIVER_TYPE_SYNC) {
        pkt = (hci_driver_packet_t *)data;
        pkt->length = length;
        rc = bredr_hci_trans_sync_rx(pkt);
    }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
    else {
        rc = -1;
    }

    return rc;
}

#if UC_BT_CTRL_BLE_IS_ENABLE
/* Functions for controller Tx. */
static int
hci_transport_controller_le_evt_tx(uint8_t *hci_ev, void *arg)
{
    /* TODO: In order to support dual-host mode, use legacy interfaces for ble when nimble host is enabled */
    uint32_t len;

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        r_ble_hci_trans_buf_free(hci_ev);
        return -1;
    }

    len = hci_ev[1] + 2;
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_EVT, hci_ev, len,
                                                         HCI_DRIVER_DIR_LEC2H);
}

static int
hci_transport_controller_le_acl_tx(struct ble_mbuf *om, void *arg)
{
    /* TODO: In order to support dual-host mode, use legacy interfaces for ble when nimble host is enabled */
    uint16_t len;

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        ble_mbuf_free_chain(om);
        return -1;
    }

    len = BLE_MBUF_PKTHDR(om)->omp_len;
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_ACL, (uint8_t *)om, len,
                                                         HCI_DRIVER_DIR_LEC2H);
}
#endif // UC_BT_CTRL_BLE_IS_ENABLE

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
static int
hci_transport_controller_bredr_tx_dummy(hci_driver_packet_t *pkt, void *arg)
{
    return -1;
}

static int
hci_transport_controller_bredr_acl_tx(hci_driver_packet_t *pkt, void *arg)
{
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_ACL, (uint8_t *)pkt, pkt->length,
                                                         HCI_DRIVER_DIR_BREDRC2H);
}

static int
hci_transport_controller_bredr_sync_tx(hci_driver_packet_t *pkt, void *arg)
{
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_SYNC, (uint8_t *)pkt, pkt->length,
                                                         HCI_DRIVER_DIR_BREDRC2H);
}

static int
hci_transport_controller_bredr_evt_tx(hci_driver_packet_t *pkt, void *arg)
{
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_EVT, (uint8_t *)pkt, pkt->length,
                                                         HCI_DRIVER_DIR_BREDRC2H);
}
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

#if CONFIG_BT_LE_ISO_SUPPORT
typedef void ble_host_rx_iso_data_fn(uint8_t *data, uint16_t len);

static ble_host_rx_iso_data_fn *ble_host_iso_rx_cb = NULL;

void ble_host_register_rx_iso_data_cb(void *cb)
{
    /* If the iso rx cb is already registered, we will give
     * a warning log here, and the cb will still be updated.
     */
    if (ble_host_iso_rx_cb) {
        printf("iso rx cb %p already registered\n", ble_host_iso_rx_cb);
    }

    ble_host_iso_rx_cb = cb;
}

void ble_hci_register_rx_iso_data_cb(void *cb)
{
    ble_host_register_rx_iso_data_cb(cb);
}

static int
hci_transport_controller_le_iso_tx_dummy(const uint8_t *data, uint16_t len, void *arg)
{
    return -1;
}

static int
hci_transport_controller_le_iso_tx(const uint8_t *data, uint16_t len, void *arg)
{
    //ISO_TODO: arg is not used

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        //ISO_TODO: free this tx buffer
        return -1;
    }

#if CONFIG_BT_NIMBLE_ENABLED
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_ISO, (uint8_t *)data, len,
                                                         HCI_DRIVER_DIR_LEC2H);
#elif CONFIG_BT_BLUEDROID_ENABLED
    if (ble_host_iso_rx_cb) {
        ble_host_iso_rx_cb((uint8_t *)data, len);
    }

    free((void *)data);
#endif // CONFIG_BT_NIMBLE_ENABLED
    return 0;
}
#endif // CONFIG_BT_LE_ISO_SUPPORT

/* Functions for controller Tx. */
static int
hci_transport_controller_le_tx_dummy(void *data, void *arg)
{
    return -1;
}

static int
hci_transport_controller_tx_dummy(hci_driver_packet_t *pkt)
{
    return -1;
}

static int
hci_transport_controller_tx(hci_driver_packet_t *pkt)
{
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_INITED
            && esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return -1;
    }

    return s_hci_transport_env.driver_ops->hci_driver_tx(pkt->type, (uint8_t *)pkt, pkt->length,
                                                         HCI_DRIVER_DIR_BTDMC2H);
}

int
hci_transport_init(uint8_t hci_transport_mode)
{
    int rc;
    hci_driver_ops_t *ops;

    memset(&s_hci_transport_env, 0, sizeof(hci_transport_env_t));

    switch(hci_transport_mode) {
#if UC_BT_CTRL_HCI_INTERFACE_USE_RAM
        case HCI_TRANSPORT_VHCI:
            ops = &hci_driver_vhci_ops;
            break;
#endif // UC_BT_CTRL_HCI_INTERFACE_USE_RAM
#if UC_BT_CTRL_HCI_INTERFACE_USE_UART
#if UC_BT_CTRL_UART_HCI_DMA_MODE
        case HCI_TRANSPORT_UART_UHCI:
            ops = &hci_driver_uart_dma_ops;
            break;
#else
        case HCI_TRANSPORT_UART_NO_DMA:
            ops = &hci_driver_uart_ops;
            break;
#endif // UC_BT_CTRL_UART_HCI_DMA_MODE
#endif // UC_BT_CTRL_HCI_INTERFACE_USE_UART
        default:
            assert(0);
    }

    rc = ops->hci_driver_init(hci_transport_controller_rx);
    if (rc) {
        goto error;
    }

    s_hci_transport_env.driver_ops = ops;
#if UC_BT_CTRL_BLE_IS_ENABLE
    r_ble_hci_trans_cfg_hs((esp_hci_internal_rx_cmd_fn *)hci_transport_controller_le_evt_tx, NULL,
                           (esp_hci_internal_rx_acl_fn *)hci_transport_controller_le_acl_tx, NULL);
#if CONFIG_BT_LE_ISO_SUPPORT
    ble_iso_trans_cfg_hs((esp_hci_internal_rx_iso_fn *)hci_transport_controller_le_iso_tx, NULL);
#endif // CONFIG_BT_LE_ISO_SUPPORT

#endif // UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    bredr_hci_trans_register_tx((btdm_hci_trans_tx_func_t *)hci_transport_controller_bredr_acl_tx,
                                (btdm_hci_trans_tx_func_t *)hci_transport_controller_bredr_sync_tx,
                                (btdm_hci_trans_tx_func_t *)hci_transport_controller_bredr_evt_tx);
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
    btdm_hci_trans_register_tx((btdm_hci_trans_tx_func_t *)hci_transport_controller_tx, false);
    return 0;
error:
    hci_transport_deinit();
    return rc;
}

void
hci_transport_deinit(void)
{
    hci_driver_ops_t *ops;

    btdm_hci_trans_register_tx((btdm_hci_trans_tx_func_t *)hci_transport_controller_tx_dummy, false);
#if UC_BT_CTRL_BLE_IS_ENABLE
    r_ble_hci_trans_cfg_hs((esp_hci_internal_rx_cmd_fn *)hci_transport_controller_le_tx_dummy, NULL,
                           (esp_hci_internal_rx_acl_fn *)hci_transport_controller_le_tx_dummy, NULL);
#if CONFIG_BT_LE_ISO_SUPPORT
    ble_iso_trans_cfg_hs((esp_hci_internal_rx_iso_fn *)hci_transport_controller_le_iso_tx_dummy, NULL);
#endif // CONFIG_BT_LE_ISO_SUPPORT
#endif // UC_BT_CTRL_BLE_IS_ENABLE
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    bredr_hci_trans_register_tx((btdm_hci_trans_tx_func_t *)hci_transport_controller_bredr_tx_dummy,
                                (btdm_hci_trans_tx_func_t *)hci_transport_controller_bredr_tx_dummy,
                                (btdm_hci_trans_tx_func_t *)hci_transport_controller_bredr_tx_dummy);
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
    ops = s_hci_transport_env.driver_ops;
    if (ops) {
        ops->hci_driver_deinit();
    }
    memset(&s_hci_transport_env, 0, sizeof(hci_transport_env_t));
}
