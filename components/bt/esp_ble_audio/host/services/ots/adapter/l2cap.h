/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OTS_ADAPTER_L2CAP_H_
#define OTS_ADAPTER_L2CAP_H_

#include <stdint.h>

#include "sdkconfig.h"

#include <zephyr/sys/util.h>

#if CONFIG_BT_BLUEDROID_ENABLED
#include "bluedroid/l2cap.h"
#else
#include "nimble/l2cap.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define L2CAP_LE_SUCCESS                0x0000
#define L2CAP_LE_ERR_PSM_NOT_SUPP       0x0002
#define L2CAP_LE_ERR_NO_RESOURCES       0x0004
#define L2CAP_LE_ERR_AUTHENTICATION     0x0005
#define L2CAP_LE_ERR_AUTHORIZATION      0x0006
#define L2CAP_LE_ERR_KEY_SIZE           0x0007
#define L2CAP_LE_ERR_ENCRYPTION         0x0008
#define L2CAP_LE_ERR_INVALID_SCID       0x0009
#define L2CAP_LE_ERR_SCID_IN_USE        0x000A
#define L2CAP_LE_ERR_UNACCEPT_PARAMS    0x000B
#define L2CAP_LE_ERR_INVALID_PARAMS     0x000C

#define L2CAP_LE_OTS_PSM                0x0025
#define L2CAP_LE_OTS_MTU                MIN(CONFIG_BT_OTS_L2CAP_CHAN_TX_MTU, \
                                            CONFIG_BT_OTS_L2CAP_CHAN_RX_MTU)
#define L2CAP_OTS_MAX_CHAN              CONFIG_BT_MAX_CONN

enum bt_le_l2cap_event_type {
    BT_LE_L2CAP_EVENT_CONNECTED,
    BT_LE_L2CAP_EVENT_DISCONNECTED,
    BT_LE_L2CAP_EVENT_RECEIVED,
    BT_LE_L2CAP_EVENT_SENT,
#if CONFIG_BT_BLUEDROID_ENABLED
    /* Bluedroid answers an inbound connect request out-of-band via
     * L2CA_ConnectLECocRsp(), so unlike NimBLE it needs no inline verdict. */
    BT_LE_L2CAP_EVENT_ACCEPT,
#endif /* CONFIG_BT_BLUEDROID_ENABLED */
};

struct bt_le_l2cap_event {
    uint8_t type;
    uint16_t conn_handle;
    uint16_t psm;

    union {
        struct {
            uint16_t tx_cid;
            uint16_t tx_mtu;
            uint16_t rx_cid;
            uint16_t rx_mtu;
        } connected;

        struct {
            uint8_t *data;      /* heap copy, freed after dispatch */
            uint16_t len;
        } received;

#if CONFIG_BT_BLUEDROID_ENABLED
        struct {
            uint16_t chan_handle;
            uint8_t l2cap_id;
        } accept;
#endif /* CONFIG_BT_BLUEDROID_ENABLED */
    };
};

#if CONFIG_BT_BLUEDROID_ENABLED
int bt_le_l2cap_post_accept(uint16_t conn_handle, uint16_t psm,
                            uint16_t chan_handle, uint8_t l2cap_id);
#else /* !CONFIG_BT_BLUEDROID_ENABLED */
int bt_le_l2cap_accept_safe(uint16_t conn_handle, uint16_t psm,
                            uint16_t scid, uint16_t mtu,
                            uint16_t mps, uint16_t credits,
                            uint16_t *result);
#endif /* !CONFIG_BT_BLUEDROID_ENABLED */

int bt_le_l2cap_post_connected(uint16_t conn_handle, uint16_t psm,
                               uint16_t tx_cid, uint16_t tx_mtu,
                               uint16_t rx_cid, uint16_t rx_mtu);

int bt_le_l2cap_post_disconnected(uint16_t conn_handle, uint16_t psm);

int bt_le_l2cap_post_received(uint16_t conn_handle, uint16_t psm,
                              const uint8_t *data, uint16_t len);

/* Neither host reports a per-SDU TX completion for LE CoC, so each adapter
 * synthesises it from its write's return code plus its credit-restored event. */
int bt_le_l2cap_post_sent(uint16_t conn_handle, uint16_t psm);

void bt_le_l2cap_handle_event(void *data, size_t data_len);

void bt_le_l2cap_event_free(void *data);

int bt_le_l2cap_ots_init(void);

void bt_le_l2cap_ots_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* OTS_ADAPTER_L2CAP_H_ */
