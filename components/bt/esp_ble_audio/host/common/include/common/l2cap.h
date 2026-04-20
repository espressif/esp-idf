/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_L2CAP_H_
#define HOST_COMMON_L2CAP_H_

#include <stdint.h>

#include "sdkconfig.h"

#include "nimble/l2cap.h"

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

int bt_le_l2cap_accept(uint16_t conn_handle, uint16_t psm,
                       uint16_t scid, uint16_t mtu,
                       uint16_t mps, uint16_t credits,
                       uint16_t *result);

void bt_le_l2cap_connected(uint16_t conn_handle, uint16_t psm,
                           uint16_t tx_cid, uint16_t tx_mtu,
                           uint16_t rx_cid, uint16_t rx_mtu);

void bt_le_l2cap_disconnected(uint16_t conn_handle, uint16_t psm);

void bt_le_l2cap_received(uint16_t conn_handle, uint16_t psm,
                          uint8_t *data, uint16_t len);

int bt_le_l2cap_init(void);

void bt_le_l2cap_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_L2CAP_H_ */
