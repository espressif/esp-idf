/*
 * SPDX-FileCopyrightText: 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>

/* NOTE: These helper functions always encodes into the same buffer storage.
 * It is the responsibility of the user of this function to copy the information
 * in this string if needed.
 *
 * NOTE: These functions are not thread-safe!
 */
const char *bt_hex(const void *buf, size_t len);
const char *bt_addr_str(const bt_addr_t *addr);
const char *bt_addr_le_str(const bt_addr_le_t *addr);
const char *bt_uuid_str(const struct bt_uuid *uuid);
