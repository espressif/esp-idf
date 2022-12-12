/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef L2CAP_HCI_LINK_INTERFACE_H
#define L2CAP_HCI_LINK_INTERFACE_H

#include "bt_common.h"

extern BOOLEAN l2c_link_hci_conn_comp (UINT8 status, UINT16 handle, BD_ADDR p_bda);
extern BOOLEAN l2c_link_hci_disc_comp (UINT16 handle, UINT8 reason);

#endif /* L2CAP_HCI_LINK_INTERFACE_H */
