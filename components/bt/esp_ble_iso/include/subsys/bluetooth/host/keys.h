/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_SUBSYS_BLUETOOTH_HOST_KEYS_H_
#define ZEPHYR_SUBSYS_BLUETOOTH_HOST_KEYS_H_

#include <zephyr/bluetooth/bluetooth.h>

/** @cond INTERNAL_HIDDEN */

struct bt_ltk {
    uint8_t rand[8];
    uint8_t ediv[2];
    uint8_t val[16];
};

struct bt_keys {
    uint8_t       id;
    bt_addr_le_t  addr;
    uint16_t      keys;
    struct bt_ltk ltk;
};

/** @endcond */

#endif /* ZEPHYR_SUBSYS_BLUETOOTH_HOST_KEYS_H_ */
