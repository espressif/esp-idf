/*
 * SPDX-FileCopyrightText: 2022 Stephanos Ioannidis <root@stephanos.io>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/bluetooth/addr.h>

const bt_addr_t bt_addr_any = { { 0, 0, 0, 0, 0, 0 } };
const bt_addr_t bt_addr_none = { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
const bt_addr_le_t bt_addr_le_any = { 0, { { 0, 0, 0, 0, 0, 0 } } };
const bt_addr_le_t bt_addr_le_none = { 0, { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } } };
