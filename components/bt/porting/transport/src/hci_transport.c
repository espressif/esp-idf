/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
    hci_transport_host_recv_fn *host_recv_cb;
    hci_driver_ops_t *driver_ops;
} hci_transport_env_t;

static hci_transport_env_t s_hci_transport_env;

/* Functions for packets Rx. */
static int
hci_transport_controller_packet_rx(hci_driver_data_type_t data_type, uint8_t *data)
{
    if (data_type == HCI_DRIVER_TYPE_CMD) {
        r_ble_hci_trans_hs_cmd_tx(data);
    }

#if CONFIG_BT_CONTROLLER_ONLY || CONFIG_BT_BLUEDROID_ENABLED || \
   (CONFIG_BT_NIMBLE_ENABLED && (CONFIG_BT_NIMBLE_ROLE_CENTRAL || CONFIG_BT_NIMBLE_ROLE_PERIPHERAL))
    if (data_type == HCI_DRIVER_TYPE_ACL) {
        r_ble_hci_trans_hs_acl_tx((struct os_mbuf *) data);
    }
#endif
    return 0;
}

static int
hci_transport_host_packet_rx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length)
{
    if (!s_hci_transport_env.host_recv_cb) {
        return -1;
    }
    return s_hci_transport_env.host_recv_cb((hci_trans_pkt_ind_t)data_type, data, length);
}

static int
hci_transport_packet_rx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                        hci_driver_direction_t dir)
{
    int rc;
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        return -1;
    }

    if (dir == HCI_DRIVER_DIR_C2H) {
        rc = hci_transport_host_packet_rx(data_type, data, length);
    } else {
        rc = hci_transport_controller_packet_rx(data_type, data);
    }

    return rc;
}

/* Functions for controller Tx. */
static int
hci_transport_controller_tx_dummy(void *data, void *arg)
{
    return -1;
}

static int
hci_transport_controller_evt_tx(uint8_t *hci_ev, void *arg)
{
    uint32_t len;

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        r_ble_hci_trans_buf_free(hci_ev);
        return -1;
    }

    len = hci_ev[1] + 2;
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_EVT, hci_ev, len,
                                                         HCI_DRIVER_DIR_C2H);
}

static int
hci_transport_controller_acl_tx(struct os_mbuf *om, void *arg)
{
    uint16_t len;
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        os_mbuf_free_chain(om);
        return -1;
    }

    len = OS_MBUF_PKTHDR(om)->omp_len;
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_ACL, (uint8_t *)om, len,
                                                         HCI_DRIVER_DIR_C2H);
}

/* Functions for host Tx. */
int
hci_transport_host_cmd_tx(uint8_t *data, uint32_t length)
{
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_CMD, data, length,
                                                         HCI_DRIVER_DIR_H2C);
}

int
hci_transport_host_acl_tx(uint8_t *data, uint32_t length)
{
    return s_hci_transport_env.driver_ops->hci_driver_tx(HCI_DRIVER_TYPE_ACL, data, length,
                                                         HCI_DRIVER_DIR_H2C);
}

int
hci_transport_host_callback_register(hci_transport_host_recv_fn *callback)
{
    s_hci_transport_env.host_recv_cb = callback;
    return 0;
}

int
hci_transport_init(uint8_t hci_transport_mode)
{
    int rc;
    hci_driver_ops_t *ops;

    memset(&s_hci_transport_env, 0, sizeof(hci_transport_env_t));

    switch(hci_transport_mode) {
#if CONFIG_BT_LE_HCI_INTERFACE_USE_RAM
        case HCI_TRANSPORT_VHCI:
            ops = &hci_driver_vhci_ops;
            break;
#endif // CONFIG_BT_LE_HCI_INTERFACE_USE_RAM
#if CONFIG_BT_LE_HCI_INTERFACE_USE_UART
#if CONFIG_BT_LE_UART_HCI_DMA_MODE
        case HCI_TRANSPORT_UART_UHCI:
            ops = &hci_driver_uart_dma_ops;
            break;
#else
        case HCI_TRANSPORT_UART_NO_DMA:
            ops = &hci_driver_uart_ops;
            break;
#endif // CONFIG_BT_LE_UART_HCI_DMA_MODE
#endif // CONFIG_BT_LE_HCI_INTERFACE_USE_UART
        default:
            assert(0);
    }

    rc = ops->hci_driver_init(hci_transport_packet_rx);
    if (rc) {
        goto error;
    }

    s_hci_transport_env.driver_ops = ops;
    r_ble_hci_trans_cfg_hs(hci_transport_controller_evt_tx, NULL, hci_transport_controller_acl_tx, NULL);

    return 0;

error:
    hci_transport_deinit();
    return rc;
}

void
hci_transport_deinit(void)
{
    hci_driver_ops_t *ops;

    r_ble_hci_trans_cfg_hs((esp_hci_internal_rx_cmd_fn *)hci_transport_controller_tx_dummy, NULL,
                           (esp_hci_internal_rx_acl_fn *)hci_transport_controller_tx_dummy, NULL);

    ops = s_hci_transport_env.driver_ops;
    if (ops) {
        ops->hci_driver_deinit();
    }
    memset(&s_hci_transport_env, 0, sizeof(hci_transport_env_t));
}
