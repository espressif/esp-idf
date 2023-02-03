/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ACL_HCI_LINK_INTERFACE_H
#define ACL_HCI_LINK_INTERFACE_H

#include "bt_common.h"

void btm_acl_connected(BD_ADDR bda, UINT16 handle, UINT8 link_type, UINT8 enc_mode, UINT8 status);
void btm_acl_disconnected(UINT16 handle, UINT8 reason);

#endif /* ACL_HCI_LINK_INTERFACE_H */
